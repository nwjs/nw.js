// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sqlite_persistent_cookie_store.h"

#include <list>
#include <map>
#include <set>
#include <utility>

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/metrics/histogram.h"
#include "base/string_util.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time.h"
#include "content/nw/src/net/clear_on_exit_policy.h"
#include "content/public/browser/browser_thread.h"
#include "url/gurl.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/cookies/canonical_cookie.h"
#include "sql/error_delegate_util.h"
#include "sql/meta_table.h"
#include "sql/statement.h"
#include "sql/transaction.h"
#include "third_party/sqlite/sqlite3.h"

using base::Time;
using base::FilePath;
using content::BrowserThread;

// This class is designed to be shared between any calling threads and the
// database thread. It batches operations and commits them on a timer.
//
// SQLitePersistentCookieStore::Load is called to load all cookies.  It
// delegates to Backend::Load, which posts a Backend::LoadAndNotifyOnDBThread
// task to the DB thread.  This task calls Backend::ChainLoadCookies(), which
// repeatedly posts itself to the DB thread to load each eTLD+1's cookies in
// separate tasks.  When this is complete, Backend::CompleteLoadOnIOThread is
// posted to the IO thread, which notifies the caller of
// SQLitePersistentCookieStore::Load that the load is complete.
//
// If a priority load request is invoked via SQLitePersistentCookieStore::
// LoadCookiesForKey, it is delegated to Backend::LoadCookiesForKey, which posts
// Backend::LoadKeyAndNotifyOnDBThread to the DB thread. That routine loads just
// that single domain key (eTLD+1)'s cookies, and posts a Backend::
// CompleteLoadForKeyOnIOThread to the IO thread to notify the caller of
// SQLitePersistentCookieStore::LoadCookiesForKey that that load is complete.
//
// Subsequent to loading, mutations may be queued by any thread using
// AddCookie, UpdateCookieAccessTime, and DeleteCookie. These are flushed to
// disk on the DB thread every 30 seconds, 512 operations, or call to Flush(),
// whichever occurs first.
class SQLitePersistentCookieStore::Backend
    : public base::RefCountedThreadSafe<SQLitePersistentCookieStore::Backend> {
 public:
  Backend(const FilePath& path,
          bool restore_old_session_cookies,
          ClearOnExitPolicy* clear_on_exit_policy)
      : path_(path),
        db_(NULL),
        num_pending_(0),
        force_keep_session_state_(false),
        initialized_(false),
        corruption_detected_(false),
        restore_old_session_cookies_(restore_old_session_cookies),
        clear_on_exit_policy_(clear_on_exit_policy),
        num_cookies_read_(0),
        num_priority_waiting_(0),
        total_priority_requests_(0) {
  }

  // Creates or loads the SQLite database.
  void Load(const LoadedCallback& loaded_callback);

  // Loads cookies for the domain key (eTLD+1).
  void LoadCookiesForKey(const std::string& domain,
      const LoadedCallback& loaded_callback);

  // Batch a cookie addition.
  void AddCookie(const net::CanonicalCookie& cc);

  // Batch a cookie access time update.
  void UpdateCookieAccessTime(const net::CanonicalCookie& cc);

  // Batch a cookie deletion.
  void DeleteCookie(const net::CanonicalCookie& cc);

  // Commit pending operations as soon as possible.
  void Flush(const base::Closure& callback);

  // Commit any pending operations and close the database.  This must be called
  // before the object is destructed.
  void Close();

  void SetForceKeepSessionState();

 private:
  friend class base::RefCountedThreadSafe<SQLitePersistentCookieStore::Backend>;

  class KillDatabaseErrorDelegate : public sql::ErrorDelegate {
   public:
    KillDatabaseErrorDelegate(Backend* backend);

    virtual ~KillDatabaseErrorDelegate() {}

    // ErrorDelegate implementation.
    virtual int OnError(int error,
                        sql::Connection* connection,
                        sql::Statement* stmt) OVERRIDE;

   private:

    class HistogramUniquifier {
     public:
      static const char* name() { return "Sqlite.Cookie.Error"; }
    };

    // Do not increment the count on Backend, as that would create a circular
    // reference (Backend -> Connection -> ErrorDelegate -> Backend).
    Backend* backend_;

    // True if the delegate has previously attempted to kill the database.
    bool attempted_to_kill_database_;

    DISALLOW_COPY_AND_ASSIGN(KillDatabaseErrorDelegate);
  };

  // You should call Close() before destructing this object.
  ~Backend() {
    DCHECK(!db_.get()) << "Close should have already been called.";
    DCHECK(num_pending_ == 0 && pending_.empty());
  }

  // Database upgrade statements.
  bool EnsureDatabaseVersion();

  class PendingOperation {
   public:
    typedef enum {
      COOKIE_ADD,
      COOKIE_UPDATEACCESS,
      COOKIE_DELETE,
    } OperationType;

    PendingOperation(OperationType op, const net::CanonicalCookie& cc)
        : op_(op), cc_(cc) { }

    OperationType op() const { return op_; }
    const net::CanonicalCookie& cc() const { return cc_; }

   private:
    OperationType op_;
    net::CanonicalCookie cc_;
  };

 private:
  // Creates or loads the SQLite database on DB thread.
  void LoadAndNotifyOnDBThread(const LoadedCallback& loaded_callback,
                               const base::Time& posted_at);

  // Loads cookies for the domain key (eTLD+1) on DB thread.
  void LoadKeyAndNotifyOnDBThread(const std::string& domains,
                                  const LoadedCallback& loaded_callback,
                                  const base::Time& posted_at);

  // Notifies the CookieMonster when loading completes for a specific domain key
  // or for all domain keys. Triggers the callback and passes it all cookies
  // that have been loaded from DB since last IO notification.
  void Notify(const LoadedCallback& loaded_callback, bool load_success);

  // Sends notification when the entire store is loaded, and reports metrics
  // for the total time to load and aggregated results from any priority loads
  // that occurred.
  void CompleteLoadOnIOThread(const LoadedCallback& loaded_callback,
                              bool load_success);

  // Sends notification when a single priority load completes. Updates priority
  // load metric data. The data is sent only after the final load completes.
  void CompleteLoadForKeyOnIOThread(const LoadedCallback& loaded_callback,
                                    bool load_success);

  // Sends all metrics, including posting a ReportMetricsOnDBThread task.
  // Called after all priority and regular loading is complete.
  void ReportMetrics();

  // Sends DB-thread owned metrics (i.e., the combined duration of all DB-thread
  // tasks).
  void ReportMetricsOnDBThread();

  // Initialize the data base.
  bool InitializeDatabase();

  // Loads cookies for the next domain key from the DB, then either reschedules
  // itself or schedules the provided callback to run on the IO thread (if all
  // domains are loaded).
  void ChainLoadCookies(const LoadedCallback& loaded_callback);

  // Load all cookies for a set of domains/hosts
  bool LoadCookiesForDomains(const std::set<std::string>& key);

  // Batch a cookie operation (add or delete)
  void BatchOperation(PendingOperation::OperationType op,
                      const net::CanonicalCookie& cc);
  // Commit our pending operations to the database.
  void Commit();
  // Close() executed on the background thread.
  void InternalBackgroundClose();

  void DeleteSessionCookiesOnStartup();

  void DeleteSessionCookiesOnShutdown();

  void KillDatabase();
  void ScheduleKillDatabase();

  FilePath path_;
  scoped_ptr<sql::Connection> db_;
  sql::MetaTable meta_table_;

  typedef std::list<PendingOperation*> PendingOperationsList;
  PendingOperationsList pending_;
  PendingOperationsList::size_type num_pending_;
  // True if the persistent store should skip delete on exit rules.
  bool force_keep_session_state_;
  // Guard |cookies_|, |pending_|, |num_pending_|, |force_keep_session_state_|
  base::Lock lock_;

  // Temporary buffer for cookies loaded from DB. Accumulates cookies to reduce
  // the number of messages sent to the IO thread. Sent back in response to
  // individual load requests for domain keys or when all loading completes.
  std::vector<net::CanonicalCookie*> cookies_;

  // Map of domain keys(eTLD+1) to domains/hosts that are to be loaded from DB.
  std::map<std::string, std::set<std::string> > keys_to_load_;

  // Map of (domain keys(eTLD+1), is secure cookie) to number of cookies in the
  // database.
  typedef std::pair<std::string, bool> CookieOrigin;
  typedef std::map<CookieOrigin, int> CookiesPerOriginMap;
  CookiesPerOriginMap cookies_per_origin_;

  // Indicates if DB has been initialized.
  bool initialized_;

  // Indicates if the kill-database callback has been scheduled.
  bool corruption_detected_;

  // If false, we should filter out session cookies when reading the DB.
  bool restore_old_session_cookies_;

  // Policy defining what data is deleted on shutdown.
  scoped_refptr<ClearOnExitPolicy> clear_on_exit_policy_;

  // The cumulative time spent loading the cookies on the DB thread. Incremented
  // and reported from the DB thread.
  base::TimeDelta cookie_load_duration_;

  // The total number of cookies read. Incremented and reported on the DB
  // thread.
  int num_cookies_read_;

  // Guards the following metrics-related properties (only accessed when
  // starting/completing priority loads or completing the total load).
  base::Lock metrics_lock_;
  int num_priority_waiting_;
  // The total number of priority requests.
  int total_priority_requests_;
  // The time when |num_priority_waiting_| incremented to 1.
  base::Time current_priority_wait_start_;
  // The cumulative duration of time when |num_priority_waiting_| was greater
  // than 1.
  base::TimeDelta priority_wait_duration_;

  DISALLOW_COPY_AND_ASSIGN(Backend);
};

SQLitePersistentCookieStore::Backend::KillDatabaseErrorDelegate::
KillDatabaseErrorDelegate(Backend* backend)
    : backend_(backend),
      attempted_to_kill_database_(false) {
}

int SQLitePersistentCookieStore::Backend::KillDatabaseErrorDelegate::OnError(
    int error, sql::Connection* connection, sql::Statement* stmt) {
  sql::LogAndRecordErrorInHistogram<HistogramUniquifier>(error, connection);

  // Do not attempt to kill database more than once. If the first time failed,
  // it is unlikely that a second time will be successful.
  if (!attempted_to_kill_database_ && sql::IsErrorCatastrophic(error)) {
    attempted_to_kill_database_ = true;

    backend_->ScheduleKillDatabase();
  }

  return error;
}

// Version number of the database.
//
// Version 5 adds the columns has_expires and is_persistent, so that the
// database can store session cookies as well as persistent cookies. Databases
// of version 5 are incompatible with older versions of code. If a database of
// version 5 is read by older code, session cookies will be treated as normal
// cookies. Currently, these fields are written, but not read anymore.
//
// In version 4, we migrated the time epoch.  If you open the DB with an older
// version on Mac or Linux, the times will look wonky, but the file will likely
// be usable. On Windows version 3 and 4 are the same.
//
// Version 3 updated the database to include the last access time, so we can
// expire them in decreasing order of use when we've reached the maximum
// number of cookies.
static const int kCurrentVersionNumber = 5;
static const int kCompatibleVersionNumber = 5;

namespace {

// Increments a specified TimeDelta by the duration between this object's
// constructor and destructor. Not thread safe. Multiple instances may be
// created with the same delta instance as long as their lifetimes are nested.
// The shortest lived instances have no impact.
class IncrementTimeDelta {
 public:
  explicit IncrementTimeDelta(base::TimeDelta* delta) :
      delta_(delta),
      original_value_(*delta),
      start_(base::Time::Now()) {}

  ~IncrementTimeDelta() {
    *delta_ = original_value_ + base::Time::Now() - start_;
  }

 private:
  base::TimeDelta* delta_;
  base::TimeDelta original_value_;
  base::Time start_;

  DISALLOW_COPY_AND_ASSIGN(IncrementTimeDelta);
};

// Initializes the cookies table, returning true on success.
bool InitTable(sql::Connection* db) {
  if (!db->DoesTableExist("cookies")) {
    if (!db->Execute("CREATE TABLE cookies ("
                     "creation_utc INTEGER NOT NULL UNIQUE PRIMARY KEY,"
                     "host_key TEXT NOT NULL,"
                     "name TEXT NOT NULL,"
                     "value TEXT NOT NULL,"
                     "path TEXT NOT NULL,"
                     "expires_utc INTEGER NOT NULL,"
                     "secure INTEGER NOT NULL,"
                     "httponly INTEGER NOT NULL,"
                     "last_access_utc INTEGER NOT NULL, "
                     "has_expires INTEGER NOT NULL DEFAULT 1, "
                     "persistent INTEGER NOT NULL DEFAULT 1)"))
      return false;
  }

  // Older code created an index on creation_utc, which is already
  // primary key for the table.
  if (!db->Execute("DROP INDEX IF EXISTS cookie_times"))
    return false;

  if (!db->Execute("CREATE INDEX IF NOT EXISTS domain ON cookies(host_key)"))
    return false;

  return true;
}

}  // namespace

void SQLitePersistentCookieStore::Backend::Load(
    const LoadedCallback& loaded_callback) {
  // This function should be called only once per instance.
  DCHECK(!db_.get());
  BrowserThread::PostTask(
      BrowserThread::DB, FROM_HERE,
      base::Bind(&Backend::LoadAndNotifyOnDBThread, this, loaded_callback,
                 base::Time::Now()));
}

void SQLitePersistentCookieStore::Backend::LoadCookiesForKey(
    const std::string& key,
    const LoadedCallback& loaded_callback) {
  {
    base::AutoLock locked(metrics_lock_);
    if (num_priority_waiting_ == 0)
      current_priority_wait_start_ = base::Time::Now();
    num_priority_waiting_++;
    total_priority_requests_++;
  }

  BrowserThread::PostTask(
    BrowserThread::DB, FROM_HERE,
    base::Bind(&Backend::LoadKeyAndNotifyOnDBThread, this,
               key,
               loaded_callback,
               base::Time::Now()));
}

void SQLitePersistentCookieStore::Backend::LoadAndNotifyOnDBThread(
    const LoadedCallback& loaded_callback, const base::Time& posted_at) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  IncrementTimeDelta increment(&cookie_load_duration_);

  UMA_HISTOGRAM_CUSTOM_TIMES(
      "Cookie.TimeLoadDBQueueWait",
      base::Time::Now() - posted_at,
      base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
      50);

  if (!InitializeDatabase()) {
    BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&SQLitePersistentCookieStore::Backend::CompleteLoadOnIOThread,
                 this, loaded_callback, false));
  } else {
    ChainLoadCookies(loaded_callback);
  }
}

void SQLitePersistentCookieStore::Backend::LoadKeyAndNotifyOnDBThread(
    const std::string& key,
    const LoadedCallback& loaded_callback,
    const base::Time& posted_at) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  IncrementTimeDelta increment(&cookie_load_duration_);

  UMA_HISTOGRAM_CUSTOM_TIMES(
      "Cookie.TimeKeyLoadDBQueueWait",
      base::Time::Now() - posted_at,
      base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
      50);

  bool success = false;
  if (InitializeDatabase()) {
    std::map<std::string, std::set<std::string> >::iterator
      it = keys_to_load_.find(key);
    if (it != keys_to_load_.end()) {
      success = LoadCookiesForDomains(it->second);
      keys_to_load_.erase(it);
    } else {
      success = true;
    }
  }

  BrowserThread::PostTask(
    BrowserThread::IO, FROM_HERE,
    base::Bind(
        &SQLitePersistentCookieStore::Backend::CompleteLoadForKeyOnIOThread,
        this, loaded_callback, success));
}

void SQLitePersistentCookieStore::Backend::CompleteLoadForKeyOnIOThread(
    const LoadedCallback& loaded_callback,
    bool load_success) {
  Notify(loaded_callback, load_success);

  {
    base::AutoLock locked(metrics_lock_);
    num_priority_waiting_--;
    if (num_priority_waiting_ == 0) {
      priority_wait_duration_ +=
          base::Time::Now() - current_priority_wait_start_;
    }
  }

}

void SQLitePersistentCookieStore::Backend::ReportMetricsOnDBThread() {
  UMA_HISTOGRAM_CUSTOM_TIMES(
      "Cookie.TimeLoad",
      cookie_load_duration_,
      base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
      50);
}

void SQLitePersistentCookieStore::Backend::ReportMetrics() {
  BrowserThread::PostTask(BrowserThread::DB, FROM_HERE, base::Bind(
      &SQLitePersistentCookieStore::Backend::ReportMetricsOnDBThread, this));

  {
    base::AutoLock locked(metrics_lock_);
    UMA_HISTOGRAM_CUSTOM_TIMES(
        "Cookie.PriorityBlockingTime",
        priority_wait_duration_,
        base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
        50);

    UMA_HISTOGRAM_COUNTS_100(
        "Cookie.PriorityLoadCount",
        total_priority_requests_);

    UMA_HISTOGRAM_COUNTS_10000(
        "Cookie.NumberOfLoadedCookies",
        num_cookies_read_);
  }
}

void SQLitePersistentCookieStore::Backend::CompleteLoadOnIOThread(
    const LoadedCallback& loaded_callback, bool load_success) {
  Notify(loaded_callback, load_success);

  if (load_success)
    ReportMetrics();
}

void SQLitePersistentCookieStore::Backend::Notify(
    const LoadedCallback& loaded_callback,
    bool load_success) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  std::vector<net::CanonicalCookie*> cookies;
  {
    base::AutoLock locked(lock_);
    cookies.swap(cookies_);
  }

  loaded_callback.Run(cookies);
}

bool SQLitePersistentCookieStore::Backend::InitializeDatabase() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  if (initialized_ || corruption_detected_) {
    // Return false if we were previously initialized but the DB has since been
    // closed, or if corruption caused a database reset during initialization.
    return db_ != NULL;
  }

  base::Time start = base::Time::Now();

  const FilePath dir = path_.DirName();
  if (!file_util::PathExists(dir) && !file_util::CreateDirectory(dir)) {
    return false;
  }

  int64 db_size = 0;
  if (file_util::GetFileSize(path_, &db_size)) {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    UMA_HISTOGRAM_COUNTS("Cookie.DBSizeInKB", db_size / 1024 );
  }

  db_.reset(new sql::Connection);
  db_->set_error_delegate(new KillDatabaseErrorDelegate(this));

  if (!db_->Open(path_)) {
    NOTREACHED() << "Unable to open cookie DB.";
    if (corruption_detected_)
      db_->Raze();
    meta_table_.Reset();
    db_.reset();
    return false;
  }

  if (!EnsureDatabaseVersion() || !InitTable(db_.get())) {
    NOTREACHED() << "Unable to open cookie DB.";
    if (corruption_detected_)
      db_->Raze();
    meta_table_.Reset();
    db_.reset();
    return false;
  }

  UMA_HISTOGRAM_CUSTOM_TIMES(
    "Cookie.TimeInitializeDB",
    base::Time::Now() - start,
    base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
    50);

  start = base::Time::Now();

  // Retrieve all the domains
  sql::Statement smt(db_->GetUniqueStatement(
    "SELECT DISTINCT host_key FROM cookies"));

  if (!smt.is_valid()) {
    if (corruption_detected_)
      db_->Raze();
    meta_table_.Reset();
    db_.reset();
    return false;
  }

  // Build a map of domain keys (always eTLD+1) to domains.
  while (smt.Step()) {
    std::string domain = smt.ColumnString(0);
    std::string key =
      net::RegistryControlledDomainService::GetDomainAndRegistry(domain);

    std::map<std::string, std::set<std::string> >::iterator it =
      keys_to_load_.find(key);
    if (it == keys_to_load_.end())
      it = keys_to_load_.insert(std::make_pair
                                (key, std::set<std::string>())).first;
    it->second.insert(domain);
  }

  UMA_HISTOGRAM_CUSTOM_TIMES(
    "Cookie.TimeInitializeDomainMap",
    base::Time::Now() - start,
    base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromMinutes(1),
    50);

  initialized_ = true;
  return true;
}

void SQLitePersistentCookieStore::Backend::ChainLoadCookies(
    const LoadedCallback& loaded_callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  IncrementTimeDelta increment(&cookie_load_duration_);

  bool load_success = true;

  if (!db_.get()) {
    // Close() has been called on this store.
    load_success = false;
  } else if (keys_to_load_.size() > 0) {
    // Load cookies for the first domain key.
    std::map<std::string, std::set<std::string> >::iterator
      it = keys_to_load_.begin();
    load_success = LoadCookiesForDomains(it->second);
    keys_to_load_.erase(it);
  }

  // If load is successful and there are more domain keys to be loaded,
  // then post a DB task to continue chain-load;
  // Otherwise notify on IO thread.
  if (load_success && keys_to_load_.size() > 0) {
    BrowserThread::PostTask(
      BrowserThread::DB, FROM_HERE,
      base::Bind(&Backend::ChainLoadCookies, this, loaded_callback));
  } else {
    BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&SQLitePersistentCookieStore::Backend::CompleteLoadOnIOThread,
                 this, loaded_callback, load_success));
    if (load_success && !restore_old_session_cookies_)
      DeleteSessionCookiesOnStartup();
  }
}

bool SQLitePersistentCookieStore::Backend::LoadCookiesForDomains(
  const std::set<std::string>& domains) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  sql::Statement smt;
  if (restore_old_session_cookies_) {
    smt.Assign(db_->GetCachedStatement(
      SQL_FROM_HERE,
      "SELECT creation_utc, host_key, name, value, path, expires_utc, "
      "secure, httponly, last_access_utc, has_expires, persistent "
      "FROM cookies WHERE host_key = ?"));
  } else {
    smt.Assign(db_->GetCachedStatement(
      SQL_FROM_HERE,
      "SELECT creation_utc, host_key, name, value, path, expires_utc, "
      "secure, httponly, last_access_utc, has_expires, persistent "
      "FROM cookies WHERE host_key = ? AND persistent = 1"));
  }
  if (!smt.is_valid()) {
    smt.Clear();  // Disconnect smt_ref from db_.
    meta_table_.Reset();
    db_.reset();
    return false;
  }

  std::vector<net::CanonicalCookie*> cookies;
  std::set<std::string>::const_iterator it = domains.begin();
  for (; it != domains.end(); ++it) {
    smt.BindString(0, *it);
    while (smt.Step()) {
      scoped_ptr<net::CanonicalCookie> cc(
          new net::CanonicalCookie(
              // The "source" URL is not used with persisted cookies.
              GURL(),                                         // Source
              smt.ColumnString(2),                            // name
              smt.ColumnString(3),                            // value
              smt.ColumnString(1),                            // domain
              smt.ColumnString(4),                            // path
              Time::FromInternalValue(smt.ColumnInt64(0)),    // creation_utc
              Time::FromInternalValue(smt.ColumnInt64(5)),    // expires_utc
              Time::FromInternalValue(smt.ColumnInt64(8)),    // last_access_utc
              smt.ColumnInt(6) != 0,                          // secure
              smt.ColumnInt(7) != 0));                        // httponly
      DLOG_IF(WARNING,
              cc->CreationDate() > Time::Now()) << L"CreationDate too recent";
      cookies_per_origin_[CookieOrigin(cc->Domain(), cc->IsSecure())]++;
      cookies.push_back(cc.release());
      ++num_cookies_read_;
    }
    smt.Reset(true);
  }
  {
    base::AutoLock locked(lock_);
    cookies_.insert(cookies_.end(), cookies.begin(), cookies.end());
  }
  return true;
}

bool SQLitePersistentCookieStore::Backend::EnsureDatabaseVersion() {
  // Version check.
  if (!meta_table_.Init(
      db_.get(), kCurrentVersionNumber, kCompatibleVersionNumber)) {
    return false;
  }

  if (meta_table_.GetCompatibleVersionNumber() > kCurrentVersionNumber) {
    LOG(WARNING) << "Cookie database is too new.";
    return false;
  }

  int cur_version = meta_table_.GetVersionNumber();
  if (cur_version == 2) {
    sql::Transaction transaction(db_.get());
    if (!transaction.Begin())
      return false;
    if (!db_->Execute("ALTER TABLE cookies ADD COLUMN last_access_utc "
                     "INTEGER DEFAULT 0") ||
        !db_->Execute("UPDATE cookies SET last_access_utc = creation_utc")) {
      LOG(WARNING) << "Unable to update cookie database to version 3.";
      return false;
    }
    ++cur_version;
    meta_table_.SetVersionNumber(cur_version);
    meta_table_.SetCompatibleVersionNumber(
        std::min(cur_version, kCompatibleVersionNumber));
    transaction.Commit();
  }

  if (cur_version == 3) {
    // The time epoch changed for Mac & Linux in this version to match Windows.
    // This patch came after the main epoch change happened, so some
    // developers have "good" times for cookies added by the more recent
    // versions. So we have to be careful to only update times that are under
    // the old system (which will appear to be from before 1970 in the new
    // system). The magic number used below is 1970 in our time units.
    sql::Transaction transaction(db_.get());
    transaction.Begin();
#if !defined(OS_WIN)
    ignore_result(db_->Execute(
        "UPDATE cookies "
        "SET creation_utc = creation_utc + 11644473600000000 "
        "WHERE rowid IN "
        "(SELECT rowid FROM cookies WHERE "
          "creation_utc > 0 AND creation_utc < 11644473600000000)"));
    ignore_result(db_->Execute(
        "UPDATE cookies "
        "SET expires_utc = expires_utc + 11644473600000000 "
        "WHERE rowid IN "
        "(SELECT rowid FROM cookies WHERE "
          "expires_utc > 0 AND expires_utc < 11644473600000000)"));
    ignore_result(db_->Execute(
        "UPDATE cookies "
        "SET last_access_utc = last_access_utc + 11644473600000000 "
        "WHERE rowid IN "
        "(SELECT rowid FROM cookies WHERE "
          "last_access_utc > 0 AND last_access_utc < 11644473600000000)"));
#endif
    ++cur_version;
    meta_table_.SetVersionNumber(cur_version);
    transaction.Commit();
  }

  if (cur_version == 4) {
    const base::TimeTicks start_time = base::TimeTicks::Now();
    sql::Transaction transaction(db_.get());
    if (!transaction.Begin())
      return false;
    if (!db_->Execute("ALTER TABLE cookies "
                      "ADD COLUMN has_expires INTEGER DEFAULT 1") ||
        !db_->Execute("ALTER TABLE cookies "
                      "ADD COLUMN persistent INTEGER DEFAULT 1")) {
      LOG(WARNING) << "Unable to update cookie database to version 5.";
      return false;
    }
    ++cur_version;
    meta_table_.SetVersionNumber(cur_version);
    meta_table_.SetCompatibleVersionNumber(
        std::min(cur_version, kCompatibleVersionNumber));
    transaction.Commit();
    UMA_HISTOGRAM_TIMES("Cookie.TimeDatabaseMigrationToV5",
                        base::TimeTicks::Now() - start_time);
  }

  // Put future migration cases here.

  if (cur_version < kCurrentVersionNumber) {
    UMA_HISTOGRAM_COUNTS_100("Cookie.CorruptMetaTable", 1);

    meta_table_.Reset();
    db_.reset(new sql::Connection);
    if (!file_util::Delete(path_, false) ||
        !db_->Open(path_) ||
        !meta_table_.Init(
            db_.get(), kCurrentVersionNumber, kCompatibleVersionNumber)) {
      UMA_HISTOGRAM_COUNTS_100("Cookie.CorruptMetaTableRecoveryFailed", 1);
      NOTREACHED() << "Unable to reset the cookie DB.";
      meta_table_.Reset();
      db_.reset();
      return false;
    }
  }

  return true;
}

void SQLitePersistentCookieStore::Backend::AddCookie(
    const net::CanonicalCookie& cc) {
  BatchOperation(PendingOperation::COOKIE_ADD, cc);
}

void SQLitePersistentCookieStore::Backend::UpdateCookieAccessTime(
    const net::CanonicalCookie& cc) {
  BatchOperation(PendingOperation::COOKIE_UPDATEACCESS, cc);
}

void SQLitePersistentCookieStore::Backend::DeleteCookie(
    const net::CanonicalCookie& cc) {
  BatchOperation(PendingOperation::COOKIE_DELETE, cc);
}

void SQLitePersistentCookieStore::Backend::BatchOperation(
    PendingOperation::OperationType op,
    const net::CanonicalCookie& cc) {
  // Commit every 30 seconds.
  static const int kCommitIntervalMs = 30 * 1000;
  // Commit right away if we have more than 512 outstanding operations.
  static const size_t kCommitAfterBatchSize = 512;
  DCHECK(!BrowserThread::CurrentlyOn(BrowserThread::DB));

  // We do a full copy of the cookie here, and hopefully just here.
  scoped_ptr<PendingOperation> po(new PendingOperation(op, cc));

  PendingOperationsList::size_type num_pending;
  {
    base::AutoLock locked(lock_);
    pending_.push_back(po.release());
    num_pending = ++num_pending_;
  }

  if (num_pending == 1) {
    // We've gotten our first entry for this batch, fire off the timer.
    BrowserThread::PostDelayedTask(
        BrowserThread::DB, FROM_HERE,
        base::Bind(&Backend::Commit, this),
        base::TimeDelta::FromMilliseconds(kCommitIntervalMs));
  } else if (num_pending == kCommitAfterBatchSize) {
    // We've reached a big enough batch, fire off a commit now.
    BrowserThread::PostTask(
        BrowserThread::DB, FROM_HERE,
        base::Bind(&Backend::Commit, this));
  }
}

void SQLitePersistentCookieStore::Backend::Commit() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  PendingOperationsList ops;
  {
    base::AutoLock locked(lock_);
    pending_.swap(ops);
    num_pending_ = 0;
  }

  // Maybe an old timer fired or we are already Close()'ed.
  if (!db_.get() || ops.empty())
    return;

  sql::Statement add_smt(db_->GetCachedStatement(SQL_FROM_HERE,
      "INSERT INTO cookies (creation_utc, host_key, name, value, path, "
      "expires_utc, secure, httponly, last_access_utc, has_expires, "
      "persistent) "
      "VALUES (?,?,?,?,?,?,?,?,?,?,?)"));
  if (!add_smt.is_valid())
    return;

  sql::Statement update_access_smt(db_->GetCachedStatement(SQL_FROM_HERE,
      "UPDATE cookies SET last_access_utc=? WHERE creation_utc=?"));
  if (!update_access_smt.is_valid())
    return;

  sql::Statement del_smt(db_->GetCachedStatement(SQL_FROM_HERE,
                         "DELETE FROM cookies WHERE creation_utc=?"));
  if (!del_smt.is_valid())
    return;

  sql::Transaction transaction(db_.get());
  if (!transaction.Begin())
    return;

  for (PendingOperationsList::iterator it = ops.begin();
       it != ops.end(); ++it) {
    // Free the cookies as we commit them to the database.
    scoped_ptr<PendingOperation> po(*it);
    switch (po->op()) {
      case PendingOperation::COOKIE_ADD:
        cookies_per_origin_[
            CookieOrigin(po->cc().Domain(), po->cc().IsSecure())]++;
        add_smt.Reset(true);
        add_smt.BindInt64(0, po->cc().CreationDate().ToInternalValue());
        add_smt.BindString(1, po->cc().Domain());
        add_smt.BindString(2, po->cc().Name());
        add_smt.BindString(3, po->cc().Value());
        add_smt.BindString(4, po->cc().Path());
        add_smt.BindInt64(5, po->cc().ExpiryDate().ToInternalValue());
        add_smt.BindInt(6, po->cc().IsSecure());
        add_smt.BindInt(7, po->cc().IsHttpOnly());
        add_smt.BindInt64(8, po->cc().LastAccessDate().ToInternalValue());
        add_smt.BindInt(9, po->cc().IsPersistent());
        add_smt.BindInt(10, po->cc().IsPersistent());
        if (!add_smt.Run())
          NOTREACHED() << "Could not add a cookie to the DB.";
        break;

      case PendingOperation::COOKIE_UPDATEACCESS:
        update_access_smt.Reset(true);
        update_access_smt.BindInt64(0,
            po->cc().LastAccessDate().ToInternalValue());
        update_access_smt.BindInt64(1,
            po->cc().CreationDate().ToInternalValue());
        if (!update_access_smt.Run())
          NOTREACHED() << "Could not update cookie last access time in the DB.";
        break;

      case PendingOperation::COOKIE_DELETE:
        cookies_per_origin_[
            CookieOrigin(po->cc().Domain(), po->cc().IsSecure())]--;
        del_smt.Reset(true);
        del_smt.BindInt64(0, po->cc().CreationDate().ToInternalValue());
        if (!del_smt.Run())
          NOTREACHED() << "Could not delete a cookie from the DB.";
        break;

      default:
        NOTREACHED();
        break;
    }
  }
  bool succeeded = transaction.Commit();
  UMA_HISTOGRAM_ENUMERATION("Cookie.BackingStoreUpdateResults",
                            succeeded ? 0 : 1, 2);
}

void SQLitePersistentCookieStore::Backend::Flush(
    const base::Closure& callback) {
  DCHECK(!BrowserThread::CurrentlyOn(BrowserThread::DB));
  BrowserThread::PostTask(
      BrowserThread::DB, FROM_HERE, base::Bind(&Backend::Commit, this));
  if (!callback.is_null()) {
    // We want the completion task to run immediately after Commit() returns.
    // Posting it from here means there is less chance of another task getting
    // onto the message queue first, than if we posted it from Commit() itself.
    BrowserThread::PostTask(BrowserThread::DB, FROM_HERE, callback);
  }
}

// Fire off a close message to the background thread.  We could still have a
// pending commit timer or Load operations holding references on us, but if/when
// this fires we will already have been cleaned up and it will be ignored.
void SQLitePersistentCookieStore::Backend::Close() {
  if (BrowserThread::CurrentlyOn(BrowserThread::DB)) {
    InternalBackgroundClose();
  } else {
    // Must close the backend on the background thread.
    BrowserThread::PostTask(
        BrowserThread::DB, FROM_HERE,
        base::Bind(&Backend::InternalBackgroundClose, this));
  }
}

void SQLitePersistentCookieStore::Backend::InternalBackgroundClose() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  // Commit any pending operations
  Commit();

  if (!force_keep_session_state_ && clear_on_exit_policy_.get() &&
      clear_on_exit_policy_->HasClearOnExitOrigins()) {
    DeleteSessionCookiesOnShutdown();
  }

  meta_table_.Reset();
  db_.reset();
}

void SQLitePersistentCookieStore::Backend::DeleteSessionCookiesOnShutdown() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  if (!db_.get())
    return;

  sql::Statement del_smt(db_->GetCachedStatement(
      SQL_FROM_HERE, "DELETE FROM cookies WHERE host_key=? AND secure=?"));
  if (!del_smt.is_valid()) {
    LOG(WARNING) << "Unable to delete cookies on shutdown.";
    return;
  }

  sql::Transaction transaction(db_.get());
  if (!transaction.Begin()) {
    LOG(WARNING) << "Unable to delete cookies on shutdown.";
    return;
  }

  for (CookiesPerOriginMap::iterator it = cookies_per_origin_.begin();
       it != cookies_per_origin_.end(); ++it) {
    if (it->second <= 0) {
      DCHECK_EQ(0, it->second);
      continue;
    }
    if (!clear_on_exit_policy_->ShouldClearOriginOnExit(it->first.first,
                                                        it->first.second)) {
      continue;
    }

    del_smt.Reset(true);
    del_smt.BindString(0, it->first.first);
    del_smt.BindInt(1, it->first.second);
    if (!del_smt.Run())
      NOTREACHED() << "Could not delete a cookie from the DB.";
  }

  if (!transaction.Commit())
    LOG(WARNING) << "Unable to delete cookies on shutdown.";
}

void SQLitePersistentCookieStore::Backend::ScheduleKillDatabase() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  corruption_detected_ = true;

  // Don't just do the close/delete here, as we are being called by |db| and
  // that seems dangerous.
  MessageLoop::current()->PostTask(
      FROM_HERE, base::Bind(&Backend::KillDatabase, this));
}

void SQLitePersistentCookieStore::Backend::KillDatabase() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));

  if (db_.get()) {
    // This Backend will now be in-memory only. In a future run we will recreate
    // the database. Hopefully things go better then!
    bool success = db_->Raze();
    UMA_HISTOGRAM_BOOLEAN("Cookie.KillDatabaseResult", success);
    db_->Close();
    meta_table_.Reset();
    db_.reset();
  }
}

void SQLitePersistentCookieStore::Backend::SetForceKeepSessionState() {
  base::AutoLock locked(lock_);
  force_keep_session_state_ = true;
}

void SQLitePersistentCookieStore::Backend::DeleteSessionCookiesOnStartup() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  if (!db_->Execute("DELETE FROM cookies WHERE persistent == 0"))
    LOG(WARNING) << "Unable to delete session cookies.";
}

SQLitePersistentCookieStore::SQLitePersistentCookieStore(
    const FilePath& path,
    bool restore_old_session_cookies,
    ClearOnExitPolicy* clear_on_exit_policy)
    : backend_(
        new Backend(path, restore_old_session_cookies, clear_on_exit_policy)) {
}

void SQLitePersistentCookieStore::Load(const LoadedCallback& loaded_callback) {
  backend_->Load(loaded_callback);
}

void SQLitePersistentCookieStore::LoadCookiesForKey(
    const std::string& key,
    const LoadedCallback& loaded_callback) {
  backend_->LoadCookiesForKey(key, loaded_callback);
}

void SQLitePersistentCookieStore::AddCookie(const net::CanonicalCookie& cc) {
  if (backend_.get())
    backend_->AddCookie(cc);
}

void SQLitePersistentCookieStore::UpdateCookieAccessTime(
    const net::CanonicalCookie& cc) {
  if (backend_.get())
    backend_->UpdateCookieAccessTime(cc);
}

void SQLitePersistentCookieStore::DeleteCookie(const net::CanonicalCookie& cc) {
  if (backend_.get())
    backend_->DeleteCookie(cc);
}

void SQLitePersistentCookieStore::SetForceKeepSessionState() {
  if (backend_.get())
    backend_->SetForceKeepSessionState();
}

void SQLitePersistentCookieStore::Flush(const base::Closure& callback) {
  if (backend_.get())
    backend_->Flush(callback);
  else if (!callback.is_null())
    MessageLoop::current()->PostTask(FROM_HERE, callback);
}

SQLitePersistentCookieStore::~SQLitePersistentCookieStore() {
  if (backend_.get()) {
    backend_->Close();
    // Release our reference, it will probably still have a reference if the
    // background thread has not run Close() yet.
    backend_ = NULL;
  }
}

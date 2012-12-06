// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A sqlite implementation of a cookie monster persistent store.

#ifndef CHROME_BROWSER_NET_SQLITE_PERSISTENT_COOKIE_STORE_H_
#define CHROME_BROWSER_NET_SQLITE_PERSISTENT_COOKIE_STORE_H_

#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "net/cookies/cookie_monster.h"

class ClearOnExitPolicy;
class FilePath;
class Task;

namespace net {
class CanonicalCookie;
}

// Implements the PersistentCookieStore interface in terms of a SQLite database.
// For documentation about the actual member functions consult the documentation
// of the parent class |net::CookieMonster::PersistentCookieStore|.
// If provided, a |ClearOnExitPolicy| is consulted when the SQLite database is
// closed to decide which cookies to keep.
class SQLitePersistentCookieStore
    : public net::CookieMonster::PersistentCookieStore {
 public:
  // If non-NULL, SQLitePersistentCookieStore will keep a scoped_refptr to the
  // |clear_on_exit_policy| throughout its lifetime.
  SQLitePersistentCookieStore(
      const FilePath& path,
      bool restore_old_session_cookies,
      ClearOnExitPolicy* clear_on_exit_policy);

  // net::CookieMonster::PersistentCookieStore:
  virtual void Load(const LoadedCallback& loaded_callback) OVERRIDE;
  virtual void LoadCookiesForKey(const std::string& key,
      const LoadedCallback& callback) OVERRIDE;
  virtual void AddCookie(const net::CanonicalCookie& cc) OVERRIDE;
  virtual void UpdateCookieAccessTime(const net::CanonicalCookie& cc) OVERRIDE;
  virtual void DeleteCookie(const net::CanonicalCookie& cc) OVERRIDE;
  virtual void SetForceKeepSessionState() OVERRIDE;
  virtual void Flush(const base::Closure& callback) OVERRIDE;

 protected:
   virtual ~SQLitePersistentCookieStore();

 private:
  class Backend;

  scoped_refptr<Backend> backend_;

  DISALLOW_COPY_AND_ASSIGN(SQLitePersistentCookieStore);
};

#endif  // CHROME_BROWSER_NET_SQLITE_PERSISTENT_COOKIE_STORE_H_

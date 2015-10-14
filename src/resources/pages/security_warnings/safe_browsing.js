// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Must match the commands handled by SafeBrowsingBlockingPage::CommandReceived.
var SB_CMD_DO_REPORT = 'doReport';
var SB_CMD_DONT_REPORT = 'dontReport';
var SB_CMD_EXPANDED_SEE_MORE = 'expandedSeeMore';
var SB_CMD_LEARN_MORE_2 = 'learnMore2';
var SB_CMD_PROCEED = 'proceed';
var SB_CMD_SHOW_DIAGNOSTIC = 'showDiagnostic';
var SB_CMD_SHOW_PRIVACY = 'showPrivacy';
var SB_CMD_TAKE_ME_BACK = 'takeMeBack';

// Other constants defined in safe_browsing_blocking_page.cc.
var SB_BOX_CHECKED = 'boxchecked';
var SB_DISPLAY_CHECK_BOX = 'displaycheckbox';

// This sets up the Extended Safe Browsing Reporting opt-in.
function setupCheckbox() {
  if (loadTimeData.getString('type') != 'SAFEBROWSING' ||
      !loadTimeData.getBoolean(SB_DISPLAY_CHECK_BOX)) {
    return;
  }

  $('opt-in-label').innerHTML = loadTimeData.getString('optInLink');
  $('opt-in-checkbox').checked = loadTimeData.getBoolean(SB_BOX_CHECKED);
  $('malware-opt-in').classList.remove('hidden');

  $('opt-in-checkbox').addEventListener('click', function() {
    sendCommand(
        $('opt-in-checkbox').checked ? SB_CMD_DO_REPORT : SB_CMD_DONT_REPORT);
  });
}

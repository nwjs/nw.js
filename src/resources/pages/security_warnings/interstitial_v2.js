// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This is the shared code for the new (Chrome 37) security interstitials. It is
// used for both SSL interstitials and Safe Browsing interstitials.

var expandedDetails = false;
var keyPressState = 0;

/**
 * A convenience method for sending commands to the parent page.
 * @param {string} cmd  The command to send.
 */
function sendCommand(cmd) {
  window.domAutomationController.setAutomationId(1);
  window.domAutomationController.send(cmd);
}

/**
 * This allows errors to be skippped by typing "danger" into the page.
 * @param {string} e The key that was just pressed.
 */
function handleKeypress(e) {
  var BYPASS_SEQUENCE = 'danger';
  if (BYPASS_SEQUENCE.charCodeAt(keyPressState) == e.keyCode) {
    keyPressState++;
    if (keyPressState == BYPASS_SEQUENCE.length) {
      sendCommand(SSL_CMD_PROCEED);
      keyPressState = 0;
    }
  } else {
    keyPressState = 0;
  }
}

/**
 * This appends a piece of debugging information to the end of the warning.
 * When complete, the caller must also make the debugging div
 * (error-debugging-info) visible.
 * @param {string} title  The name of this debugging field.
 * @param {string} value  The value of the debugging field.
 */
function appendDebuggingField(title, value) {
  // The values input here are not trusted. Never use innerHTML on these
  // values!
  var spanTitle = document.createElement('span');
  spanTitle.classList.add('debugging-title');
  spanTitle.innerText = title + ': ';

  var spanValue = document.createElement('span');
  spanValue.classList.add('debugging-value');
  spanValue.innerText = value;

  var pElem = document.createElement('p');
  pElem.classList.add('debugging-content');
  pElem.appendChild(spanTitle);
  pElem.appendChild(spanValue);
  $('error-debugging-info').appendChild(pElem);
}

function toggleDebuggingInfo() {
  $('error-debugging-info').classList.toggle('hidden');
}

function setupEvents() {
  var overridable = loadTimeData.getBoolean('overridable');
  var interstitialType = loadTimeData.getString('type');
  var ssl = interstitialType == 'SSL';
  var captivePortal = interstitialType == 'CAPTIVE_PORTAL';
  var badClock = ssl && loadTimeData.getBoolean('bad_clock');
  var hidePrimaryButton = badClock && loadTimeData.getBoolean(
      'hide_primary_button');

  if (ssl) {
    $('body').classList.add(badClock ? 'bad-clock' : 'ssl');
    $('error-code').textContent = loadTimeData.getString('errorCode');
    $('error-code').classList.remove('hidden');
  } else if (captivePortal) {
    $('body').classList.add('captive-portal');
  } else {
    $('body').classList.add('safe-browsing');
  }

  if (hidePrimaryButton) {
    $('primary-button').classList.add('hidden');
  } else {
    $('primary-button').addEventListener('click', function() {
      switch (interstitialType) {
        case 'CAPTIVE_PORTAL':
          sendCommand(CAPTIVEPORTAL_CMD_OPEN_LOGIN_PAGE);
          break;

        case 'SSL':
          if (badClock)
            sendCommand(SSL_CMD_CLOCK);
          else if (overridable)
            sendCommand(SSL_CMD_DONT_PROCEED);
          else
            sendCommand(SSL_CMD_RELOAD);
          break;

        case 'SAFEBROWSING':
          sendCommand(SB_CMD_TAKE_ME_BACK);
          break;

        default:
          throw 'Invalid interstitial type';
      }
    });
  }

  if (overridable) {
    // Captive portal page isn't overridable.
    $('proceed-link').addEventListener('click', function(event) {
      sendCommand(ssl ? SSL_CMD_PROCEED : SB_CMD_PROCEED);
    });
  } else if (!ssl) {
    $('final-paragraph').classList.add('hidden');
  }

  if (ssl && overridable) {
    $('proceed-link').classList.add('small-link');
  } else if ($('help-link')) {
    // Overridable SSL page doesn't have this link.
    $('help-link').addEventListener('click', function(event) {
      if (ssl)
        sendCommand(SSL_CMD_HELP);
      else if (loadTimeData.getBoolean('phishing'))
        sendCommand(SB_CMD_LEARN_MORE_2);
      else
        sendCommand(SB_CMD_SHOW_DIAGNOSTIC);
    });
  }

  if (captivePortal) {
    // Captive portal page doesn't have details button.
    $('details-button').classList.add('hidden');
  } else {
    $('details-button').addEventListener('click', function(event) {
      var hiddenDetails = $('details').classList.toggle('hidden');
      $('details-button').innerText = hiddenDetails ?
          loadTimeData.getString('openDetails') :
          loadTimeData.getString('closeDetails');
      if (!expandedDetails) {
        // Record a histogram entry only the first time that details is opened.
        sendCommand(ssl ? SSL_CMD_MORE : SB_CMD_EXPANDED_SEE_MORE);
        expandedDetails = true;
      }
    });
  }

  preventDefaultOnPoundLinkClicks();
  setupCheckbox();
  setupSSLDebuggingInfo();
  document.addEventListener('keypress', handleKeypress);
}

document.addEventListener('DOMContentLoaded', setupEvents);

#!/usr/bin/env python
# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Utility functions for dealing with crash reports."""

import logging
import os
_LOGGER = logging.getLogger(os.path.basename(__file__))


def LogCrashKeys(report):
  for key in sorted(report.keys()):
    val = report[key][0]
    if (len(val) < 64):
      _LOGGER.debug('Got crashkey "%s": "%s"', key, val)
    else:
      _LOGGER.debug('Got crashkey "%s": ...%d bytes...', key, len(val))


def ValidateCrashReport(report, expectations=None):
  expected_keys = {}

  # Merge in additional expectations.
  if expectations:
    for key, value in expectations.items():
      expected_keys[key] = value

  # Validate the expectations.
  missing_keys = False
  for expected_key, error in expected_keys.items():
    if expected_key not in report:
      _LOGGER.error('Missing expected "%s" crash key.', expected_key)
      _LOGGER.error('"%s" integration appears broken.', error)
      missing_keys = True

  if missing_keys:
    raise Exception('Missing expected crash keys.')

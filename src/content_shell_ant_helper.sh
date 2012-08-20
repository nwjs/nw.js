#!/bin/sh

# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -eu

# A helper script to help convert the values of the fifth parameter for
# ant building: 'Debug' -> 'debug'; 'Release' -> 'release'.
# It's used by content/content_shell.gypi

buildtype="$(echo "${11}" | tr '[A-Z]' '[a-z]')"

# TODO(yongsheng): enable build type by adding "${buildtype}" into the
# end of parameter list once the workflow is ready to support release
# mode.
exec "${1}" "${2}" "${3}" "${4}" "${5}" "${6}" "${7}" "${8}" "${9}" "${10}"

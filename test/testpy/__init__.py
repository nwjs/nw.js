# Copyright 2008 the V8 project authors. All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of Google Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import test
import os
import shutil
from shutil import rmtree
from os import mkdir
from glob import glob
from os.path import join, dirname, exists
import re
import json
from functools import reduce

FLAGS_PATTERN = re.compile(r"//\s+Flags:(.*)")
FILES_PATTERN = re.compile(r"//\s+Files:(.*)")


class SimpleTestCase(test.TestCase):

  def __init__(self, path, file, arch, mode, nwdir, context, config, additional=[]):
    super(SimpleTestCase, self).__init__(context, path, arch, mode, nwdir)
    self.file = file
    self.config = config
    self.arch = arch
    self.mode = mode
    self.tmpdir = join(dirname(self.config.root), 'tmp')
    self.additional_flags = additional
    self.expected_quit_dir = ''

  def GetTmpDir(self):
    return "%s.%d" % (self.tmpdir, self.thread_id)

  
  def AfterRun(self, result):
    # delete the whole tmp dir
    try:
      rmtree(self.GetTmpDir())
    except:
      pass
    # make it again.
    try:
      mkdir(self.GetTmpDir())
    except:
      pass

    if self.expected_quit_dir :
      if not os.path.exists(os.path.join(self.file, self.expected_quit_dir)):
        print("expected_quit_dir:", self.expected_quit_dir)
        self.expected_quit_dir_fail = True
      else:
        try:
          rmtree(os.path.join(self.file, self.expected_quit_dir))
        except:
          pass

  def BeforeRun(self):
    # delete the whole tmp dir
    try:
      rmtree(self.GetTmpDir())
    except:
      pass
    # make it again.
    # intermittently fails on win32, so keep trying
    while not os.path.exists(self.GetTmpDir()):
      try:
        mkdir(self.GetTmpDir())
      except:
        pass

    manifest = json.loads(open(os.path.join(self.file, 'package.json')).read(), 'utf-8')
    if manifest.get('expect_dir_exists'):
      self.expected_quit_dir = manifest['expect_dir_exists']
      try:
        rmtree(os.path.join(self.file, self.expected_quit_dir))
      except:
        pass
    else:
      self.expected_quit_dir = ''
  
  def GetLabel(self):
    return "%s %s" % (self.mode, self.GetName())

  def GetName(self):
    return self.path[-1]

  def GetCommand(self):
    result = [self.config.context.GetVm(self.arch, self.mode, self.nwdir)]
    manifest = json.loads(open(os.path.join(self.file, 'package.json')).read(), 'utf-8')
    if manifest.get('expect_exit_code'):
      self.expected_exit_code = manifest['expect_exit_code']
    else:
      self.expected_exit_code = 0

    result += [self.file]

    return result

  def IsFailureOutput(self, output):
    return output.exit_code != self.expected_exit_code or hasattr(self, 'expected_quit_dir_fail')

  def GetSource(self):
    return open(self.file).read()

class SimpleTestConfiguration(test.TestConfiguration):

  def __init__(self, context, root, section, additional=[]):
    super(SimpleTestConfiguration, self).__init__(context, root)
    self.section = section
    self.additional_flags = additional

  def Ls(self, path):
    def SelectTest(name):
      return os.path.isdir(os.path.join(path, name))
    return [f[0:] for f in os.listdir(path) if SelectTest(f)]

  def ListTests(self, current_path, path, arch, mode, nwdir):
    all_tests = [current_path + [t] for t in self.Ls(join(self.root))]
    result = []
    for test in all_tests:
      if self.Contains(path, test):
        file_path = join(self.root, reduce(join, test[1:], ""))
        result.append(SimpleTestCase(test, file_path, arch, mode, nwdir, self.context,
                                     self, self.additional_flags))
    return result

  def GetBuildRequirements(self):
    return ['sample', 'sample=shell']

  def GetTestStatus(self, sections, defs):
    status_file = join(self.root, '%s.status' % (self.section))
    if exists(status_file):
      test.ReadConfigurationInto(status_file, sections, defs)

class ParallelTestConfiguration(SimpleTestConfiguration):
  def __init__(self, context, root, section, additional=[]):
    super(ParallelTestConfiguration, self).__init__(context, root, section,
                                                    additional)

  def ListTests(self, current_path, path, arch, mode, nwdir):
    result = super(ParallelTestConfiguration, self).ListTests(
         current_path, path, arch, mode, nwdir)
    for test in result:
      test.parallel = True
    return result

class AddonTestConfiguration(SimpleTestConfiguration):
  def __init__(self, context, root, section, additional=[]):
    super(AddonTestConfiguration, self).__init__(context, root, section)

  def Ls(self, path):
    def SelectTest(name):
      return name.endswith('.js')

    result = []
    for subpath in os.listdir(path):
      if os.path.isdir(join(path, subpath)):
        for f in os.listdir(join(path, subpath)):
          if SelectTest(f):
            result.append([subpath, f[:-3]])
    return result

  def ListTests(self, current_path, path, arch, mode, nwdir):
    all_tests = [current_path + t for t in self.Ls(join(self.root))]
    result = []
    for test in all_tests:
      if self.Contains(path, test):
        file_path = join(self.root, reduce(join, test[1:], "") + ".js")
        result.append(
            SimpleTestCase(test, file_path, arch, mode, nwdir, self.context, self))
    return result

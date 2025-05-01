import test
import os
from os.path import join, dirname, exists, basename, isdir
import re
import utils
from functools import reduce

class RemotingTestCase(test.TestCase):

  def __init__(self, path, file, arch, mode, nwdir, context, config, additional=[]):
    super(RemotingTestCase, self).__init__(context, path, arch, mode, nwdir)
    self.file = file
    self.config = config
    self.arch = arch
    self.mode = mode
    self.additional_flags = additional
    self.nwdir = nwdir

  def GetTmpDir(self):
    return "%s.%d" % (self.tmpdir, self.thread_id)

  def GetChromeDriver(self, arch, mode, nwdir):
    if utils.IsWindows():
        return os.path.abspath(join(nwdir, 'chromedriver.exe'))

    return os.path.abspath(join(nwdir, 'chromedriver'))
  
  def AfterRun(self, result):
      return

  def BeforeRun(self):
      return

  def GetLabel(self):
    return "%s %s" % (self.mode, self.GetName())

  def GetName(self):
    return self.path[-1]

  def GetEnv(self):
    libpath=os.getenv('PYTHONPATH')
    if libpath is None:
        libpath = join(self.file, '..', '..', '..', '..', '..', 'third_party/webdriver/pylib')
    return {'PYTHONPATH': libpath, 'CHROMEDRIVER': self.GetChromeDriver(self.arch, self.mode, self.nwdir)}

  def GetCommand(self):
    if utils.IsWindows():
        result = ['C:\\Python311\\python']
    else:
        result = ['python3']
    result += [self.file + '/test.py']

    return result

  def IsFailureOutput(self, output):
    return output.exit_code != 0

  def GetSource(self):
    return open(self.file).read()

class RemotingTestConfiguration(test.TestConfiguration):

  def __init__(self, context, root, section, additional=[]):
    super(RemotingTestConfiguration, self).__init__(context, root)
    self.section = section
    self.additional_flags = additional

  def Ls(self, path):
    def SelectTest(name):
      test = os.path.join(path, name)
      return (os.path.isdir(test) and
              not os.path.exists(os.path.join(test, 'skipped')) and
              os.path.exists(os.path.join(test, 'test.py'))
    return [f[0:] for f in os.listdir(path) if SelectTest(f)]

  def ListTests(self, current_path, path, arch, mode, nwdir):
    all_tests = [current_path + [t] for t in self.Ls(join(self.root))]
    result = []
    for test in all_tests:
      if self.Contains(path, test):
        file_path = join(self.root, reduce(join, test[1:], ""))
        result.append(RemotingTestCase(test, file_path, arch, mode, nwdir, self.context,
                                     self, self.additional_flags))
    return result

  def GetBuildRequirements(self):
    return ['sample', 'sample=shell']

  def GetTestStatus(self, sections, defs):
    status_file = join(self.root, '%s.status' % (self.section))
    if exists(status_file):
      test.ReadConfigurationInto(status_file, sections, defs)

def GetConfiguration(context, root):
  return RemotingTestConfiguration(context, root, 'sanity')

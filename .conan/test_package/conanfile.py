#!/usr/bin/env python
import os
from conans import ConanFile, CMake

class BackportConanTest(ConanFile):
  settings = "os", "compiler", "arch", "build_type"
  generators = "cmake"

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()

  def imports(self):
    pass

  def test(self):
    os.chdir("bin")
    if str(self.settings.os) in ["Windows", "Linux", "Macos"]:
        self.run("test-package")
    else:
      self.output.warn("Skipping unit test execution due to cross compiling for {}".format(self.settings.os))

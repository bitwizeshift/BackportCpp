#!/usr/bin/env python3

from conans import ConanFile, CMake

class BackportConan(ConanFile):

  # Package Info
  name = "Backport"
  version = "0.1.0"
  description = "Modern C++ backported to C++11"
  url = "https://github.com/bitwizeshift/bpstd"
  author = "Matthew Rodusek <matthew.rodusek@gmail.com>"
  license = "MIT"
  generators = "cmake"

  # Sources
  exports = ("LICENSE")
  exports_sources = ("cmake/*",
                     "include/*",
                     "test/*",
                     "CMakeLists.txt",
                     "LICENSE")

  # Settings
  options = {}
  default_options = {}
  build_requires = ("Catch2/2.7.1@catchorg/stable")

  def source(self):
    pass

  def build(self):
    pass

  def test(self):
    pass

  def package(self):
    cmake = CMake(self)
    cmake.definitions["BACKPORT_COMPILE_UNIT_TESTS"] = "ON"
    cmake.configure()

    # Compile and run the unit tests
    cmake.build()
    cmake.build(target="test")

    cmake.install()

    self.copy(pattern="LICENSE", dst="licenses")
    return

  def package_id(self):
    self.info.header_only()
    return
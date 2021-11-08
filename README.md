# BOINC Applications

## Status

| Build Status | Code Quality |
| --- | --- |
| [![Linux](https://github.com/BOINC/boinc-apps/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/linux.yml) | [![codecov](https://codecov.io/gh/BOINC/boinc-apps/branch/master/graph/badge.svg?token=RZ5J0e24Ye)](https://codecov.io/gh/BOINC/boinc-apps) |
| [![OSX](https://github.com/BOINC/boinc-apps/actions/workflows/osx.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/osx.yml) | [![Coverity Scan Build Status](https://scan.coverity.com/projects/24043/badge.svg)](https://scan.coverity.com/projects/boinc-boinc-apps) |
| [![Windows](https://github.com/BOINC/boinc-apps/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/windows.yml) | |

## Building

Currently supported platforms are:
- linux (x64)
- osx (x64)
- windows (x64 and x86)

To build any of the applications, you need to run the following command:
```
$ python3 build.py [PARAMS] APP
```
Where `APP` is the name of the application you want to build or `all` to build them all.

List of available applications:
- boinc-autodock-vina

List of available parameters:
- `-ts` - build the application for development (`default`) or for the ci (`ci`). Default is `default`.
- `-t` - triplet of the target platform. Available values: `x64-linux-static`, `x64-osx-static`, `x64-windows-static`, `x86-windows-static`. Default on linux is `x64-linux-static`, on windows is `x64-windows-static`. There is no default value for OSX.
- `-nb` - perform no build (only unit-tests will be executed). If no unit-tests were build, script will fail with an error.
- `-nt` - run no tests after build.
- `-occ` - path to the OpenCppCoverage tool. If not specified, script will run unit-tests with no coverage report. This is windows specific option nad will be ignored on any other OS.

### Examples:

Build all applications for development on windows x64:
```
$ python3 build.py -ts=default -t=x64-windows-static all
```
Build the `boinc-autodock-vina` application for ci on linux x64:
```
$ python3 build.py -ts=ci -t=x64-linux-static boinc-autodock-vina
```
Build the `boinc-autodock-vina` application for ci on OSX x64 and run no tests:
```
$ python3 build.py -ts=ci -t=x64-osx-static -nt boinc-autodock-vina
```

# BOINC Applications

## Status

| Build Status | Code Quality |
| --- | --- |
| [![Android](https://github.com/BOINC/boinc-apps/actions/workflows/android.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/android.yml) | [![codecov](https://codecov.io/gh/BOINC/boinc-apps/branch/master/graph/badge.svg?token=RZ5J0e24Ye)](https://codecov.io/gh/BOINC/boinc-apps) |
| [![Linux](https://github.com/BOINC/boinc-apps/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/linux.yml) | [![CodeQL](https://github.com/BOINC/boinc-apps/actions/workflows/codeql.yml/badge.svg)](https://github.com/BOINC/boinc-apps/actions/workflows/codeql.yml) |
| [![OSX](https://github.com/BOINC/boinc-apps/actions/workflows/osx.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/osx.yml) | [![Coverity Scan Build Status](https://scan.coverity.com/projects/24043/badge.svg)](https://scan.coverity.com/projects/boinc-boinc-apps) |
| [![Windows](https://github.com/BOINC/boinc-apps/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/BOINC/boinc-apps/actions/workflows/windows.yml) | |

## OS support

### Android

| Application | arm64-android-static | arm-neon-android-static | arm-android-static | x64-android-static | x86-android-static |
| --- | --- | --- | --- | --- | --- |
| boinc-autodock-vina | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
### Linux

| Application | arm64-linux-static | armneon-linux-static | arm-linux-static | x64-linux-static | x86-linux-static |
| --- | --- | --- | --- | --- | --- |
| boinc-autodock-vina | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |

### OSX

| Application | arm64-osx-static | x64-osx-static |
| --- | --- | --- |
| boinc-autodock-vina | :heavy_check_mark: | :heavy_check_mark: |

### Windows

| Application | x64-windows-static | x86-windows-static |
| --- | --- | --- |
| boinc-autodock-vina | :heavy_check_mark: | :heavy_check_mark: |

## Building

Currently supported platforms are:
- linux (arm64, arm+vpfv3, arm+neon, x64 and x86)
- osx (arm64 and x64)
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
- `-t` - triplet of the target platform. Available values are shown in the table above (OS Support). Default on linux is `x64-linux-static`, on osx is `x64-osx-static`, on windows is `x64-windows-static`.
- `-nb` - perform no build (only unit-tests will be executed). If no unit-tests were build, script will fail with an error.
- `-nt` - run no tests after build.
- `-qemu` - path to the QEMU tool. If not specified, script will run unit-tests with no QEMU emulation. This is linux and macos specific option and will be ignored on any other OS.
- `-cr` - include coverage report

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

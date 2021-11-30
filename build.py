# This file is part of BOINC.
# https://boinc.berkeley.edu
# Copyright (C) 2021 University of California
#
# BOINC is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# BOINC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

import os
import subprocess
import sys

apps = ['all', 'boinc-autodock-vina']

def help():
    print('Usage:')
    print('\tpython build.py [PARAMS] APP')
    print('PARAMS:')
    print('\t-ts=[default|ci]')
    print('\t-t=overlay_triplet')
    print('APP:')
    for a in apps:
        print('\t' + a)

def get_target_os_from_triplet(triplet):
    t = triplet.split('-')
    if (len(t) > 3 and t[1] == 'neon'):
        target_os = t[2]
    elif (len(t) > 2):
        target_os = t[1]
    else:
        target_os = 'linux'

    return target_os

def get_arch_from_triplet(triplet):
    t = triplet.split('-')
    if (len(t) > 1):
        arch = t[0]
    else:
        arch = 'x64'

    if (arch == 'arm' and len(t) > 2 and t[1] == 'neon'):
        arch = 'armneon'

    target_os = get_target_os_from_triplet(triplet)

    if (target_os == 'windows' and arch == 'x86'):
        arch = 'Win32'
    elif (target_os == 'osx' and arch == 'x64'):
        arch = 'x86_64'
    elif (target_os == 'linux'):
        if (arch == 'x64'):
            arch = '64'
        elif (arch == 'x86'):
            arch = '32'

    return arch

def install_android_ndk(build_path, app, triplet):
    ndk_version = 'r23b'
    ndk_path = os.path.join(build_path, app, triplet)

    if (not os.path.exists(ndk_path)):
        os.makedirs(ndk_path, exist_ok=True)
        ndk_url = 'https://dl.google.com/android/repository/android-ndk-' + ndk_version + '-linux.zip'
        ndk_zip = os.path.join(build_path, 'android-ndk.zip')
        print('Downloading Android NDK from ' + ndk_url)
        subprocess.run(['curl', '-L', '-o', ndk_zip, ndk_url])
        subprocess.run(['unzip', '-q', ndk_zip, '-d', ndk_path])
        os.remove(ndk_zip)

    return os.path.join(ndk_path, 'android-ndk-' + ndk_version)

def build_specific_init_params(CC, CXX, LD, CFLAGS, CXXFLAGS, LDFLAGS):
    specific_init_params = ''

    if (CC is not None):
        specific_init_params = ('{specific_init_params} CC="{CC}"').format(specific_init_params=specific_init_params, CC=CC)
    if (CXX is not None):
        specific_init_params = ('{specific_init_params} CXX="{CXX}"').format(specific_init_params=specific_init_params, CXX=CXX)
    if (LD is not None):
        specific_init_params = ('{specific_init_params} LD="{LD}"').format(specific_init_params=specific_init_params, LD=LD)
    if (CFLAGS is not None):
        specific_init_params = ('{specific_init_params} CFLAGS="{CFLAGS}"').format(specific_init_params=specific_init_params, CFLAGS=CFLAGS)
    if (CXXFLAGS is not None):
        specific_init_params = ('{specific_init_params} CXXFLAGS="{CXXFLAGS}"').format(specific_init_params=specific_init_params, CXXFLAGS=CXXFLAGS)
    if (LDFLAGS is not None):
        specific_init_params = ('{specific_init_params} LDFLAGS="{LDFLAGS}"').format(specific_init_params=specific_init_params, LDFLAGS=LDFLAGS)

    if (specific_init_params != ''):
        specific_init_params = ('cmake -E env {specific_init_params}').format(specific_init_params=specific_init_params)

    return specific_init_params

def build_linux_specific_init_params(arch):
    CC = None
    CXX = None
    LD = None
    CFLAGS = None
    CXXFLAGS = None
    LDFLAGS = None

    if (arch == '64' or arch == '32'):
        CC = ('gcc -m{arch}').format(arch=arch)
        CXX = ('g++ -m{arch}').format(arch=arch)
        CFLAGS = ('-m{arch}').format(arch=arch)
        CXXFLAGS = ('-m{arch}').format(arch=arch)
        LDFLAGS =('-m{arch} -static-libstdc++ -static').format(arch=arch)
    elif (arch == 'arm'):
        CC = 'arm-linux-gnueabihf-gcc'
        CXX = 'arm-linux-gnueabihf-g++'
        LD = 'arm-linux-gnueabihf-ld'
        CFLAGS = '-march=armv7-a+vfpv3-d16'
        CXXFLAGS = '-march=armv7-a+vfpv3-d16'
        LDFLAGS = '-march=armv7-a+vfpv3-d16 -static-libstdc++ -static'
    elif (arch == 'armneon'):
        CC = 'arm-linux-gnueabihf-gcc'
        CXX = 'arm-linux-gnueabihf-g++'
        LD = 'arm-linux-gnueabihf-ld'
        CFLAGS = '-march=armv7-a+neon-vfpv3'
        CXXFLAGS = '-march=armv7-a+neon-vfpv3'
        LDFLAGS = '-march=armv7-a+neon-vfpv3 -static-libstdc++ -static'
    elif (arch == 'arm64'):
        CC = 'aarch64-linux-gnu-gcc'
        CXX = 'aarch64-linux-gnu-g++'
        LD = 'aarch64-linux-gnu-ld'
        CFLAGS = '-march=armv8-a'
        CXXFLAGS = '-march=armv8-a'
        LDFLAGS = '-march=armv8-a -static-libstdc++ -static'

    return build_specific_init_params(CC, CXX, LD, CFLAGS, CXXFLAGS, LDFLAGS)

def build_android_specific_init_params(arch, tc_path):
    CC = None
    CXX = None
    LD = None
    CFLAGS = None
    CXXFLAGS = None
    LDFLAGS = None

    if (arch == 'arm' or arch == 'armneon'):
        toolchain_root = os.path.join(tc_path, 'toolchains', 'llvm', 'prebuilt', 'linux-x86_64')
        sysroot = os.path.join(toolchain_root, 'sysroot')
        includes = os.path.join(tc_path, 'arm-linux-androideabi')
        CC = 'armv7a-linux-androideabi21-clang'
        CXX = 'armv7a-linux-androideabi21-clang++'
        LD = 'arm-linux-androideabi-ld'
        if (arch == 'arm'):
            CFLAGS = (
                '--sysroot={sysroot} '
                '-I{includes}/include '
                '-O3 '
                '-fomit-frame-pointer '
                '-fPIE '
                '-march=armv7-a '
                '-mfloat-abi=softfp '
                '-mfpu=vfpv3-d16 '
                '-D__ANDROID_API__=21'
                ).format(
                    sysroot=sysroot,
                    includes=includes
                )
        elif (arch == 'armneon'):
            CFLAGS = (
                '--sysroot={sysroot} '
                '-I{includes}/include '
                '-O3 '
                '-fomit-frame-pointer '
                '-fPIE '
                '-march=armv7-a '
                '-mfloat-abi=softfp '
                '-mfpu=neon-vfpv3 '
                '-D__ANDROID_API__=21'
                ).format(
                    sysroot=sysroot,
                    includes=includes
                )
        CXXFLAGS = CFLAGS
        LDFLAGS = (
            '-L{includes}/usr/lib '
            '-L{includes}/lib '
            '-fPIE '
            '-pie '
            '-march=armv7-a '
            '-static-libstdc++ '
            '-llog '
            '-latomic '
            '-Wl,--fix-cortex-a8'
            ).format(
                includes=includes
            )
    elif (arch == 'arm64' or arch == 'x64' or arch == 'x86'):
        toolchain_path = os.path.join(tc_path, 'toolchains', 'llvm', 'prebuilt', 'linux-x86_64')
        sysroot = os.path.join(toolchain_path, 'sysroot')
        if (arch == 'arm64'):
            includes = os.path.join(tc_path, 'aarch64-linux-android')
            CC = 'aarch64-linux-android21-clang'
            CXX = 'aarch64-linux-android21-clang++'
            LD = 'aarch64-linux-android-ld'
        elif (arch == 'x64'):
            includes = os.path.join(tc_path, 'x86_64-linux-android')
            CC = 'x86_64-linux-android21-clang'
            CXX = 'x86_64-linux-android21-clang++'
            LD = 'x86_64-linux-android-ld'
        elif (arch == 'x86'):
            includes = os.path.join(tc_path, 'i686-linux-android')
            CC = 'i686-linux-android21-clang'
            CXX = 'i686-linux-android21-clang++'
            LD = 'i686-linux-android-ld'
        CFLAGS = (
            '--sysroot={sysroot} '
            '-I{includes}/include '
            '-O3 '
            '-fomit-frame-pointer '
            '-fPIE '
            '-D__ANDROID_API__=21'
            ).format(
                sysroot=sysroot,
                includes=includes
            )
        CXXFLAGS = CFLAGS
        LDFLAGS = (
            '-L{includes}/usr/lib '
            '-L{includes}/lib '
            '-fPIE '
            '-pie '
            '-static-libstdc++ '
            '-llog '
            '-latomic'
            ).format(
                includes=includes
            )

    return build_specific_init_params(CC, CXX, LD, CFLAGS, CXXFLAGS, LDFLAGS)

def fix_path_for_android(arch, android_tc_path):
    tc_binaries = os.path.join(android_tc_path, 'toolchains', 'llvm', 'prebuilt', 'linux-x86_64', 'bin')

    tc_includes = None
    if (arch == 'arm' or arch == 'armneon'):
        tc_includes = os.path.join(android_tc_path, 'arm-linux-androideabi', 'bin')
    elif (arch == 'arm64'):
        tc_includes = os.path.join(android_tc_path, 'aarch64-linux-android', 'bin')
    elif (arch == 'x64'):
        tc_includes = os.path.join(android_tc_path, 'x86_64-linux-android', 'bin')
    elif (arch == 'x86'):
        tc_includes = os.path.join(android_tc_path, 'i686-linux-android', 'bin')

    os.environ['PATH'] = tc_binaries + ':' + os.environ['PATH']
    if (tc_includes is not None):
        os.environ['PATH'] = tc_includes + ':' + os.environ['PATH']

if (len(sys.argv) < 2):
    help()
    sys.exit(1)

apps_to_build = []
vcpkg_overlay_triplets = None
vcpkg_overlay_triplet = None
opencppcoverage_path = None
qemu_command = None

run_build = True
run_tests = True

for a in sys.argv[1:]:
    if a in apps:
        apps_to_build.append(a)
    elif a.startswith('-'):
        if (a == '-nb'):
            run_build = False
        elif (a == '-nt'):
            run_tests = False
        else:
            p = a.split('=')
            if (len(p) < 2):
                print('Invalid option: ' + a)
                help()
                sys.exit(1)
            if (p[0] == '-ts' and vcpkg_overlay_triplets is None):
                vcpkg_overlay_triplets = p[1]
            elif (p[0] == '-t' and vcpkg_overlay_triplet is None):
                vcpkg_overlay_triplet = p[1]
            elif (p[0] == '-occ' and opencppcoverage_path is None):
                opencppcoverage_path = p[1]
            elif (p[0] == '-qemu' and qemu_command is None):
                qemu_command = p[1]
            else:
                print('Invalid option: ' + a)
                help()
                sys.exit(1)
    else:
        print('Invalid app: ' + a)
        help()
        sys.exit(1)

if apps_to_build == []:
    print('No app to build specified')
    help()
    sys.exit(1)

if 'all' in apps_to_build:
    apps_to_build = apps[1:]

if vcpkg_overlay_triplets is None:
    vcpkg_overlay_triplets = 'default'
if vcpkg_overlay_triplet is None:
    if (os.name == 'nt'):
        vcpkg_overlay_triplet = 'x64-windows-static'
    elif (os.name == 'posix' and sys.platform == 'darwin'):
        vcpkg_overlay_triplet = 'x64-osx-static'
    else:
        vcpkg_overlay_triplet = 'x64-linux-static'

vcpkg_bootstrap_file = 'bootstrap-vcpkg.bat' if os.name == 'nt' else 'bootstrap-vcpkg.sh'

if run_build:
    if not os.path.isfile('vcpkg/'+vcpkg_bootstrap_file):
        result = subprocess.call('git clone https://github.com/microsoft/vcpkg.git', shell=True)
        if result != 0:
            print('Failed to clone vcpkg')
            sys.exit(1)

    result = subprocess.call('git -C vcpkg pull', shell=True)
    if result != 0:
        print('Failed to pull vcpkg')
        sys.exit(1)

    if os.name == 'nt':
        result = subprocess.call('vcpkg\\'+vcpkg_bootstrap_file, shell=True)
    else:
        result = subprocess.call('./vcpkg/'+vcpkg_bootstrap_file, shell=True)
    if result != 0:
        print('Failed to bootstrap vcpkg')
        sys.exit(1)

arch = get_arch_from_triplet(vcpkg_overlay_triplet)
if (os.name == 'nt'):
    arch_param = ('-A {arch}').format(arch=arch)
elif (os.name == 'posix' and sys.platform == 'darwin'):
    arch_param = ('-DCMAKE_OSX_ARCHITECTURES={arch}').format(arch=arch)
else:
    arch_param = ''

vcpkg_cmake = os.path.join(os.getcwd(),'vcpkg', 'scripts', 'buildsystems', 'vcpkg.cmake')
vcpkg_overlay_triplets = os.path.join(os.getcwd(), 'vcpkg_triplets', vcpkg_overlay_triplets)
vcpkg_overlay_ports = os.path.join(os.getcwd(), 'vcpkg_custom_ports')
boinc_apps_git_revision = subprocess.check_output('git rev-parse --short HEAD', shell=True).decode('utf-8').strip()
path_fixed = False

if (get_target_os_from_triplet(vcpkg_overlay_triplet) == 'linux'):
    specific_init_params = build_linux_specific_init_params(arch)
else:
    specific_init_params = ''

for a in apps_to_build:
    if run_build:
        print('Building ' + a)
        if (get_target_os_from_triplet(vcpkg_overlay_triplet) == 'android'):
            android_tc_path = os.path.join(os.getcwd(), install_android_ndk(os.path.join(os.getcwd(), 'build'), a, vcpkg_overlay_triplet))
            if (not path_fixed):
                fix_path_for_android(get_arch_from_triplet(vcpkg_overlay_triplet), android_tc_path)
                path_fixed = True
            specific_init_params = build_android_specific_init_params(arch, android_tc_path)

        build_dir = os.path.join('build', a, vcpkg_overlay_triplet)

        result = subprocess.call((
            '{specific_init_params} '
            'cmake -B {build_dir} '
            '-S {a} '
            '{arch_param} '
            '-DCMAKE_TOOLCHAIN_FILE={vcpkg_cmake} '
            '-DVCPKG_OVERLAY_PORTS={vcpkg_overlay_ports} '
            '-DVCPKG_OVERLAY_TRIPLETS={vcpkg_overlay_triplets} '
            '-DVCPKG_TARGET_TRIPLET={vcpkg_overlay_triplet} '
            '-DVCPKG_INSTALL_OPTIONS=--clean-after-build '
            '-DBOINC_APPS_GIT_REVISION={boinc_apps_git_revision}'
            ).format(
                a=a,
                build_dir=build_dir,
                vcpkg_cmake=vcpkg_cmake,
                vcpkg_overlay_ports=vcpkg_overlay_ports,
                vcpkg_overlay_triplets=vcpkg_overlay_triplets,
                vcpkg_overlay_triplet=vcpkg_overlay_triplet,
                arch_param=arch_param,
                boinc_apps_git_revision=boinc_apps_git_revision,
                specific_init_params=specific_init_params
                ), shell=True)
        if result != 0:
            print('Failed to build ' + a)
            sys.exit(1)

        result = subprocess.call((
            'cmake --build {build_dir} '
            '--config Release'
            ).format(
                a=a,
                build_dir=build_dir
                ), shell=True)
        if result != 0:
            print('Failed to build ' + a)
            sys.exit(1)
    if run_tests:
        print('Testing ' + a)
        if os.name == 'nt':
            unittest_path = os.path.join(os.getcwd(), 'build', a, vcpkg_overlay_triplet, 'Release', 'unit-tests.exe')
        else:
            unittest_path = os.path.join(os.getcwd(), 'build', a, vcpkg_overlay_triplet, 'unit-tests')
        if os.path.isfile(unittest_path):
            if os.name == 'nt':
                if opencppcoverage_path is None:
                    result = subprocess.call(unittest_path, shell=True)
                else:
                    result = subprocess.call((
                        '{opencppcoverage_path} '
                        '--cover_children '
                        '--optimized_build '
                        '--sources {sources} '
                        '--export_type=cobertura:cobertura.xml '
                        '-- '
                        '{unittest_path} '
                        '--gtest_output=xml:gtest.xml'
                        ).format(
                            opencppcoverage_path=opencppcoverage_path,
                            sources=os.getcwd(),
                            unittest_path=unittest_path
                            ), shell=True)
            else:
                if qemu_command is None:
                    result = subprocess.call(unittest_path, shell=True)
                else:
                    result = subprocess.call((
                        '{qemu_command} '
                        '{unittest_path}'
                        ).format(
                            unittest_path=unittest_path,
                            qemu_command=qemu_command
                            ), shell=True)
        if result != 0:
            print('Failed to run unit tests for ' + a)
            sys.exit(1)

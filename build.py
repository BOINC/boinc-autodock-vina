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

def getArchFromTriplet(triplet):
    t = triplet.split('-')
    if (len(t) > 1):
        arch = t[0]
    else:
        arch = "x64"

    if (os.name == 'nt' and arch == "x86"):
        arch = "Win32"

    return arch

if (len(sys.argv) < 2):
    help()
    sys.exit(1)

apps_to_build = []
vcpkg_overlay_triplets = None
vcpkg_overlay_triplet = None
opencppcoverage_path = None

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
    if os.name == 'nt':
        vcpkg_overlay_triplet = 'x64-windows-static'
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

arch = getArchFromTriplet(vcpkg_overlay_triplet)
arch_param = ('-A {arch}').format(arch=arch) if os.name == 'nt' else ''

vcpkg_cmake = os.getcwd() + '/vcpkg/scripts/buildsystems/vcpkg.cmake'
vcpkg_overlay_triplets = os.getcwd() + '/vcpkg_triplets/' + vcpkg_overlay_triplets
vcpkg_overlay_ports = os.getcwd() + '/vcpkg_custom_ports/'

for a in apps_to_build:
    if run_build:
        print('Building ' + a)

        result = subprocess.call((
            'cmake -B build/{a}/{vcpkg_overlay_triplet} '
            '-S {a} '
            '{arch_param} '
            '-DCMAKE_TOOLCHAIN_FILE={vcpkg_cmake} '
            '-DVCPKG_OVERLAY_PORTS={vcpkg_overlay_ports} '
            '-DVCPKG_OVERLAY_TRIPLETS={vcpkg_overlay_triplets} '
            '-DVCPKG_TARGET_TRIPLET={vcpkg_overlay_triplet} '
            '-DVCPKG_INSTALL_OPTIONS=--clean-after-build'
            ).format(
                a=a,
                vcpkg_cmake=vcpkg_cmake,
                vcpkg_overlay_ports=vcpkg_overlay_ports,
                vcpkg_overlay_triplets=vcpkg_overlay_triplets,
                vcpkg_overlay_triplet=vcpkg_overlay_triplet,
                arch_param=arch_param
                ), shell=True)
        if result != 0:
            print('Failed to build ' + a)
            sys.exit(1)

        result = subprocess.call((
            'cmake --build build/{a}/{vcpkg_overlay_triplet} --config Release'
            ).format(
                a=a,
                vcpkg_overlay_triplet=vcpkg_overlay_triplet
                ), shell=True)
        if result != 0:
            print('Failed to build ' + a)
            sys.exit(1)
    if run_tests:
        print('Testing ' + a)
        if os.name == 'nt':
            unittest_path = (os.getcwd() +
                "/build/{a}/{vcpkg_overlay_triplet}/Release/unit-tests.exe"
                ).format(
                    a=a,
                    vcpkg_overlay_triplet=vcpkg_overlay_triplet
                    )
        else:
            unittest_path = (os.getcwd() +
                "/build/{a}/{vcpkg_overlay_triplet}/unit-tests"
                ).format(
                    a=a,
                    vcpkg_overlay_triplet=vcpkg_overlay_triplet
                    )
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
                result = subprocess.call(unittest_path, shell=True)
        if result != 0:
            print('Failed to run unit tests for ' + a)
            sys.exit(1)

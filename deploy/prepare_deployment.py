# This file is part of BOINC.
# http://boinc.berkeley.edu
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
# You should have received a of the GNU Lesser General Public License
# along with BOINC.  If not, see <http://www.gnu.org/licenses/>.
#

import os
import sys

linux_client_list = [
    './client/boinc',
    './client/boinccmd',
    './client/switcher'
]

linux_apps_list = [
    './samples/condor/boinc_gahp',
    './samples/example_app/uc2',
    './samples/example_app/ucn',
    './samples/example_app/uc2_graphics',
    './samples/example_app/slide_show',
    './samples/multi_thread/multi_thread',
    './samples/sleeper/sleeper',
    './samples/vboxmonitor/vboxmonitor',
    './samples/vboxwrapper/vboxwrapper',
    './samples/worker/worker',
    './samples/wrapper/wrapper',
    './samples/openclapp/openclapp',
    './samples/wrappture/wrappture_example',
    './samples/wrappture/fermi'
]

linux_manager_list = [
    './clientgui/boincmgr'
]

mingw_apps_list = [
    './lib/wrapper.exe'
]

mingw_apps_vcpkg_list = [
    './samples/condor/boinc_gahp.exe',
    './samples/example_app/uc2.exe',
    './samples/example_app/ucn.exe',
    './samples/example_app/uc2_graphics.exe',
    './samples/example_app/slide_show.exe',
    './samples/multi_thread/multi_thread.exe',
    './samples/sleeper/sleeper.exe',
    './samples/worker/worker.exe',
    './samples/wrapper/wrapper.exe',
    './samples/wrappture/wrappture_example.exe',
    './samples/wrappture/fermi.exe'
]

android_manager_generic_list = [
    './android/BOINC/app/build/outputs/apk/debug/app-debug.apk',
    './android/BOINC/app/build/outputs/apk/release/app-release-unsigned.apk'
]

android_manager_xiaomi_list = [
    './android/BOINC/app/build/outputs/apk/xiaomi_debug/app-xiaomi_debug.apk',
    './android/BOINC/app/build/outputs/apk/xiaomi_release/app-xiaomi_release-unsigned.apk'
]

android_manager_armv6_list = [
    './android/BOINC/app/build/outputs/apk/armv6_debug/app-armv6_debug.apk',
    './android/BOINC/app/build/outputs/apk/armv6_release/app-armv6_release-unsigned.apk'
]

android_apps_list = [
    # boinc_gahp
    './samples/condor/android_armv6_boinc_gahp',
    './samples/condor/android_arm_boinc_gahp',
    './samples/condor/android_arm64_boinc_gahp',
    './samples/condor/android_x86_boinc_gahp',
    './samples/condor/android_x86_64_boinc_gahp',
    # uc2
    './samples/example_app/android_armv6_uc2',
    './samples/example_app/android_arm_uc2',
    './samples/example_app/android_arm64_uc2',
    './samples/example_app/android_x86_uc2',
    './samples/example_app/android_x86_64_uc2',
    # ucn
    './samples/example_app/android_armv6_ucn',
    './samples/example_app/android_arm_ucn',
    './samples/example_app/android_arm64_ucn',
    './samples/example_app/android_x86_ucn',
    './samples/example_app/android_x86_64_ucn',
    # multi_thread
    './samples/multi_thread/android_armv6_multi_thread',
    './samples/multi_thread/android_arm_multi_thread',
    './samples/multi_thread/android_arm64_multi_thread',
    './samples/multi_thread/android_x86_multi_thread',
    './samples/multi_thread/android_x86_64_multi_thread',
    # sleeper
    './samples/sleeper/android_armv6_sleeper',
    './samples/sleeper/android_arm_sleeper',
    './samples/sleeper/android_arm64_sleeper',
    './samples/sleeper/android_x86_sleeper',
    './samples/sleeper/android_x86_64_sleeper',
    # worker
    './samples/worker/android_armv6_worker',
    './samples/worker/android_arm_worker',
    './samples/worker/android_arm64_worker',
    './samples/worker/android_x86_worker',
    './samples/worker/android_x86_64_worker',
    # wrapper
    './samples/wrapper/android_armv6_wrapper',
    './samples/wrapper/android_arm_wrapper',
    './samples/wrapper/android_arm64_wrapper',
    './samples/wrapper/android_x86_wrapper',
    './samples/wrapper/android_x86_64_wrapper',
    # wrappture_example
    './samples/wrappture/android_armv6_wrappture_example',
    './samples/wrappture/android_arm_wrappture_example',
    './samples/wrappture/android_arm64_wrappture_example',
    './samples/wrappture/android_x86_wrappture_example',
    './samples/wrappture/android_x86_64_wrappture_example',
    # fermi
    './samples/wrappture/android_armv6_fermi',
    './samples/wrappture/android_arm_fermi',
    './samples/wrappture/android_arm64_fermi',
    './samples/wrappture/android_x86_fermi',
    './samples/wrappture/android_x86_64_fermi'
]

windows_apps_list = [
    './win_build/Build/x64/Release/htmlgfx*.exe',
    './win_build/Build/x64/Release/wrapper*.exe',
    './win_build/Build/x64/Release/vboxwrapper*.exe',
    './win_build/Build/x64/Release/boincsim.exe',
    './win_build/Build/x64/Release/slide_show.exe',
    './win_build/Build/x64/Release/example*.exe',
    './win_build/Build/x64/Release/worker*.exe',
    './win_build/Build/x64/Release/sleeper*.exe',
    './win_build/Build/x64/Release/boinclog.exe',
    './win_build/Build/x64/Release/boincsim.exe',
    './win_build/Build/x64/Release/multi_thread*.exe',
    './win_build/Build/x64/Release/slide_show.exe',
    './win_build/Build/x64/Release/test*.exe',
    './win_build/Build/x64/Release/wrappture*.exe',
    './win_build/Build/ARM64/Release/htmlgfx*.exe',
    './win_build/Build/ARM64/Release/wrapper*.exe',
    './win_build/Build/ARM64/Release/vboxwrapper*.exe',
    './win_build/Build/ARM64/Release/boincsim.exe',
    './win_build/Build/ARM64/Release/slide_show.exe',
    './win_build/Build/ARM64/Release/example*.exe',
    './win_build/Build/ARM64/Release/worker*.exe',
    './win_build/Build/ARM64/Release/sleeper*.exe',
    './win_build/Build/ARM64/Release/boinclog.exe',
    './win_build/Build/ARM64/Release/boincsim.exe',
    './win_build/Build/ARM64/Release/multi_thread*.exe',
    './win_build/Build/ARM64/Release/slide_show.exe',
    './win_build/Build/ARM64/Release/test*.exe',
    './win_build/Build/ARM64/Release/wrappture*.exe'
]

windows_client_list = [
    './win_build/Build/x64/Release/boinc.exe',
    './win_build/Build/x64/Release/boincsvcctrl.exe',
    './win_build/Build/x64/Release/boinccmd.exe',
    './win_build/Build/x64/Release/boincscr.exe',
    './win_build/Build/x64/Release/boinc.scr',
    './win_build/Build/ARM64/Release/boinc.exe',
    './win_build/Build/ARM64/Release/boincsvcctrl.exe',
    './win_build/Build/ARM64/Release/boinccmd.exe',
    './win_build/Build/ARM64/Release/boincscr.exe',
    './win_build/Build/ARM64/Release/boinc.scr',
    './curl/ca-bundle.crt'
]

windows_manager_list = [
    './win_build/Build/x64/Release/boinctray.exe',
    './win_build/Build/x64/Release/boincmgr.exe',
    './win_build/Build/ARM64/Release/boinctray.exe',
    './win_build/Build/ARM64/Release/boincmgr.exe'
]

def prepare_7z_archive(archive_name, target_directory, files_list):
    os.makedirs(target_directory, exist_ok=True)
    archive_path = os.path.join(target_directory, archive_name + '.7z')
    command = '7z a -t7z -mx=9 ' + archive_path + ' ' + ' '.join(files_list)
    os.system(command)

def help():
    print('Usage: python preprare_deployment.py BOINC_TYPE')
    print('BOINC_TYPE : [linux_client | linux_client-vcpkg | linux_apps | linux_apps-vcpkg | linux_manager-with-webview | linux_manager-without-webview | win_apps-mingw | win_apps-mingw-vcpkg | android_manager | android_manager-vcpkg | android_apps | android_apps-vcpkg | win_apps | win_client | win_manager]')

def prepare_linux_client(target_directory):
    prepare_7z_archive('linux_client', target_directory, linux_client_list)

def prepare_linux_client_vcpkg(target_directory):
    prepare_7z_archive('linux_client-vcpkg', target_directory, linux_client_list)

def prepare_linux_apps(target_directory):
    prepare_7z_archive('linux_apps', target_directory, linux_apps_list)

def prepare_linux_apps_vcpkg(target_directory):
    prepare_7z_archive('linux_apps-vcpkg', target_directory, linux_apps_list)

def prepare_linux_manager_with_webview(target_directory):
    prepare_7z_archive('linux_manager-with-webview', target_directory, linux_manager_list)

def prepare_linux_manager_without_webview(target_directory):
    prepare_7z_archive('linux_manager-without-webview', target_directory, linux_manager_list)

def prepare_win_apps_mingw(target_directory):
    prepare_7z_archive('win_apps-mingw', target_directory, mingw_apps_list)

def prepare_win_apps_mingw_vcpkg(target_directory):
    prepare_7z_archive('win_apps-mingw-vcpkg', target_directory, mingw_apps_vcpkg_list)

def prepare_android_manager(target_directory):
    prepare_7z_archive('android_manager', target_directory, android_manager_generic_list)
    prepare_7z_archive('android_manager_xiaomi', target_directory, android_manager_xiaomi_list)
    prepare_7z_archive('android_manager_armv6', target_directory, android_manager_armv6_list)

def prepare_android_manager_vcpkg(target_directory):
    prepare_7z_archive('android_manager-vcpkg', target_directory, android_manager_generic_list)
    prepare_7z_archive('android_manager-vcpkg_xiaomi', target_directory, android_manager_xiaomi_list)
    prepare_7z_archive('android_manager-vcpkg_armv6', target_directory, android_manager_armv6_list)

def prepare_android_apps(target_directory):
    prepare_7z_archive('android_apps', target_directory, android_apps_list)

def prepare_android_apps_vcpkg(target_directory):
    prepare_7z_archive('android_apps-vcpkg', target_directory, android_apps_list)

def prepare_win_apps(target_directory):
    prepare_7z_archive('win_apps', target_directory, windows_apps_list)

def prepare_win_client(target_directory):
    prepare_7z_archive('win_client', target_directory, windows_client_list)

def prepare_win_manager(target_directory):
    prepare_7z_archive('win_manager', target_directory, windows_manager_list)

boinc_types = {
    'linux_client': prepare_linux_client,
    'linux_client-vcpkg': prepare_linux_client_vcpkg,
    'linux_apps': prepare_linux_apps,
    'linux_apps-vcpkg': prepare_linux_apps_vcpkg,
    'linux_manager-with-webview': prepare_linux_manager_with_webview,
    'linux_manager-without-webview': prepare_linux_manager_without_webview,
    'win_apps-mingw': prepare_win_apps_mingw,
    'win_apps-mingw-vcpkg': prepare_win_apps_mingw_vcpkg,
    'android_manager': prepare_android_manager,
    'android_manager-vcpkg': prepare_android_manager_vcpkg,
    'android_apps': prepare_android_apps,
    'android_apps-vcpkg': prepare_android_apps_vcpkg,
    'win_apps': prepare_win_apps,
    'win_client': prepare_win_client,
    'win_manager': prepare_win_manager
}

if (len(sys.argv) != 2):
    help()
    sys.exit(1)


boinc_type = sys.argv[1]
target_dir = 'deploy'

if (boinc_type not in boinc_types):
    print(f'Unknown BOINC_TYPE: {boinc_type}')
    help()
    sys.exit(1)

boinc_types[boinc_type](target_dir)

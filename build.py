import os
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

for a in sys.argv[1:]:
    if a in apps:
        apps_to_build.append(a)
    elif a.startswith('-'):
        p = a.split('=')
        if (len(p) < 2):
            print('Invalid option: ' + a)
            help()
            sys.exit(1)
        if (p[0] == '-ts' and vcpkg_overlay_triplets is None):
            vcpkg_overlay_triplets = p[1]
        elif (p[0] == '-t' and vcpkg_overlay_triplet is None):
            vcpkg_overlay_triplet = p[1]
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

if not os.path.isfile('vcpkg/'+vcpkg_bootstrap_file):
    os.system('git clone https://github.com/microsoft/vcpkg.git')

os.system('git -C vcpkg pull')

if os.name == 'nt':
    os.system('vcpkg\\'+vcpkg_bootstrap_file)
else:
    os.system('.vcpkg/'+vcpkg_bootstrap_file)

arch = getArchFromTriplet(vcpkg_overlay_triplet)

vcpkg_cmake = os.getcwd() + '/vcpkg/scripts/buildsystems/vcpkg.cmake'
vcpkg_overlay_triplets = os.getcwd() + '/vcpkg_triplets/' + vcpkg_overlay_triplets
vcpkg_overlay_ports = os.getcwd() + '/vcpkg_custom_ports/'

for a in apps_to_build:
    print('Building ' + a)
    os.system((
        'cmake -B build/{a}/{vcpkg_overlay_triplet} '
        '-S {a} '
        '-A {arch} '
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
            arch=arch
            ))
    os.system((
        'cmake --build build/{a}/{vcpkg_overlay_triplet} --config Release'
        ).format(
            a=a,
            vcpkg_overlay_triplet=vcpkg_overlay_triplet
            ))
    unittest_path = (os.getcwd() +
        "/build/{a}/{vcpkg_overlay_triplet}/Release/unit-tests.exe"
         ).format(
            a=a,
            vcpkg_overlay_triplet=vcpkg_overlay_triplet
            )
    if os.path.isfile(unittest_path):
        os.system(unittest_path)

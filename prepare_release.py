import os
import shutil
import sys
import zipfile

def get_platform(filename, app):
    platform = filename[len(app)+1:-len('-static-.zip')]
    if (platform == 'armneon-linux'):
        platform = 'arm-neon-linux'
    return platform

def get_server_release_platform(platform):
    platforms = {
        'arm64-android': 'aarch64-android-linux-gnu',
        'arm64-linux' : 'aarch64-unknown-linux-gnu',
        'arm-android': 'arm-android-linux-gnu',
        'arm-linux': 'arm-unknown-linux-gnueabihf',
        'arm64-osx': 'arm64-apple-darwin',
        'x86-linux': 'i686-pc-linux-gnu',
        'x86-windows': 'windows_intelx86',
        'x64-windows': 'windows_x86_64',
        'x86-android': 'x86-android-linux-gnu',
        'x64-android': 'x86_64-android-linux-gnu',
        'x64-osx': 'x86_64-apple-darwin',
        'x64-linux': 'x86_64-pc-linux-gnu'
    }
    return platforms[platform] if platform in platforms else None

def is_windows(platform):
    return 'windows' in platform


def unzip(archive, destination, filename):
    with zipfile.ZipFile(archive, 'r') as zip_ref:
        zip_ref.extract(filename, destination)

def zip(source, destination):
    with zipfile.ZipFile(destination, 'w', compression=zipfile.ZIP_DEFLATED) as zip_ref:
        for root, dirs, files in os.walk(source):
            for file in files:
                zip_ref.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), os.path.join(source, '..')))

if len(sys.argv) != 5:
    print('Usage: python prepare_release.py <current_dir> <app_name> <release_version> <server_app_name>')
    sys.exit(1)

current_dir = sys.argv[1]
app = sys.argv[2]
release = sys.argv[3]
server_app_name = sys.argv[4]

release_dir = os.path.join(current_dir, app + '-release-'+ release)
server_release_dir_without_version = os.path.join(current_dir, server_app_name)
server_release_dir = os.path.join(server_release_dir_without_version, release)
wu_generator_release_dir = os.path.join(current_dir, app + '-wu-generator-release-'+ release)

for root, dirs, files in os.walk(current_dir):
    for file in files:
        full_path = os.path.join(root, file)
        if file.startswith(app) and os.path.isfile(full_path):
            current_platform = get_platform(file, app)
            current_platform_dir = os.path.join(release_dir, current_platform)
            os.makedirs(current_platform_dir, exist_ok=True)
            unzip(full_path, current_platform_dir, app + '.exe' if is_windows(current_platform) else app)
            server_release_platform = get_server_release_platform(current_platform)
            if server_release_platform is not None:
                server_release_platform_dir = os.path.join(server_release_dir, server_release_platform)
                os.makedirs(server_release_platform_dir, exist_ok=True)
                unzip(full_path, server_release_platform_dir, app + '.exe' if is_windows(current_platform) else app)
                extracted_file = os.path.join(server_release_platform_dir, app + '.exe' if is_windows(current_platform) else app)
                new_file = os.path.join(server_release_platform_dir, app + '_' + server_release_platform + '_v' + release)
                if is_windows(current_platform):
                    new_file += '.exe'
                os.rename(extracted_file, new_file)
            if current_platform == 'x64-linux':
                os.makedirs(wu_generator_release_dir, exist_ok=True)
                unzip(full_path, wu_generator_release_dir, 'work-generator')
                unzip(full_path, wu_generator_release_dir, 'config-validator')
            os.remove(full_path)
    break

zip(release_dir, os.path.join(current_dir, app + '-release-'+ release + '.zip'))
zip(server_release_dir_without_version, os.path.join(current_dir, app + '-v'+ release + '.zip'))
zip(wu_generator_release_dir, os.path.join(current_dir, app + '-wu-generator-release-'+ release + '.zip'))

shutil.rmtree(release_dir)
shutil.rmtree(server_release_dir_without_version)
shutil.rmtree(wu_generator_release_dir)

#!/usr/bin/env python3
import os
import shutil
import stat
import subprocess
import sys

executable = ""
executable_dir = ""
copied_dependencies = []


def is_homebrew_library(library):
    homebrew_locations = ["/usr/local/opt", "/opt/homebrew"]
    for homebrew_location in homebrew_locations:
        if homebrew_location in library:
            return True
    return False


def rewrite_identity(object):
    shutil.chown(object, os.getuid())
    st = os.stat(object)
    os.chmod(object, st.st_mode | stat.S_IWUSR)
    id = f"@executable_path/{os.path.basename(object)}"
    ret = subprocess.run(["install_name_tool", "-id", id, object])
    if ret.returncode != 0:
        print("Error:", ret.stderr.decode('utf-8'))
    os.chmod(object, (st.st_mode | stat.S_IWUSR) ^ stat.S_IWUSR)
    print(f"Rewritten identity of {object}")


def rewrite_dependency(object, dependency):
    shutil.chown(object, os.getuid())
    st = os.stat(object)
    os.chmod(object, st.st_mode | stat.S_IWUSR)
    dest = f"@executable_path/{os.path.basename(dependency)}"
    ret = subprocess.run(["install_name_tool", "-change", dependency,
                          dest, object])
    if ret.returncode != 0:
        print("Error:", ret.stderr.decode('utf-8'))
    os.chmod(object, (st.st_mode | stat.S_IWUSR) ^ stat.S_IWUSR)
    print(f"Rewritten reference from {dependency} to {dest}")


def copy_and_rewrite(file):
    global executable
    global executable_dir
    if not os.path.isfile(file):
        # raise Exception("{} is not a file.".format(executable))
        return []
    if os.path.exists(executable_dir + "/" + os.path.basename(file)):
        return []
    otool_ret = subprocess.run(["otool", "-L", file], capture_output=True)
    if otool_ret.returncode != 0:
        raise Exception("An error occured in calling otool -L:\n {}"
                        .format(otool_ret.stderr.decode('utf-8')))
    dependencies = []
    for line in otool_ret.stdout.decode('utf-8').split('\n')[1:]:
        line = line.strip()
        if len(line) == 0 or line.find(" (compatibility version") == -1:
            continue
        library = os.path.abspath(line[0:line.find(" (compatibility version")])
        if is_homebrew_library(library):
            dependencies.append(library)
    copied_file = file
    if file != executable:
        copied_file = shutil.copy2(file, executable_dir)
        print(f"Copied {file} to {copied_file}")
    for dependency in dependencies:
        if dependency == file:
            rewrite_identity(copied_file)
        else:
            rewrite_dependency(copied_file, dependency)
            if dependency not in copied_dependencies:
                copied_dependencies.append(dependency)
                copy_and_rewrite(dependency)


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 ./tools/copy_mac_libs.py <path to executable>")
        return 1
    if not os.path.isfile(sys.argv[1]):
        print(sys.argv[1], "is not a file.")
        return 1
    global executable
    global executable_dir
    executable = os.path.abspath(sys.argv[1])
    executable_dir = os.path.dirname(executable)
    print("Executable Dir =", executable_dir)
    copy_and_rewrite(executable)
    return 0


exit(main())

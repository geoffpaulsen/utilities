#!/usr/bin/python
import os
import stat
import glob
import shutil
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# Install all scripts to /usr/local/bin/ or provide path as command line arg.
def install():
    install_dir = '/usr/local/bin/'
    if len(sys.argv) > 1:
        install_dir = sys.argv[1]

    if not os.path.isdir(install_dir):
        print(bcolors.FAIL + "Error:" + bcolors.ENDC + " Path '" + install_dir +
                "' does not exist. Nothing installed.")
        return

    scripts = glob.iglob('*.py')
    gen = (x for x in scripts if x != 'install.py')
    for fn in gen:
        new_path = install_dir + fn.replace('.py', '')
        shutil.copy2(fn, new_path)
        st = os.stat(new_path)
        os.chmod(new_path, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | 
                stat.S_IXOTH)
        print(bcolors.OKGREEN + "Installed" + bcolors.ENDC + " '" + fn + "' to "
                + new_path)

if __name__ == '__main__':
    install()

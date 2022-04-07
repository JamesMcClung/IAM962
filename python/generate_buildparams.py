import sys
import os

########################################################################
# Ensure correct number of args

if len(sys.argv) < 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/buildparams.h NX=128 DT= ...")

########################################################################
# Parse path to file and defines to define

path, *defines = sys.argv[1:]

# alphabetize so permuting defines doesn't trigger a rebuild
defines.sort()

########################################################################
# Make any missing directories in path

dir = os.path.dirname(path)
if not os.path.exists(dir):
    os.mkdir(dir)

########################################################################
# Replace old file contents with new file contents

# Generate new build params from args

def generate_c_line(define: str):
    name, val = define.split('=')
    name = name.upper()
    if val:
        return f"#define {name} {val}"

new_buildparams = "#pragma once\n" + "\n".join(filter(None, map(generate_c_line, defines)))

# Read old build params

if os.path.isfile(path):
    with open(path, 'r') as file:
        old_buildparams = file.read()
else:
    old_buildparams = ""

# Only update file if doing so would change the contents

if old_buildparams != new_buildparams:
    print(f"Updating: {path}")
    with open(path, 'w') as file:
        file.write(new_buildparams)
else:
    print(f"Already up-to-date: {path}")
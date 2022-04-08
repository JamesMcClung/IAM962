import sys
import os

########################################################################
# Constants

AMEND_FLAG = "--amend"

AMEND_MODE = "amend"
REPLACE_MODE = "replace"

########################################################################
# Ensure correct number of args

if len(sys.argv) < 2:
    sys.exit(f"Usage: {sys.argv[0]} path/to/buildparams.h NX=128 DT= ...")

########################################################################
# Parse path to file, mode, and defines of form "name=val"

path, *rest = sys.argv[1:]

mode = REPLACE_MODE

if AMEND_FLAG in rest:
    mode = AMEND_MODE

# remove flags
defines = [d for d in rest if not d.startswith("-")]

########################################################################
# Make any missing directories in path

dir = os.path.dirname(path)
if not os.path.exists(dir):
    os.mkdir(dir)

########################################################################
# Read old build params

if os.path.isfile(path):
    with open(path, 'r') as file:
        old_buildparams = file.read()
else:
    old_buildparams = ""
    
########################################################################
# Utility

def parse_defines(defines: str) -> dict:
    # make args case-insensitive with call to upper()
    return {name.upper(): val or None for name, val in [define.split('=') for define in defines]}

def construct_c_line(name, val) -> str:
    if val:
        return f"#define {name} {val}"
    else:
        return f"// use default {name}"

def deconstruct_c_line(line: str) -> tuple:
    words = line.split(" ")
    if words[0] == "#define":
        # account for possible spaces in val, e.g. `#define foo "hello world"`
        return words[1], ' '.join(words[2:])
    elif words[0].startswith("//"):
        # line such as `//#define NX` or `// use default NX`
        return words[-1], None

########################################################################
# Generate new build params

new_dict = parse_defines(defines)

if mode == AMEND_MODE:
    _, *old_c_lines = old_buildparams.split('\n')

    old_dict = dict(map(deconstruct_c_line, old_c_lines))
    new_dict = old_dict | new_dict

# sort so that permuting defines in command line doesn't force rebuild
new_buildparams = "#pragma once\n" + "\n".join(sorted(construct_c_line(name, val) for name, val in new_dict.items()))

########################################################################
# Update file if its contents would change

if old_buildparams != new_buildparams:
    print(f"Updating: {path}")
    with open(path, 'w') as file:
        file.write(new_buildparams)
else:
    print(f"Already up-to-date: {path}")
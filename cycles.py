# Code to detect cycles in dependency graph.
import os, re

def get_lines(root, file):
    lines = []
    with open(os.path.join(root, file)) as f:
        for line in f.readlines():
            if "#include" in line:
                lines.append(line)
    return lines

def get_includes(file, file_lines):
    file_lines.map()

for root, dir, files in os.walk("./src"):
    for file in files:
        if file.endswith(('.h', '.cpp', '.lex', '.ypp')):
            # gets all the files that includes other files.
            file_lines = get_lines(root, file)

# The idea is to build a dependency graph and then 
# use union find to find cycles (i.e. cyclical dependencies)
            

    
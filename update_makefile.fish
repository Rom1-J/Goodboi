#!/usr/bin/env fish

set -l obj_files (find src -type f -name "*.c" | sed 's/\.c$/.o/')

sed "s|{{OBJS_FILES}}|$obj_files|g" Makefile.template > Makefile

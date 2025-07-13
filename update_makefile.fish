#!/usr/bin/env fish

set -l obj_files (find src -type f -name "*.c" | sed 's/\.c$/.o/')
set -l rk_version (echo (date +%s):(git rev-parse --short HEAD 2>/dev/null || echo "N/A"))

sed "s|{{OBJS_FILES}}|$obj_files|g" Makefile.template \
    | sed "s|{{RK_VERSION}}|$rk_version|g" > Makefile

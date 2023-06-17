#! /bin/sh

set -e 

[ -f limine.h ] || curl -Lo limine.h https://github.com/limine-bootloader/limine/raw/trunk/limine.h
[ -d flanterm ] || git clone https://github.com/mintsuki/flanterm.git --depth=1

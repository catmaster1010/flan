#! /bin/sh

set -e 

[ -f limine.h ] || curl -Lo limine.h https://github.com/limine-bootloader/limine/raw/trunk/limine.h
[ -d flanterm ] || git clone https://github.com/mintsuki/flanterm.git --depth=1

MLIBC_VERSION=4.0.0-rc1

if ! [ -d mlibc-headers ]; then
    git clone https://github.com/managarm/mlibc.git
    cd mlibc
    git checkout ${MLIBC_VERSION}
    for f in ../../patches/mlibc/*; do patch -p1 < $f; done
    mkdir ../mlibc-build
    cd ../mlibc-build
    meson setup --cross-file ../../build-support/cross_file.txt --prefix=/ -Dheaders_only=true  ../mlibc
    ninja
    DESTDIR=$(realpath ../mlibc-headers-out) ninja install
    cd ..
    mv mlibc-headers-out/include mlibc-headers
    rm -rf mlibc mlibc-build mlibc-headers-out
fi

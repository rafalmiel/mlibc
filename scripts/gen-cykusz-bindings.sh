#!/bin/bash

SPATH=$(dirname $(readlink -f "$0"))
ROOTPATH=$SPATH/../../..

cd $ROOTPATH

./dockit.sh ./build.sh sysroot_rust_bindings
./dockit.sh cp $SPATH/../subprojects/freestnd-c-hdrs/x86_64/include/{stddef,stdarg}.h $ROOTPATH/cykusz_mlibc/usr/include/

cd $SPATH

python $SPATH/rust-libc.py  $ROOTPATH/cykusz_mlibc/usr/include  | rustfmt > rust-libc-cykusz.rs

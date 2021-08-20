#!/bin/sh -x

dd if=/dev/zero of=sdcard.img iflag=fullblock bs=1M count=16    # create a 16 MB disk
parted sdcard.img --script -- mklabel msdos                     # create label
parted sdcard.img --script -- mkpart primary fat16 1MiB 100%    # create partition
mkfs.vfat -F16 sdcard.img                                       # format disk
# TODO - check boot size
dd bs=1 seek=62 if=boot.txt of=sdcard.img conv=notrunc          # copy boot file to boot area (byte 62 onwards)
mcopy -i sdcard.img hello.txt ::                                # copy file to image

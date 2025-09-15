#!/bin/bash
rm -rf newopenssh
rm -rf _newinstall
# 1. zlib
cd zlib
make clean
CC=arm-linux-gnueabihf-gcc CFLAGS="-march=armv7-a -mfpu=vfpv3" ./configure --prefix=$PWD/_install
make -j 12
make -j 12 install
cd ..

# 2. OpenSSL
cd openssl
make clean
./Configure linux-armv4 --cross-compile-prefix=arm-linux-gnueabihf- --prefix=$PWD/_install shared CFLAGS="-march=armv7-a -mfpu=vfpv3"
make -j 12
make -j 12 install
cd ..

# 3. OpenSSH
cd openssh-portable
make clean
sudo CC=arm-linux-gnueabihf-gcc ./configure \
    --prefix=$PWD/_install \
    --host=arm-linux-gnueabihf \
    --with-zlib=$PWD/../zlib/_install \
    --with-ssl-dir=$PWD/../openssl/_install \
    CFLAGS="-march=armv7-a -mfpu=vfpv3" \
    LDFLAGS="-L$PWD/../openssl/_install/lib -L$PWD/../zlib/_install/lib -Wl,-m,armelf_linux_eabi -Wl,--fix-cortex-a8"
make -j 12
make -j 12 install
mv _install newopenssh
mv newopenssh ..
cd ..

mkdir _newinstall
cp -r busybox/* _newinstall
cp -r newopenssh/bin/* _newinstall/bin
cp -r newopenssh/etc _newinstall
cp -r newopenssh/libexec _newinstall
cp -r newopenssh/sbin _newinstall
cp -r newopenssh/share _newinstall

cp /usr/arm-linux-gnueabihf/lib/ld-linux-armhf.so.3 _newinstall/lib/
cp /usr/arm-linux-gnueabihf/lib/libc.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libatomic.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libm.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libdl.so* _newinstall/lib/
cp /usr/arm-linux-gnueabihf/lib/libpthread.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libresolv.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libnss_dns.so* _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libnss_files.so _newinstall/lib
cp /usr/arm-linux-gnueabihf/lib/libnss_files.so* _newinstall/lib
cp zlib/_install/lib/libz.so* _newinstall/lib
cp openssl/_install/lib/libcrypto.so* _newinstall/lib
cp openssl/_install/lib/libssl.so* _newinstall/lib

cd _newinstall
rm ../fs/initramfs.cpio.gz
find . | cpio -H newc -o | gzip > ../fs/initramfs.cpio.gz
cd ..

QEMU_AUDIO_DRV=none qemu-system-arm -M vexpress-a9 -kernel zImage -dtb vexpress-v2p-ca9.dtb -initrd fs/initramfs.cpio.gz -append "console=ttyAMA0 rdinit=/bin/ash" -nographic


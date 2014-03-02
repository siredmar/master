make -j4
make modules -j4

scp arch/arm/boot/zImage root@gnublin:/boot

rm ../mod/*
make modules_install INSTALL_MOD_PATH=../mod


cd ../mod
tar cvfz mod.tar.gz .
scp mod.tar.gz root@gnublin:/

ssh root@gnublin "cd /; tar xvfz mod.tar.gz; echo done"

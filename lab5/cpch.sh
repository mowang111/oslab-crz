
sudo ./mount-hdc
cp linux-0.11/include/semaphore.h hdc/usr/include/
cp linux-0.11/include/unistd.h hdc/usr/include/
cp lab5/pc.c hdc/usr/root/
sudo umount hdc

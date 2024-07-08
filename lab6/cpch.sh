
sudo ./mount-hdc
cp lab6/test.c hdc/usr/root/
cp lab6/producer.c hdc/usr/root/
cp lab6/consumer.c hdc/usr/root/
cp lab6/testlab6.sh hdc/usr/root/
cp linux-0.11/include/unistd.h hdc/usr/include/
cp linux-0.11/include/sys/shm.h hdc/usr/include/sys/

sudo umount hdc

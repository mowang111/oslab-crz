gcc -o producer producer.c -Wall
gcc -o consumer consumer.c -Wall
./producer &
./consumer > consumer.log
#gcc main.c -o pan-hash -lcrypto -L/opt/ssl/lib -Wall


pan-hash: main.o
	cc -o pan-hash main.o -lcrypto -L/opt/ssl/lib -Wall

main.o: main.c
	cc -c main.c

clean:
	rm *.o pan-hash

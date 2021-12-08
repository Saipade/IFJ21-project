CC = gcc
CFLAGS = -std=c99

all:
	$(CC) $(CFLAGS) -o main *.c 

pack:
	tar -cvzf xfesiu00.tgz *.h *.c Makefile dokumentace.pdf rozdeleni

clean_pack:
	rm -f xfesiu00.tgz

clean: clean_pack
	rm -rf *.o *.out
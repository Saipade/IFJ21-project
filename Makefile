CC = gcc

all:
	$(CC) -o proi *.c 

pack:
	tar -cvzf xfesiu00.tgz *.h *.c Makefile

clean_pack:
	rm -f xfesiu00.tgz

clean: clean_pack
	rm -rf *.o *.out
CC=gcc
CFLAGS=-O2
LIBS=-lwolfssl

abc: pwCracker.o pwCracker2.o pwSetter.o
	$(CC) -o pwCracker pwCracker.o $(LIBS)
	$(CC) -o pwCracker2 pwCracker2.o $(LIBS)
	$(CC) -o pwSetter pwSetter.o $(LIBS)

clean:
	rm pwCracker pwCracker2 pwCracker.o pwCracker2.o pwSetter.o

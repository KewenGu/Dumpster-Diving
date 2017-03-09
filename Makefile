all: rm dv dump

rm: rm.o utility.o
	gcc -g rm.o utility.o -o rm

rm.o: rm.c utility.h
	gcc -g -c rm.c

dv: dv.o utility.o
	gcc -g dv.o utility.o -o dv

dv.o: dv.c utility.h
	gcc -g -c dv.c

dump: dump.o utility.o
	gcc -g dump.o utility.o -o dump

dump.o: dump.c utility.h
	gcc -g -c dump.c

utility.o: utility.c utility.h
	gcc -g -c utility.c

clean:
	rm -f *.o rm dv dump

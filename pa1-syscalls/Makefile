mycat: main.o
	gcc -o mycat main.o

main.o: main.c
	gcc -c main.c

.PHONY: clean
clean:
	rm -rf mycat *.o
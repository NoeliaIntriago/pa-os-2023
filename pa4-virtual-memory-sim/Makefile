GCC = gcc
CFLAGS = -Wall -g
OBJS = main.o

pagingdemand: $(OBJS)
	$(GCC) $(CFLAGS) -o main $(OBJS)

main.o: memory_management.c

clean:
	rm -f *.o pagingdemand
GCC = gcc
CFLAGS = -Wall -g
OBJS = memory_management.o

memory_management: $(OBJS)
	$(GCC) $(CFLAGS) -o memory_management $(OBJS)

memory_management.o: memory_management.c

test: memory_management
	./memory_management data/addresses.txt correct_out.txt
	diff correct_out.txt test/correct.txt

clean:
	rm -f *.o memory_management
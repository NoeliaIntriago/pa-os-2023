GCC = gcc
CFLAGS = -Wall -g
OBJS = memory_management.o

memory_management: $(OBJS)
	$(GCC) $(CFLAGS) -o memory_management $(OBJS)

memory_management.o: memory_management.c

test: memory_management
	./memory_management data/addresses.txt output.txt
	@diff_lines=$$(diff output.txt test/correct.txt | grep -c '^[<>]') ; \
	if [ $$diff_lines -eq 0 ]; then \
		echo "EXITO -> ARCHIVOS IDÉNTICOS"; \
		exit 0; \
	else \
		echo "ERROR -> ARCHIVOS DISTINTOS: $$diff_lines líneas diferentes"; \
		exit 1; \
	fi

clean:
	rm -f *.o memory_management
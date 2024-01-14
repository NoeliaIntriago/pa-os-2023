GCC = gcc
CFLAGS = -Wall -g
OBJS = memory_management.o

memory_management: $(OBJS)
	$(GCC) $(CFLAGS) -o memory_management $(OBJS)

memory_management.o: memory_management.c

test: memory_management
	./memory_management data/addresses.txt correct_out.txt
	@if diff correct_out.txt test/correct.txt > /dev/null; then \
		echo "EXITO -> ARCHIVOS IDÉNTICOS"; \
	else \
		echo "ERROR -> ARCHIVOS DISTINTOS"; \
	fi

clean:
	rm -f *.o memory_management
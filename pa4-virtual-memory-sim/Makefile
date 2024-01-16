GCC = gcc
CFLAGS = -Wall -g
OBJS = pagingdemand.o

pagingdemand: $(OBJS)
	$(GCC) $(CFLAGS) -o pagingdemand $(OBJS)

pagingdemand.o: pagingdemand.c

test: pagingdemand
	./pagingdemand addresses.txt data.txt
	@diff_lines=$$(diff data.txt correct.txt | grep -c '^[<>]') ; \
	if [ $$diff_lines -eq 0 ]; then \
		echo "EXITO -> ARCHIVOS IDÉNTICOS"; \
		exit 0; \
	else \
		echo "ERROR -> ARCHIVOS DISTINTOS: $$diff_lines líneas diferentes"; \
		exit 1; \
	fi

clean:
	rm -f *.o pagingdemand data.txt
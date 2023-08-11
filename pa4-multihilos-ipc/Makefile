GCC = gcc
CFLAGS = -Wall -Wshadow
OBJS = ex5.o bmp.o filter.o

ex5: $(OBJS)
	$(GCC) $(CFLAGS) $(OBJS) -o $@

.c.o: 
	$(GCC) $(CFLAGS) -c $*.c 

clean:
	rm *.o ex5

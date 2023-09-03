GCC=gcc
CFLAGS=-g -Wall -Wshadow

all: ex5_pipeline blur_filter edge_detection_filter

ex5_pipeline: ./publicador.c ./bmp.c ./bmp.h
	$(GCC) $(CFLAGS) ./publicador.c ./bmp.c -I./ -o $@

blur_filter: ./blur.c ./bmp.c ./bmp.h
	$(GCC) $(CFLAGS) ./blur.c ./bmp.c -I./ -o $@ -lpthread

edge_detection_filter: ./edge.c ./bmp.c ./bmp.h
	$(GCC) $(CFLAGS) ./edge.c ./bmp.c -o $@ -lpthread

clean:
	rm ex5_pipeline blur_filter edge_detection_filter

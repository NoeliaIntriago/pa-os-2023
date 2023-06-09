all: init timeshmm timepipe

init: 
	mkdir -p build

# build/time: 
# 	build/timeshmm & build/timepipe

timeshmm: 
	gcc -g -Wall -o build/timeshmm timeshmm.c -lrt

timepipe:
	gcc -g -Wall -o build/timepipe timepipe.c -lrt
     
clean:
	rm build/*
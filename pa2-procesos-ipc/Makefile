all: init timeshmm timepipe

init: 
	mkdir -p build

timeshmm: 
	gcc -g -Wall -o build/timeshmm timeshmm.c -lrt

timepipe:
	gcc -g -Wall -o build/timepipe timepipe.c -lrt
     
clean:
	 rm build/*
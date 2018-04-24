DFLAGS = -g -Wall -o

mycp.o: mycp.c
	gcc $(DFLAGS) mycp.o mycp.c
mycp: mycp.c
	gcc $(DFLAGS) mycp mycp.c
clean:
	rm -rf ./-c mycp *.o
    

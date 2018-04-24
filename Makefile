DFLAGS = -g -Wall -o

mycp: mycp.c
	gcc $(DFLAGS) mycp mycp.c
mycp.o: mycp.c
	gcc $(DFLAGS) mycp.o mycp.c
clean:
	rm -rf ./-c mycp *.o
    

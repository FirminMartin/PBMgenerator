all: pbmgen

pbmgen: main.o method.o pbm.o 
	gcc $^ -o $@

clean:
	rm -f *.o pbmgen


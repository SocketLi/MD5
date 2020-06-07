objects=main.o md5.o
MD5:$(objects)
	g++ -o MD5 $(objects) -g -std=c++11
	rm -f $(objects)
%.o: %.cc
	g++ -c -g -std=c++11 $<
clean:
	rm -f MD5 $(objects)
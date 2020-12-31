all:
	g++ jr.h jr.cpp -o jr
	g++ Writer.h Parser.h main.cpp -o parser
clean:
	rm -rf *.o
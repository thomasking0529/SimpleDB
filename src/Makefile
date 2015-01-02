default: all
	
all:
	g++ -c *.cpp -g -std=c++11
	g++ *.o -o SimpleDB
	
test: all
	./SimpleDB ../sample_test.txt
	
clean:
	rm -rf *.o SimpleDB
	
tar:
	rm -rf proj
	mkdir proj
	cp *.cpp *.hpp proj
	cp ../doc/design.pdf proj
	cp ../doc/test.pdf proj
	tar -cjvf proj.tar.bz2 proj

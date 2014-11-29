default: all
	
all:
	g++ -c *.cpp -g -std=c++11
	g++ *.o -o SimpleDB
	
test:
	echo not implemented yet.
	
clean:
	rm -rf *.o SimpleDB
	
tar:
	rm -rf proj
	mkdir proj
	cp *.cpp *.hpp proj
	cp ../doc/report.pdf proj
	tar -cjvf proj.tar.bz2 proj

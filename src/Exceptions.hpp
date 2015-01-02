#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>
#include <string>
#include <iostream>

//common exception for SimpleDB
class SDBException: std::exception {
private:
	std::string e;

public:
	//init with an error string
	SDBException(const std::string& es) {
		e = es;
	}

	void Print() {
		std::cout << e << std::endl;
	}

};

#endif

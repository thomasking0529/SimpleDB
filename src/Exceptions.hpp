#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>
#include <string>
#include <iostream>

class SDBException: std::exception {
private:
	std::string e;

public:
	SDBException(const std::string& es) {
		e = es;
	}

	void Print() {
		std::cout << e << std::endl;
	}

};

#endif

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>
#include <string>
#include <iostream>

//common exception for SimpleDB
class SDBLexerException: std::exception {
private:
	std::string e;

public:
	//init with an error string
	SDBLexerException(const std::string& es) {
		e = es;
	}

	void Print() {
		std::cout << e << std::endl;
	}

};

class SDBParserException: std::exception {
private:
	std::string e;

public:
	//init with an error string
	SDBParserException(const std::string& es) {
		e = es;
	}

	void Print() {
		std::cout << e << std::endl;
	}

};

#endif

/*
 * Lexer.hpp
 *
 *  Created on: Nov 29, 2014
 *      Author: Sun Jiacheng
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Exceptions.hpp"

/*
 * common functions
 */

bool isNum(const std::string& s);
bool isId(const std::string& s);

//Token types
enum TokenType {
	KEYWORD, ID, NUM, OP
};

struct Token {
	//type of token
	TokenType type;
	//value of token, raw string
	std::string value;
	Token(TokenType t, const std::string& v) {
		type = t;
		value = v;
	}
};

class Lexer {
private:
	//the column number
	int count;
	std::set<char> symbols;
	std::set<std::string> ops;
	//split input string into a list
	std::list<std::string> split(const std::string& s);
public:
	Lexer();
	/*
	 * use split to get raw input
	 * check illegal tokens and cerr message
	 */
	std::list<Token> GetTokens(const std::string& a);
};

#endif /* LEXER_HPP_ */

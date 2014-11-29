/*
 * Lexer.hpp
 *
 *  Created on: Nov 29, 2014
 *      Author: thomas
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

enum TokenType {
	KEYWORD, ID, NUM, OP
};

struct Token {
	TokenType type;
	std::string value;
	Token(TokenType t, const std::string& v) {
		type = t;
		value = v;
	}
};

class Lexer {
private:
	std::set<char> symbols;
	std::set<std::string> ops;
public:
	Lexer();
	/*
	 * check illegal tokens and cerr message
	 */
	std::list<std::string> split(const std::string& s);
	std::list<Token> GetTokens(const std::string& a);
};

#endif /* LEXER_HPP_ */

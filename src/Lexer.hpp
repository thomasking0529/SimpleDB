/*
 * Lexer.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <string>
#include <list>

enum TokenType {
	KEYWORD, ID, NUM, OP
};

struct Token {
	TokenType type;
	std::string value;
};

class Lexer {
public:
	Lexer();
	/*
	 * check illegal tokens and cerr message
	 */
	std::list<Token> GetTokens(const std::string& a);
};

#endif /* LEXER_HPP_ */

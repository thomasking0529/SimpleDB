/*
 * Parser.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include "Parser.hpp"

Parser::Parser() {
	lexer = new Lexer();
}

Statement Parser::Parse(const std::string& s) {
	std::list<Token> ts = lexer->GetTokens(s);
	Statement st;
	return st;
}

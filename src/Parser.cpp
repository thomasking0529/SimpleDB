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
	//use parseWhere to parse where statement
	Statement st;
	return st;
}

Condition* Parser::parseWhere(std::list<Token> ts) {
	//example:
	Condition* c = new Condition(NE);
	return c;
}

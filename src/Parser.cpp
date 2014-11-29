/*
 * Parser.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include "Parser.hpp"

Parser::Parser() {

}

Statement Parser::parseCreate(const std::list<Token>& token_list) {
	//TODO

	Statement ret;
	return ret;
}

Statement Parser::parseInsert(const std::list<Token>& token_list) {
	//TODO

	Statement ret;
	return ret;
}

Statement Parser::parseDelete(const std::list<Token>& token_list) {
	//TODO
	//use parseWhere to generate a list of conditions

	Statement ret;
	return ret;
}

Statement Parser::parseSelect(const std::list<Token>& token_list) {
	//TODO
	//use parseWhere to generate a list of conditions

	Statement ret;
	return ret;
}

std::list<Condition> Parser::parseWhere(const std::list<Token> & token_list) {
	//TODO

	std::list<Condition> ret;

	return ret;

}

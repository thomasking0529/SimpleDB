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

}

Statement Parser::parseInsert(const std::list<Token>& token_list) {
	//TODO

}

Statement Parser::parseDelete(const std::list<Token>& token_list) {
	//TODO
	//use parseWhere to generate a list of conditions

}

Statement Parser::parseSelect(const std::list<Token>& token_list) {
	//TODO
	//use parseWhere to generate a list of conditions

}

std::list<Condition> Parser::parseWhere(const std::list<Token> & token_list) {
	//TODO

}

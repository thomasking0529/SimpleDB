/*
 * Main.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include <iostream>
#include <fstream>
#include "Core.hpp"

int main(int argc, char *argv[]) {
	Lexer lex;
	std::list<std::string> a = lex.split(
			"select * from a where(id == a || id == 0)");
	std::list<std::string>::iterator i;
	for (i = a.begin(); i != a.end(); i++) {
		std::cout << *i << std::endl;
	}
	std::list<Token> s = lex.GetTokens(
			"select * from a where(id == a || id == 0 && id == s)");
	std::list<Token>::iterator ite;
	for (ite = s.begin(); ite != s.end(); ite++) {
		std::cout << ite->type << " " << ite->value << std::endl;
	}
//	SimpleDB* db = new SimpleDB();
	if (argc == 2) {
		//open file
		std::ifstream fin(argv[1]);
		//execute tests
//		db->Execute("select * from a");
		std::cout << "Line :" << std::endl;
	} else {
		//open terminal
	}
	return 0;
}

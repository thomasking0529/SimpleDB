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
	for (auto& i : a) {
		std::cout << i << std::endl;
	}
	std::list<Token> s = lex.GetTokens(
			"select * from a where(id == a || id == 0 && id == s)");
	for (auto& i : s) {
		std::cout << i.type << " " << i.value << std::endl;
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

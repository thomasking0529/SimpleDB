/*
 * Main.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include <iostream>
#include <fstream>
#include "Core.hpp"
#include "Parser.hpp"

bool isEmpty(const std::string& s) {
	for(auto& i : s) {
		if(i != ' ' && i != '\n' && i != '\t') {
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	SimpleDB* db = new SimpleDB();
	if (argc == 2) {
		//open file
		std::string es;
		std::ifstream fin(argv[1]);

		int line_count = 1;

		while (std::getline(fin, es, ';') && !isEmpty(es)) {
			std::cout << "SDB SQL " << line_count << ": " << es << std::endl;
			line_count++;
			db->Execute(es + ";");
		}
	} else {
		Polish p;
		p.Insert("32");
		p.Insert("*");
		p.Insert("3");
		p.Insert("-");
		p.Insert("6");
		p.Insert("/");
		p.Insert("2");
		std::cout << p.Calculate() << std::endl;
	}
	return 0;
}

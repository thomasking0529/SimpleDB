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
	std::string es;
	std::ifstream fin;
	if (argc == 2) {
		fin.open(argv[1]);
		int line_count = 1;

		while (std::getline(fin, es, ';') && !isEmpty(es)) {
			for(int i = 0; i < es.size(); i++) {
				if(es[i] == '\n') {
					es.erase(i, 1);
				}
			}
			std::cout << "SDB SQL " << line_count << ": " << es << std::endl;
			line_count++;
			db->Execute(es + ";");
		}
	} else {
		std::cerr << "Not input file specified.\n";
	}
	std::cout << "Bye!\n";
	return 0;
}

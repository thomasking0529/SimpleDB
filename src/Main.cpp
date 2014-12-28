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

int main(int argc, char *argv[]) {
	SimpleDB* db = new SimpleDB();
	if (argc == 2) {
		//open file
		std::string es;
		std::ifstream fin(argv[1]);

		int line_count = 1;

		while (std::getline(fin, es)) {
			while (es[es.size() - 1] == ' ' || es[es.size() - 1] == '\t')
				es.erase(es.end() - 1);
			while (es[es.size() - 1] != ';') {
				std::string tmp;
				std::getline(fin, tmp);
				es += tmp;
				while (es[es.size() - 1] == ' ' || es[es.size() - 1] == '\t')
					es.erase(es.end() - 1);
			}
			std::cout << "Line " << line_count << ": " << es << std::endl;
			line_count++;
			db->Execute(es);
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

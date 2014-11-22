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
	SimpleDB* db = new SimpleDB();
	if (argc == 2) {
		//open file
		std::ifstream fin(argv[1]);
		//execute tests
		db->Execute("select * from a");
		std::cout << "Line :" << std::endl;
	} else {
		//open terminal
	}
	return 0;
}

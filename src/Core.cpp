/*
 * Core.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include "Core.hpp"

void Table::insert(const std::list<std::string>& a) {

}

void SimpleDB::Execute(const Statement& stmt) {
	switch (stmt.act) {
	case CREATE: {
		Table t(stmt.table, stmt.key, stmt.prop_list);
		tables.push_back(t);
	}
		break;
	case DELETE:
		//TODO
		break;
	case INSERT:
		//TODO
		break;
	case QUERY:
		//TODO
		break;
	default:
		std::cerr << "Unkown Action\n";
	}
}

/*
 * Core.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include "Core.hpp"

void Table::Insert(const std::vector<int>& a) {
	Row tmp = Row(a, key_idx);
	if (rows.find(tmp) != rows.end()) {
		std::cerr << "Key value conflicts!\n";
	} else {
		rows.insert(tmp);
	}
}
void Table::Delete(const Condition* cond) {
	/*
	 * TODO:
	 * check condition
	 * delete rows;
	 */
}

void SimpleDB::Execute(const Statement& stmt) {
	switch (stmt.act) {
	case CREATE: {
		Table t(stmt.table, stmt.key_idx, stmt.prop_list);
		//check if exists
		tables.insert(t);
	}
		break;
	case DELETE:
		//TODO
		break;
	case INSERT:
		//TODO
		//check key conflicts using set<>keys
		break;
	case QUERY:
		//TODO
		break;
	default:
		std::cerr << "Unknown Action\n";
	}
}

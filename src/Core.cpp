/*
 * Core.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#include "Core.hpp"
#include <sstream>
#include <stdlib.h>

void Table::Insert(const std::vector<int>& a) {
	Row tmp = Row(a, key_idx, count);
	if (rows.find(tmp) != rows.end()) {
		std::cerr << "Key value conflicts!\n";
	} else {
		rows.insert(tmp);
		count++;
	}
}

int checkCond(const std::vector<Property> &props, const Row &r,
		const Condition* cond) {
	if (cond == NULL) {
		return true;
	}

	if (cond->opd != "no num") {
		if (isNum(cond->opd)) {
			// get num from string
			return atoi(cond->opd.c_str());
		} else {
			// get value of that column
			int index;
			for (unsigned int i = 0; i < props.size(); i++) {
				if (props[i].id == cond->opd) {
					index = i;
					break;
				}
			}
			return r.cols[index];
		}
	}

	int rightOp;
	rightOp = checkCond(props, r, cond->rc);

	// test	
	// std::cerr << "rightOp: " << rightOp << std::endl;
	// test

	// when op is NOT
	if (cond->op == NOT) {
		return (!rightOp);
	}

	// when op is AND/OR/LT/GT/NE/EQ/GTE/LTE, just fetch two operands
	int leftOp;
	leftOp = checkCond(props, r, cond->lc) ;

	// test
	// std::cerr << "leftOp: " << leftOp << std::endl;
	// test

	switch (cond->op) {
	case AND:
		return leftOp && rightOp;
	case OR:
		return leftOp || rightOp;
	case LT:
		return (leftOp < rightOp);
	case GT:
		return (leftOp > rightOp);
	case NE:
		return (leftOp != rightOp);
	case EQ:
		return (leftOp == rightOp);
	case GTE:
		return (leftOp >= rightOp);
	case LTE:
		return (leftOp <= rightOp);
	case PLUS:
		return (leftOp+rightOp);
	case MINUS:
		return (leftOp-rightOp);
	case MULTIPLY:
		return (leftOp*rightOp);
	case DIVIDE:
		return (leftOp/rightOp);
	}
	return false;
}

std::vector<int> Table::Query(const Condition* cond) {
	std::vector<int> keyOfRows;
	std::set<Row>::iterator it = rows.begin();
	for (; it != rows.end(); it++) {

		// test
		// std::cerr <<  "checkCond: " << checkCond(props, *it, cond) << std::endl;
		// test

		if (checkCond(props, *it, cond)) {
			keyOfRows.push_back(it->key);
		}
	}
	return keyOfRows;
}

void Table::Delete(const Condition* cond) {
	/*
	 * TODO:
	 * check condition
	 * delete rows;
	 * don't decrease count
	 */
	// find rows that match the condition
	std::vector<int> keyOfRows = Query(cond);

	// delete rows
	std::vector<int>::iterator it2 = keyOfRows.begin();
	for (; it2 != keyOfRows.end(); it2++) {
		rows.erase(Row(*it2));
	}
}

void SimpleDB::Execute(const Statement& stmt) {
	// check if table exist
	Table t(stmt.table, stmt.key_idx, stmt.prop_list);
	std::set<Table>::iterator it = tables.find(t);

	if (stmt.act == CREATE) {
		if (it == tables.end()) {
			// test if primary keys matches prop_list
			for (unsigned int i = 0; i < stmt.key_idx.size(); i++) {
				bool findKey = false;
				for (auto& p : stmt.prop_list) {
					if (p.id == stmt.key_idx[i]) {
						findKey = true;
						break;
					}
				}
				if (!findKey) {
					std::cerr << "primary key not in property list"
							<< std::endl;
					return;
				}
			}
			tables.insert(t);
		} else {
			std::cerr << "Table name conflicts!\n";
		}
	} else {
		if (it == tables.end()) {
			std::cerr << "Table " << stmt.table << " does not exist!\n";
			return;
		}
		switch (stmt.act) {
		case DELETE:
			const_cast<Table&>(*it).Delete(stmt.cond);
			break;
		case INSERT:
			// check primary key constraint violation(It is done in Table::Insert() )
			const_cast<Table&>(*it).Insert(stmt.value_list);
			break;
		case QUERY: {
			// find keys of all rows that match the conditions
			std::vector<int> keyOfRows = const_cast<Table&>(*it).Query(
					stmt.cond);

			// print the result
			std::vector<std::string> ids;
			std::vector<int> indexes;
			if (stmt.prop_list.back().id == "*") {
				for (unsigned int i = 0; i < it->props.size(); i++) {
					ids.push_back(it->props[i].id);
					indexes.push_back(i);
				}
			} else {
				for (auto& i : stmt.prop_list) {
					ids.push_back(i.id);
					for (unsigned int j = 0; j < it->props.size(); j++) {
						if (it->props[j].id == i.id) {
							indexes.push_back(j);
						}
					}
				}
			}

			std::string colWidth(20, '-');
			std::string lineSep("|");
			std::string titleLine("|");
			for (unsigned int i = 0; i < ids.size(); i++) {
				lineSep += colWidth + "|";
				// adjust leftSpace and rightSpace to align titles in the center of each blank
				std::string leftSpace((colWidth.size() - ids[i].size()) / 2,
						' ');
				std::string rightSpace(
						(colWidth.size() - ids[i].size() - leftSpace.size()),
						' ');
				titleLine += leftSpace + ids[i] + rightSpace + "|";
			}

			std::cout << lineSep << std::endl << titleLine << std::endl
					<< lineSep << std::endl;

			std::string leftSpace(8, ' ');
			std::vector<int> dummyVec;
			for (unsigned int i = 0; i < keyOfRows.size(); i++) {
				std::string dataLine = "|";
				for (unsigned int j = 0; j < ids.size(); j++) {
					int data =
							it->rows.find(Row(keyOfRows[i]))->cols[indexes[j]];
					std::stringstream ss;
					ss << data;
					std::string dataStr = ss.str();
					std::string rightSpace(
							(colWidth.size() - dataStr.size() - leftSpace.size()),
							' ');
					dataLine += leftSpace + dataStr + rightSpace + "|";
				}
				std::cout << dataLine << std::endl << lineSep << std::endl;
			}
			std::cout << keyOfRows.size() << " rows found" << std::endl;
		}
			break;
		default:
			std::cerr << "Unknown Action\n";
		}
	}
}

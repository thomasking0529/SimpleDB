/*
 * Core.cpp
 *
 *  Created on: Dec 29, 2014
 *      Author: Sun Dongliang
 */

#include "Core.hpp"
#include <sstream>
#include <stdlib.h>

void Table::Insert(const std::vector<int>& a, const std::list<Property>& prop_list) {
	// construct a row of values
	std::vector<int> record;
	std::vector<int>::const_iterator it_value;
	std::list<Property>::const_iterator it_prop;
	for (int i = 0; i < this->props.size(); i++) {
		bool find = false;
		for (it_value = a.begin(), it_prop = prop_list.begin(); it_prop != prop_list.end(); it_value++, it_prop++) {
			if (this->props[i] == *it_prop) {
				record.push_back(*it_value);
				find = true;
				break;
			}
		}
		if (!find) {
			record.push_back(this->props[i].default_value);
		}
	}

	// insert that row
	Row tmp = Row(record, key_idx, count);
	if (rows.find(tmp) != rows.end()) {
		std::cerr << "Primary key constraint violation\n";
	} else {
		rows.insert(tmp);
		count++;
	}
}

bool isCondValid(const std::vector<Property> &props, const Condition* cond) {
	if (cond == NULL) {
		return true;
	}
	if (cond->opd != "no num") {
		if (isNum(cond->opd)) {
			return true;
		} else {
			// get value of that column
			int index = -1;
			for (unsigned int i = 0; i < props.size(); i++) {
				if (props[i].id == cond->opd) {
					index = i;
					break;
				}
			}
			if (index == -1) {
				std::cerr << " columns occurring in the where clause (if any) should be in the schema of the table" << std::endl;
				return false;
			} else {
				return true;
			}
		}
	}

	// when op is NOT
	if (cond->op == NOT) {
		return isCondValid(props, cond->lc);
	} else {
		return isCondValid(props, cond->rc) && (props, cond->lc);
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
			int index = -1;
			for (unsigned int i = 0; i < props.size(); i++) {
				if (props[i].id == cond->opd) {
					index = i;
					break;
				}
			}
			return r.cols[index];
		}
	}

	int leftOp;
	leftOp = checkCond(props, r, cond->lc) ;

	// test	
	// std::cerr << "leftOp: " << leftOp << std::endl;
	// test

	// when op is NOT
	if (cond->op == NOT) {
		return (!leftOp);
	}

	// when op is AND/OR/LT/GT/NE/EQ/GTE/LTE, just fetch two operands
	int rightOp;
	rightOp = checkCond(props, r, cond->rc);

	// test
	// std::cerr << "rightOp: " << rightOp << std::endl;
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
		case DIVIDE: {
			if (rightOp == 0) {
				std::cerr << "divide by zero error, set result as 1" << std::endl;
				return 1;
			} else {
				return (leftOp/rightOp);
			}
		}
	}
	return false;
}

std::vector<int> Table::Query(const Condition* cond) {
	std::vector<int> keyOfRows;
	std::set<Row>::iterator it = rows.begin();

	// check if the condition is valid
	if (!isCondValid(props, cond)) {
		return keyOfRows;
	}

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
			// test if num of columns in a table is not greater than 100
			if (stmt.prop_list.size() > 100) {
				std::cerr << "Number of columns in a table should be no greater than 100!" << std::endl;
				return;
			}

			// test if num of columns in primary key declaration is no greater than 100
			if (stmt.key_idx.size() > 100) {
				std::cerr << "Number of columns in primary key declaration should be no greater than 100!" << std::endl;
				return;	
			}


			// test if there are duplicate column names
			std::list<Property>::const_iterator it1, it2;
			for (it1 = stmt.prop_list.begin(); it1 != stmt.prop_list.end(); it1++) {
				it2 = it1;
				for (it2++; it2 != stmt.prop_list.end(); it2++) {
					if (it1->id == it2->id) {
						std::cerr << "Duplicate column names found!"<< std::endl;
						return;
					}
				}
			}

			// test if there are no duplicate primary keys
			for (unsigned int i = 0; i < stmt.key_idx.size(); i++) {
				for (int j = i+1; j < stmt.key_idx.size(); j++) {
					if (stmt.key_idx[i] == stmt.key_idx[j]) {
						std::cerr << "Duplicate primary keys found!" << std::endl;
						return;
					}
				}
			}

			// test if primary keys contains only columns in the table
			for (unsigned int i = 0; i < stmt.key_idx.size(); i++) {
				bool findKey = false;
				for (auto& p : stmt.prop_list) {
					if (p.id == stmt.key_idx[i]) {
						findKey = true;
						break;
					}
				}
				if (!findKey) {
					std::cerr << "primary key not in property list" << std::endl;
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
		case DELETE: {
			const_cast<Table&>(*it).Delete(stmt.cond);
		}
			break;
		case INSERT: {
			// test if there are duplicate columns
			std::list<Property>::const_iterator it1, it2;
			for (it1 = stmt.prop_list.begin(); it1 != stmt.prop_list.end(); it1++) {
				it2 = it1;
				for (it2++; it2 != stmt.prop_list.end(); it2++) {
					if (it1->id == it2->id) {
						std::cerr << "Duplicate column names found!"<< std::endl;
						return;
					}
				}
			}			

			// test if all columns are in the schema of the table
			for (it1 = stmt.prop_list.begin(); it1 != stmt.prop_list.end(); it1++) {
				bool find = false;
				for (int i = 0; i < it->props.size(); i++) {
					if (it1->id == it->props[i].id) {
						find = true;
						break;
					}
				}
				if (!find) {
					std::cerr << "all columns should be in the schema of the table" << std::endl;
					return;
				}
			}

			// num of columns should equal to num of values
			if (stmt.value_list.size() != stmt.prop_list.size()) {
				std::cerr << "Number of columns should equal to Number of values"<< std::endl;
				return;
			}

			// check primary key constraint violation(It is done in Table::Insert() )
			const_cast<Table&>(*it).Insert(stmt.value_list, stmt.prop_list);
		}
			break;
		case QUERY: {
			// test if  all columns (except *) in the select list is in the schema of the table

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
					bool find = false;
					ids.push_back(i.id);
					for (unsigned int j = 0; j < it->props.size(); j++) {
						if (it->props[j].id == i.id) {
							indexes.push_back(j);
							find = true;
						}
					}
					if (!find) {
						std::cerr << "all columns (except *) in the select list should be in the schema of the table" << std::endl;
						return;
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

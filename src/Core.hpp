/*
 * Core.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: Sun Jiacheng, Sun Dongliang
 */

#ifndef CORE_HPP_
#define CORE_HPP_

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Exceptions.hpp"
#include <set>
#include <vector>
#include <iostream>

struct Row {
	int key;
	std::vector<int> keys;
	std::vector<int> cols;
	bool operator==(const Row& r) const {
		if (sizeof(keys) == 0 || sizeof(r.keys) == 0) {
			return key == r.key;
		}
		for (auto& i : keys) {
			if (cols[i] == r.cols[i])
				return true;
		}
		return false;
	}

	bool operator<(const Row& r) const {
		return key < r.key;
	}

	Row(const std::vector<int>& c, std::vector<int> k, int count) {
		cols = c;
		keys = k;
		if (keys.size() != 0) {
			key = cols[keys[0]];
		} else {
			key = count;
		}
	}

	//Dummy usage
	Row(int k) {
		key = k;
	}
};

struct Table {
	//table id
	std::string id;
	//table props
	std::vector<Property> props;
	//primary key index
	std::vector<int> key_idx;
	//if no primary key, used for search
	unsigned long long count;
	//use string to store various type of values
	std::set<Row> rows;

	//insert a record into table
	//if value not specified, use default value
	void Insert(const std::vector<int>& record, const std::list<Property>& prop_list);
	//delete a record from table
	void Delete(const Condition* cond);
	// return keys of rows that matches the condition
	std::vector<int> Query(const Condition* cond);

	Table(const std::string& i, const std::vector<std::string>& ki,
			const std::list<Property>& p) {
		count = 1;
		int n = 0;
		for (auto& i : ki) {
			for (auto& j : p) {
				if (j.id == i) {
					key_idx.push_back(n);
					break;
				}
			}
			n++;
		}
		for (auto& i : p) {
			props.push_back(i);
		}
		id = i;
	}

	bool operator<(const Table& t) const {
		return id < t.id;
	}
};

/*
 * Main backend of SimpleDB
 */
class SimpleDB {
private:
	//execute a statement
	void Execute(const Statement& stmt);
	Parser* parser;
	std::set<Table> tables;

public:
	SimpleDB() {
		//init parser
		parser = new Parser();
	}
	//public method, execute raw string
	void Execute(const std::string& stmt) {
		Statement s;
		try {
			s = parser->Parse(stmt);
		} catch (SDBException& e) {
			e.Print();
			return;
		}
		Execute(s);
	}

};

#endif /* CORE_HPP_ */

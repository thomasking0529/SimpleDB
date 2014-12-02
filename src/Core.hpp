/*
 * Core.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#ifndef CORE_HPP_
#define CORE_HPP_

#include "Lexer.hpp"
#include "Parser.hpp"
#include <set>

struct Row {
	int key;
	std::vector<int> cols;
	bool operator==(const Row& r) const {
		return key == r.key;
	}

	bool operator<(const Row& r) const {
		return key < r.key;
	}

	Row(const std::vector<int>& c, int k_idx, int count) {
		cols = c;
		if (k_idx != -1) {
			key = c[k_idx];
		} else {
			key = count;
		}
	}
};

struct Table {
	//table id
	std::string id;
	//table props
	std::vector<Property> props;
	//primary key index
	//-1 for no primary key
	int key_idx;
	//if no primary key, used for search
	unsigned long long count;
	//use string to store various type of values
	std::set<Row> rows;
	//insert a record to table
	//if value not specified, use default value
	void Insert(const std::vector<int>& record);
	void Delete(const Condition* cond);

	Table(const std::string& i, int ki, const std::vector<Property>& p) {
		count = 1;
		key_idx = ki;
		id = i;
		props = p;
	}
	bool operator<(const Table& t) const {
		return id < t.id;
	}
};

/*
 *
 * CREATE
 *    table name doesn't match with any existed table
 *
 * INSERT
 *    the table should exist
 *    no primary key constraint violation
 *    all columns should be in the schema of the table
 *    For a column without specified value, default
 *    value is used.
 *
 * WHERE
 *    columns occurring in the where clause (if any)
 *    should be in the schema of the table
 *
 * DELETE
 *    If there is a where clause, delete all rows
 *    whose where clause is evaluated to be TRUE.
 *    Otherwise, delete all rows in the table.
 *
 * SELECT
 *    If a where clause is present, those rows whose
 *    where clauses are evaluated to FALSE should
 *    be omitted. Otherwise, none should be omitted.
 *
 *    If '*' is present in the select list, all columns
 *    should be returned. Otherwise, return only
 *    those columns specified in the select list.
 */

class SimpleDB {
private:
	Parser* parser;
	std::set<Table> tables;
	/*
	 * Specially, for a query, print the result in a neat
	 * way. The effect should be similar to:
	 */

	/*
	 * Error handling:
	 *    Do nothing but print meaningful message if a statement
	 *  has consistency problem.
	 *    Print result if success
	 */
	void Execute(const Statement& stmt);

public:
	SimpleDB() {
		parser = new Parser();
	}
	void Execute(const std::string& stmt) {
		Statement s = parser->Parse(stmt);
		Execute(s);
	}
};

#endif /* CORE_HPP_ */

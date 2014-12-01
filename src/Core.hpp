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
	std::list<std::string> cols;
};

struct Table {
	//table id
	std::string id;
	//table props
	std::list<Property> props;
	//primary key
	std::string key;
	//use string to store various type of values
	std::list<Row> rows;
	//insert a record to table
	//if value not specified, use default value
	void insert(const std::list<std::string>& record);
	Table(const std::string& i, const std::string& k,
			const std::list<Property>& p) {
		key = k;
		id = i;
		props = p;
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
	std::list<Table> tables;
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

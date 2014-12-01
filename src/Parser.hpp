/*
 * Parser.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "Lexer.hpp"
#include <iostream>

enum Action {
	CREATE, DELETE, INSERT, QUERY, INVALID,
};

enum Op {
	PLUS, // +, both unary and binary
	MINUS, // -, both unary and binary
	MULTIPLY, // *
	DIVIDE, // /
	LT, // <
	GT, // >
	NE, // <>
	E, // ==
	GTE, // >=
	LTE, // <=
	AND, // &&
	OR, // ||
	NOT, // !
	EQ, // =
	LB, // (
	RB, // )
	COMMA, // ,
};

/*
 * reduce to boolean tree
 * only contains boolean operations for <id, num> pairs
 */

struct Condition {
	/*
	 * operand
	 * 		ID
	 * 		NUM
	 */
	std::string opd;
	// left operand
	Condition* lc;
	//right
	Condition* rc;
	// operator
	Op op;
};

/*
 * parser tree:
 * tree->ID OP NUM
 * 		| ID OP ID
 * 		| NUM OP NUM
 * 		| NUM OP ID
 * 		| tree OP tree
 */

/*
 * Only int current
 */

enum PropType {
	INT,
};

struct Property {
	/*
	 * id (identifier) is a sequence of digits, underline and letters. All
	 * identifiers should start with a letter or an underline. The maximum
	 * length of an identifier is 64.
	 */
	std::string id;
	/*
	 * property type
	 */
	PropType type;
	/*
	 default 0 for default value
	 */
	std::string default_value;

	bool operator==(const Property& o) {
		return o.id == id;
	}
};

struct Statement {
	/*
	 * action
	 */
	Action act;

	/*
	 * operand table
	 */
	std::string table;

	/*
	 * list of properties to return
	 * used with select and create
	 */
	std::list<Property> prop_list;

	/*
	 * value list, only for insert
	 */
	std::list<std::string> value_list;

	/*
	 * boolean tree
	 * NULL for none(insert and
	 */
	Condition* cond;
};

/*ERROR Handling
 * The parser should handle the ill-formed statements, but
 * not check its consistency.
 * To check:
 *   illegal grammar
 * Not to check:
 *   table existence
 *   value consistency
 * TODO:
 * Print meaningful error prompts
 * When an error is encountered, stop parsing.
 * what's the error
 * LINE NUMBER HANDLED BY MAIN.CPP
 */

/*
 * ssql_stmt -> create_stmt
 *    | insert_stmt
 *    | delete_stmt
 *    | query_stmt
 */

class Parser {
private:

	/*CREATE
	 *
	 *create_stmt → create table id (decl_list);
	 *    decl_list -> decl | decl_list, decl
	 *    decl -> id int default_spec | primary key(column_list)
	 *    default_spec → default = num | ε
	 *    column_list →id | column_list, id
	 *
	 * EXAMPLE:
	 * CREATE TABLE Student(sid INT, age INT DEFAULT = 18, PRIMARY KEY (sid));
	 *
	 * To check:
	 *   no duplicate column names
	 *   no two or more primary key declarations
	 *   primary key contains only columns in the table
	 *   # of columns in a table should <= 100
	 *   # of columns in primary key declaration <= 100
	 */

	/*INSERT
	 *
	 * insert_stmt → insert into id(column_list) values (value_list);
	 *     value_list → num | value_list, num
	 *
	 * To check:
	 *    no duplicate columns (see the example below)
	 *    # of columns should equal to # of values
	 */

	/*DELETE
	 *
	 * delete_stmt → delete from id where_clause;
	 *
	 * To check:
	 *
	 */

	/*SELECT
	 *
	 * select_stmt → select select_list from id where_clause;
	 *      select_list → column_list | *
	 *
	 * To check:
	 *
	 */

	/*WHERE
	 * Return paired and reduced condition list
	 *
	 * where_clause → where conjunct_list | ε
	 *     conjunct_list → bool | conjunct_list && bool
	 *     bool →operand rop operand
	 *     operand → num | id
	 *     rop → <> | == | > | < | >= | <=
	 *
	 * To check
	 *
	 */
	Lexer* lexer;

public:
	Parser();
	Statement Parse(const std::string& s);
};

#endif /* PARSER_HPP_ */

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

struct Condition {
	// left operand
	std::string lop;
	// right operand
	std::string rop;
	// operator
	Op op;
};

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
	 0 for default value
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
	 * list of conditions
	 * used with where
	 */
	std::list<Condition> conds;
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

	Statement parseCreate(const std::list<Token>& token_list);

	/*INSERT
	 *
	 * insert_stmt → insert into id(column_list) values (value_list);
	 *     value_list → num | value_list, num
	 *
	 * To check:
	 *    no duplicate columns (see the example below)
	 *    # of columns should equal to # of values
	 */

	Statement parseInsert(const std::list<Token>& token_list);

	/*DELETE
	 *
	 * delete_stmt → delete from id where_clause;
	 *
	 * To check:
	 *
	 */

	Statement parseDelete(const std::list<Token>& token_list);

	/*SELECT
	 *
	 * select_stmt → select select_list from id where_clause;
	 *      select_list → column_list | *
	 *
	 * To check:
	 *
	 */

	Statement parseSelect(const std::list<Token>& token_list);

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

	std::list<Condition> parseWhere(const std::list<Token>& token_list);

public:
	Parser();
	Statement Parse(const std::list<Token>& token_list) {
		TokenType t = token_list.front().type;

		Statement wt;
		wt.act = INVALID;
		if (t != KEYWORD) {
			std::cerr << "Illegal \n";
			return wt;
		}

		std::string op_type = token_list.front().value;
		if (op_type == "CREATE") {
			return parseSelect(token_list);
		} else if (op_type == "INSERT") {
			return parseInsert(token_list);
		} else if (op_type == "DELETE") {
			return parseDelete(token_list);
		} else if (op_type == "SELECT") {
			return parseSelect(token_list);
		} else {
			std::cerr << "Unknown Keywords\n";
			return wt;
		}
	}
};

#endif /* PARSER_HPP_ */

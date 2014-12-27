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
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stack>
#include <list>
#include <string>
#include <queue>

enum Action {
	CREATE, DELETE, INSERT, QUERY, INVALID,
};

//arithmetic operators
//for parser only

//boolean operators
enum BoolAndArithOp {
	LT, // <
	GT, // >
	NE, // <>
	EQ, // ==
	GTE, // >=
	LTE, // <=
	AND, // &&
	OR, // ||
	NOT, // !
	PLUS, // +, both unary and binary
	MINUS, // -, both unary and binary
	MULTIPLY, // *
	DIVIDE, // /
	E, // =
	LB, // (
	RB, // )
	COMMA, // ,
};

/*
 * reduce to boolean tree
 * only contains boolean operations for <id, num> or <id, id> pairs
 */

struct Condition {
	/*
	 * operand
	 * 		ID
	 * 		NUM
	 */
	// operator
	BoolAndArithOp op;
	

	//TODO:
	//use isNum and isId to check opd
	std::string opd;

	// left operand
	Condition* lc;

	//right
	Condition* rc;

	Condition() {
		lc = rc = NULL;
	}

	Condition(BoolAndArithOp o) {
		op = o;
		lc = rc = NULL;
	}

	Condition(std::string s) {
		opd = s;
		lc = rc = NULL;
	}
};

/*
 * parser tree:
 * tree->ID OP NUM
 * 		| ID OP ID
 * 		| NUM OP NUM
 * 		| NUM OP ID
 * 		| tree OP tree
 * NOT operator generate right subtree
 */

struct Property {
	/*
	 * id (identifier) is a sequence of digits, underline and letters. All
	 * identifiers should start with a letter or an underline. The maximum
	 * length of an identifier is 64.
	 */
	std::string id;

	/*
	 default 0 for default value if no default specified
	 */
	int default_value;

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
	 * location of primary key
	 * used with create
	 * -1 for no primary key
	 */
	std::vector<std::string> key_idx;

	/*
	 * value list, only for insert
	 */
	std::vector<int> value_list;


	/*
	 * boolean tree
	 * NULL for none(insert and
	 */
	Condition* cond;

	Statement() {
		cond = NULL;
	}

};
class Polish {
private:
	bool flag;
	int isNeglect;
	int count;
	std::list<std::string> ops;
	std::queue<std::string> rp;
	std::map<std::string, int> level;
	std::set<std::string> _ops;
public:
	Polish() {
		level.insert(std::pair<std::string, int>("+", 3));
		level.insert(std::pair<std::string, int>("-", 3));
		level.insert(std::pair<std::string, int>("*", 4));
		level.insert(std::pair<std::string, int>("/", 4));
		level.insert(std::pair<std::string, int>("==", 1));
		level.insert(std::pair<std::string, int>("<>", 1));
		level.insert(std::pair<std::string, int>(">", 2));
		level.insert(std::pair<std::string, int>("<", 2));
		level.insert(std::pair<std::string, int>(">=", 2));
		level.insert(std::pair<std::string, int>("<=", 2));
		level.insert(std::pair<std::string, int>("!", 0));
		level.insert(std::pair<std::string, int>("&&", -1));
		level.insert(std::pair<std::string, int>("||", -1));
		level.insert(std::pair<std::string, int>("(", -2));
		level.insert(std::pair<std::string, int>(")", -2));
		_ops.insert("+");
		_ops.insert("-");
		_ops.insert("*");
		_ops.insert("/");
		_ops.insert("<");
		_ops.insert(">");
		_ops.insert("<=");
		_ops.insert(">=");
		_ops.insert("==");
		_ops.insert("<>");
		_ops.insert("&&");
		_ops.insert("||");
		_ops.insert("!");
		isNeglect = 0;
		count = -1;
		flag = false;
	}
	void neglect() {
		flag = true;
	}
	void Insert(const std::string& item) {
		if (_ops.find(item) != _ops.end()) {
			while (!ops.empty() && level.find(item)->second <= level.find(ops.back())->second) {
				rp.push(ops.back());
				ops.pop_back();
			}
			ops.push_back(item);
		} else if (item == "(") {
			if (flag) {
				flag = false;
				ops.push_back("(");
				rp.push("0");
				Insert("-");
				isNeglect++;
				count++;
			} else if (count != -1) {
				isNeglect++;
			}
			ops.push_back("(");
		} else if (item == ")") {
			if (count != -1) {
				isNeglect--;
				if (isNeglect == count) {
					while (ops.back() != "(") {
						rp.push(ops.back());
						ops.pop_back();
					}
					ops.pop_back();
					count--;
				}
			}
			while (ops.back() != "(") {
				rp.push(ops.back());
				ops.pop_back();
			}
			ops.pop_back();
		} else {
			if (flag) {
				int tm;
				if (isNum(tm, item)) {
					rp.push("-" + item);
				} else {
					ops.push_back("(");
					rp.push("0");
					Insert("-");
					rp.push(item);
					while (ops.back() != "(") {
						rp.push(ops.back());
						ops.pop_back();
					}
					ops.pop_back();
				}
				flag = false;
			} else {
				rp.push(item);
			}
		}
	}
	int Calculate() {
		std::stack<int> nums;
		while (!ops.empty()) {
			rp.push(ops.back());
			ops.pop_back();
		}
		while (!rp.empty()) {
			std::string to_ret = rp.front();
			rp.pop();
			if (_ops.find(to_ret) != _ops.end()) {
				if (nums.size() < 2) {
					std::cerr << "Wrong expression.\n";
					return 0;
				} else {
					int b = nums.top();
					nums.pop();
					int a = nums.top();
					nums.pop();
					if (to_ret == "+") {
						nums.push(a + b);
					} else if (to_ret == "*") {
						nums.push(a * b);
					} else if (to_ret == "-") {
						nums.push(a - b);
					} else if (to_ret == "/") {
						nums.push(a / b);
					}
				}
			} else {
				nums.push(atoi(to_ret.c_str()));
			}
		}
		if (nums.size() != 1) {
			std::cerr << "Wrong expression.\n";
			return 0;
		} else {
			int t = nums.top();
			nums.pop();
			return t;
		}
	}
	Condition* buildTree() {
		Condition *t = new Condition();
		std::stack<Condition*> nums;
		while (!ops.empty()) {
			rp.push(ops.back());
			ops.pop_back();
		}
		while (!rp.empty()) {
			std::string to_ret = rp.front();
			rp.pop();
			if (_ops.find(to_ret) != _ops.end()) {
				if (to_ret != "!" && nums.size() < 2) {
					std::cerr << "Wrong expression.\n";
					return 0;
				} else if (to_ret != "!") {
					Condition* b = nums.top();
					nums.pop();
					Condition* a = nums.top();
					nums.pop();
					if (to_ret == "+") {
						int numa, numb;
						if (isNum(numa, a->opd) && isNum(numb, b->opd)) {
							int result = numa + numb;
							char a[100];
							sprintf(a, "%d", result);
							nums.push(new Condition(a));
						} else {
							Condition* re = new Condition();
							re->lc = b;
							re->rc = a;
							re->opd = "no num";
							re->op = PLUS;
							nums.push(re);
						}
					} else if (to_ret == "*") {
						int numa, numb;
						if (isNum(numa, a->opd) && isNum(numb, b->opd)) {
							int result = numa * numb;
							char a[100];
							sprintf(a, "%d", result);
							nums.push(new Condition(a));
						} else {
							Condition* re = new Condition();
							re->lc = b;
							re->rc = a;
							re->opd = "no num";
							re->op = MULTIPLY;
							nums.push(re);
						}
					} else if (to_ret == "-") {
						int numa, numb;
						if (isNum(numa, a->opd) && isNum(numb, b->opd)) {
							int result = numa - numb;
							char a[100];
							sprintf(a, "%d", result);
							nums.push(new Condition(a));
						} else {
							Condition* re = new Condition();
							re->lc = b;
							re->rc = a;
							re->opd = "no num";
							re->op = PLUS;
							nums.push(re);
						}
					} else if (to_ret == "/") {
						int numa, numb;
						if (isNum(numa, a->opd) && isNum(numb, b->opd)) {
							int result = numa / numb;
							char a[100];
							sprintf(a, "%d", result);
							nums.push(new Condition(a));
						} else {
							Condition* re = new Condition();
							re->lc = b;
							re->rc = a;
							re->opd = "no num";
							re->op = PLUS;
							nums.push(re);
						}
					} else if (to_ret == "<"){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = LT;
						nums.push(re);
					} else if (to_ret == ">"){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = GT;
						nums.push(re);
					} else if (to_ret == "<="){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = LTE;
						nums.push(re);
					} else if (to_ret == ">="){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = GTE;
						nums.push(re);
					} else if (to_ret == "<>"){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = NE;
						nums.push(re);
					} else if (to_ret == "=="){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = EQ;
						nums.push(re);
					} else if (to_ret == "&&"){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = AND;
						nums.push(re);
					} else if (to_ret == "||"){
						Condition* re = new Condition();
						re->lc = b;
						re->rc = a;
						re->opd = "no num";
						re->op = OR;
						nums.push(re);
					}
				} else if (to_ret == "!") {
					if (nums.size() < 1) {
						std::cerr << "Wrong expression.\n";
						return 0;
					}
					Condition* a = nums.top();
					nums.pop();
					Condition* re = new Condition();
					re->lc = a;
					re->opd = "no num";
					re->op = NOT;
					nums.push(re);
				}
			} else {
				Condition* tmp = new Condition();
				tmp->opd = to_ret;
				nums.push(tmp);
			}
		}
		if (nums.size() != 1) {
			std::cerr << "Wrong expression.\n";
			return 0;
		} else {
			Condition* t = nums.top();
			nums.pop();
			return t;
		}
	}

	bool isNum(int &num, std::string str) {
		int i = 0;
		while (str[i] == '-') i++;
		for (; i < str.size(); i++) {
			if (str[i] > '9' || str[i] < '0') {
				return false;
			}
		}
		num = std::atoi(str.c_str());
		return true;
	}
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

	Condition* parseWhere(std::list<Token> ts);

	/*WHERE
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
	void initTable();
	void initTerminal();
	void initAction();
	typedef void (*act)(Statement &s, Token t, std::string father, Polish* &calc);
	std::map<std::pair<std::string, std::string>, std::list<std::string> > table;
	std::set<std::string> terminal;
	std::map<std::string, act> action;
	std::stack<std::string> procedure;
	std::string getTokenSymbol(Token t);
public:
	Parser();
	Statement Parse(const std::string& s);
};

#endif /* PARSER_HPP_ */

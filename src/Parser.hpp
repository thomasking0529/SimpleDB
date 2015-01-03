/*
 * Parser.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: Sun Jiacheng, Qiu Zhilin
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


//operators
enum Op {
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
	// operator
	Op op;

	//operand
	std::string opd;

	// left subtree
	Condition* lc;

	//right subtree
	Condition* rc;

	Condition() {
		lc = rc = NULL;
	}

	Condition(Op o) {
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
	 * Property ID
	 */
	std::string id;

	/*
	 default 0 for default value if no default specified
	 */
	int default_value;

	//operator overloading
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
	 * list of primary key
	 * used with create
	 */
	std::vector<std::string> key_idx;

	/*
	 * value list, only for insert
	 * int values only
	 */
	std::vector<int> value_list;

	/*
	 * boolean tree
	 * NULL for none(insert and create)
	 */
	Condition* cond;

	Statement() {
		cond = NULL;
	}

};

/*
 * reverse polish calculater, the parser function use it to translate math
 * expression into number and translate bool expression to a condition
 * tree.
 */
class Polish {
private:
	/*
	 * flag, isNeglect, and count used for deal with the higher priority
	 * of - in express "-num" or "-id"
	 */
	bool flag;
	int isNeglect;
	int count;

	/*
	 * data structure used for translation to reverse polish expression
	 */
	std::list<std::string> ops;
	std::queue<std::string> rp;
	std::map<std::string, int> level;
	std::set<std::string> _ops;
public:
	/*
	 * constructor, init basic priority
	 */
	Polish();

	/* 
	 * - in expression  "-num" or "-id" found
	 */
	void neglect();

	/*
	 * call when parser found a token in a expression, tranlate
	 * the expression by tokens
	 */
	void Insert(const std::string& item);

	
	/*
	 * if the math expression is translate, calculate the value of
	 * the expression.
	 */
	int Calculate();

	/*
	 * if the bool expression is translate, build a condition tree of
	 * the expression.
	 */
	Condition* buildTree();

	/*
	 * common fuction for testing  whether a string is an number
	 * and if it is, the number store in num.
	 */
	bool isNum(int &num, std::string str);
};

class Parser {
private:
	/*
	 * lexer is where the tokens from
	 */
	Lexer* lexer;

	/*
	 * the action function definision, valid token t need to translate into the 
	 * Statement s,  given the Grammar Productions' left part where this token
	 * is produced to help the translation, and if token is in an expression, the
	 * Polish calc will be useful.
	 */
	typedef void (*act)(Statement &s, Token t, std::string father, Polish* &calc);

	/*
	 * the following three function is to init those table, set and map
	 * by reading the rule file we make by LL1. table gives the predictive
	 * parsing table, terminal defines which token is terminal, and the 
	 * action is the action we should take to make translattion when we
	 * recognize an valid token
	 */
	void initTable();
	void initTerminal();
	void initAction();
	std::map<std::pair<std::string, std::string>, std::list<std::string> > table;
	std::set<std::string> terminal;
	std::map<std::string, act> action;


	/*
	 * procedure is the stack use for predictive parsing
	 */
	std::stack<std::string> procedure;

	/*
	 * common function to get the token sysbol in the  grammar production
	 * to see whether the token match a sysbol in production.
	 */
	std::string getTokenSymbol(Token t);

	/*
	 * turn a num into string
	 */
	static std::string intToString(int num);
public:
	Parser();
	/*
	 * main parse function
	 */
	Statement Parse(const std::string& s);
};

#endif /* PARSER_HPP_ */

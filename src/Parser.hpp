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

//reverse polish calculater
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
		level.insert(std::pair<std::string, int>("||", -2));
		level.insert(std::pair<std::string, int>("(", -3));
		level.insert(std::pair<std::string, int>(")", -3));
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
	
	//check neglect number
	void neglect() {
		flag = !flag;
	}

	//get input tokens
	void Insert(const std::string& item) {
		if (_ops.find(item) != _ops.end()) {
			if (item == "!") Insert("!!");
			while (!ops.empty() &&
				level.find(item)->second <= level.find(ops.back())->second) {
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
					int result = -tm;
					char a[100];
					sprintf(a, "%d", result);
					rp.push(std::string(a));
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

	//calculate inputs
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

	//build condition tree
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
				if (nums.size() < 2) {
					std::cerr << "Wrong expression.\n";
					return 0;
				} else {
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
							re->lc = a;
							re->rc = b;
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
							re->lc = a;
							re->rc = b;
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
							re->lc = a;
							re->rc = b;
							re->opd = "no num";
							re->op = MINUS;
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
							re->lc = a;
							re->rc = b;
							re->opd = "no num";
							re->op = DIVIDE;
							nums.push(re);
						}
					} else if (to_ret == "<"){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = LT;
						nums.push(re);
					} else if (to_ret == ">"){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = GT;
						nums.push(re);
					} else if (to_ret == "<="){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = LTE;
						nums.push(re);
					} else if (to_ret == ">="){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = GTE;
						nums.push(re);
					} else if (to_ret == "<>"){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = NE;
						nums.push(re);
					} else if (to_ret == "=="){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = EQ;
						nums.push(re);
					} else if (to_ret == "&&"){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = AND;
						nums.push(re);
					} else if (to_ret == "||"){
						Condition* re = new Condition();
						re->lc = a;
						re->rc = b;
						re->opd = "no num";
						re->op = OR;
						nums.push(re);
					} else if (to_ret == "!") {
						if (a->opd == "!!" && b->opd == "!!") {
							nums.push(a);
						} else if (a->opd == "!!") {
							Condition* re = new Condition();
							re->lc = b;
							re->opd = "no num";
							re->op = NOT;
							nums.push(re);
						} else {
							std::cerr << "Wrong expression.\n";
							return 0;
						}
					}
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

class Parser {
private:
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

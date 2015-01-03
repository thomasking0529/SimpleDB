/*
 * Parser.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: Qiu Zhilin
 */

#define RULE "Rules.txt"
#define TERMINAL "Terminals.txt"
#include <string>
#include <cstring>
#include <stack>
#include <fstream>  
#include "Parser.hpp"

/*
 * constructor, init basic priority, and define operator
 */
Polish::Polish() {
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

/* 
 * - in expression  "-num" or "-id" found, reverse the flag, which can
 * reduce somthing like ------num.
 */
void Polish::neglect() {
	flag = !flag;
}

/*
 * call when parser found a token in a expression, tranlate
 * the token in the expression.
 */
void Polish::Insert(const std::string& item) {
	if (_ops.find(item) != _ops.end()) {
		// Unit operator ! found, make it become a binary one and 
		// always give another operand "!!"
		if (item == "!") Insert("!!");
		while (!ops.empty() &&
			level.find(item)->second <= level.find(ops.back())->second) {
			rp.push(ops.back());
			ops.pop_back();
		}
		ops.push_back(item);
	} else if (item == "(") {
		if (flag) {
			// here is a situation where -(expression) is detected, we need
			// to turn it to (0-(expression)). thus we begin counting the "("
			// to see whether is time to insert the ")"
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
				// time to insert ")" for (0-(expression))
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
			// - num or - id detected
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

/*
 * if the math expression is translate, calculate the value of
 * the expression. get two operand and an operator a time
 * calculate and push back. until no operand
 */
int Polish::Calculate() {
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
					if (b == 0) {
						throw SDBException("can't devided by zero");
					}
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

/*
 * if the bool expression is translate, build a condition tree of
 * the expression. Get two operand and an operator a time
 * build a Condtion node and push back. until no operand, the we
 * get the root Condition node.
 */
Condition* Polish::buildTree() {
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
						if (numb == 0) {
							throw SDBException("can't devided by zero");
						}
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

/*
 * common fuction for testing  whether a string is an number
 * and if it is, the number store in num.
 */
bool Polish::isNum(int &num, std::string str) {
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

// Parser constructor, call init function.
Parser::Parser() {
	initTable();
	initTerminal();
	initAction();
	lexer = new Lexer();
}

//init the predictive parsing table, 
void Parser::initTable() {
	FILE* file = fopen(RULE, "r");
	if (file == NULL) {
		std::cerr << "Rule file " << RULE << " not found\n";
		return;
	}
	char rule[200], *next;
	std::string top, front, content;
	while (fgets(rule, 200, file)) {
		std::list<std::string> lst;
		top = strtok(rule, " ");
		front = strtok(NULL, " ");
		next = strtok (NULL, " ");
		while (next != NULL) {
			lst.push_back(next);
			next = strtok (NULL, " ");
		}
		lst.pop_back();
		table[std::make_pair(top, front)] = lst;
	}
	fclose(file);
}

//init terminals set which defines which token is terminal
void Parser::initTerminal() {
	FILE* file = fopen(TERMINAL, "r");
	if (file == NULL) {
		std::cerr << "Terminal file " << TERMINAL << " not found\n";
		return;
	}
	char rule[200];
	std::string id = "id", num = "num";
	while (!feof(file)) {
		fscanf(file, "%s", rule);
		terminal.insert(rule);
	}
	fclose(file);
}
/*
 * init the action table, which define when valid token t need to translate into the 
 * Statement s,  given the Grammar Productions' left part where this token
 * is produced to help the translation, and if token is in an expression, the
 * Polish calc will be useful.
 */
void Parser::initAction() {
	std::for_each(terminal.begin(), terminal.end(), 
		[&](const std::string& ter){
			action[ter] = [](Statement &s, Token t, std::string father, Polish* &calc) {
				// for those token has no use, give empty action
			};
	});
	action["select"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = QUERY;};
	action["create"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = CREATE;};
	action["insert"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = INSERT;};
	action["delete"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = DELETE;};
	action["where"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		// need an Polish to calculate bool expression
		calc = new Polish();
	};
	action["id"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "create_stmt" || father == "insert_stmt"
			|| father == "delete_stmt" || father == "query_stmt"){
				s.table = t.value;
		} else if (father == "decl") {
			Property p;
			p.id = t.value;
			p.default_value = 0;
			s.prop_list.push_back(p);
		} else if (father == "column_list") {
			if (s.act == CREATE) {
				s.key_idx.push_back(t.value);
			} else if (s.act == QUERY || s.act == INSERT) {
				Property p;
				p.id = t.value;
				p.default_value = 0;
				s.prop_list.push_back(p);
			}
		} else if (calc != NULL && father == "unary") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["num"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "simple_unary") {
			if (calc == NULL) calc = new Polish();
			calc->Insert(t.value);
		} else if (calc != NULL && father == "unary") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["<="] = action[">="] = action["<"] = action[">"] = action["=="] = action["<>"]
	= [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "rop") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["&&"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "conjunct*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["||"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "disjunct*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["!"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "bool") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["*"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "select_list") {
			Property p;
			p.id = t.value;
			p.default_value = 0;
			s.prop_list.push_back(p);
		} else if (calc != NULL && father == "simple_term*"){
			calc->Insert(t.value);
		} else if (calc != NULL && father == "term*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["/"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_term*"){
			calc->Insert(t.value);
		} else if (calc != NULL && father == "term*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["+"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_expr*"){
			calc->Insert(t.value);
		} else if (father == "simple_unary"){
			if (calc == NULL) {
				// need an Polish to calculate math expression
				calc = new Polish();
			}
		} else if (calc != NULL && father == "expr*"){
			calc->Insert(t.value);
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action["-"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_expr*"){
			calc->Insert(t.value);
		} else if (father == "simple_unary"){
			if (calc == NULL) {
				// need an Polish to calculate math expression
				calc = new Polish();
			}
			calc->neglect();
		} else if (calc != NULL && father == "expr*"){
			calc->Insert(t.value);
		} else if (calc != NULL && father == "unary"){
			// unit operator - found
			calc->neglect();
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};	
	action["("] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "simple_unary"){
			if (calc == NULL) {
				// need an Polish to calculate math expression
				calc = new Polish();
			}
			calc->Insert(t.value);
		} else if (calc != NULL && father == "bool"){
			calc->Insert(t.value);
		} else if (father == "create_stmt" || father == "insert_stmt" || father == "decl"){
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action[")"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_unary"){
			calc->Insert(t.value);
		} else if (father == "create_stmt" || father == "insert_stmt"){
			if (calc != NULL) {
				int result = calc->Calculate();
				switch (s.act) {
				case CREATE:
					{
						Property p = s.prop_list.back();
						p.default_value = result;
						s.prop_list.pop_back();
						s.prop_list.push_back(p);
						break;
					}
				case INSERT:
					{
						s.value_list.push_back(result);
						break;
					}
				}
				delete calc;
				calc = NULL;
			} else {
				// ok
			}
		} else if (father == "bool") {
			calc->Insert(t.value);
		} else if (father == "decl") {
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};
	action[","] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && (father == "value_list*" || father == "decl_list*")){
			int result = calc->Calculate();
			switch (s.act) {
				case CREATE:
					{
						Property p = s.prop_list.back();
						p.default_value = result;
						s.prop_list.pop_back();
						s.prop_list.push_back(p);
						break;
					}
				case INSERT:
					{
						s.value_list.push_back(result);
						break;
					}
			}
			delete calc;
			calc = NULL;
		} else if (father == "column_list*" || father == "insert_stmt" || father == "decl_list*"){
		} else {
			throw SDBException("inside logically mistake \"" + t.value
				+ "\" at: column: " + intToString(t.lin)
				+ ", line: " + intToString(t.col));
		}
	};

}

/*
 * common function to get the token sysbol in the  grammar production
 * to see whether the token match a sysbol in production.
 */
std::string Parser::getTokenSymbol(Token t) {
	if (t.type == NUM) {
		return "num";
	} else if (t.type == ID) {
		return "id";
	} else {
		return t.value;
	}
}

//main parse function
Statement Parser::Parse(const std::string& s) {
	//get token list
	std::list<Token> ts = lexer->GetTokens(s);
			
	//clear procedure stack
	while (!procedure.empty()) {
		procedure.pop();
	}
	Statement st;
	procedure.push("$");
	procedure.push("ssql_stmt");
	int ip = 0;
	std::string top = procedure.top();
	std::stack<std::string> father;
	father.push("$");
	father.push("ssql_stmt");
	std::string fa;
	fa = father.top();
	Polish* calc = NULL;
	while (top != "$") {
		father.pop();
		if (top == "#") {
			// skip symbol, do nothing
			procedure.pop();
		} else if (terminal.count(top) > 0) {
			if (getTokenSymbol(ts.front()) != top) {
				throw SDBException("unexpected token \"" + ts.front().value
					+ "\" at: column: " + intToString(ts.front().lin)
					+ ", line: " + intToString(ts.front().col));
				break;
			}
			action[top](st, ts.front(), fa, calc);
			ts.pop_front();
			procedure.pop();
		} else {
			procedure.pop();
			std::list<std::string> af = table[std::make_pair(top, getTokenSymbol(ts.front()))];
			std::list<std::string>::reverse_iterator i = af.rbegin();
			for (; i != af.rend(); ++i) {
				father.push(top);
				procedure.push(*i);
			}
		}
		fa = father.top();
		top = procedure.top();
	}
	if (top != "$") throw SDBException("input end before parse done");
	if (calc != NULL && (st.act == DELETE || st.act == QUERY)) {
		st.cond = calc->buildTree();
	}
	if (ts.size() != 0) {
		throw SDBException("unprocessed token after:  \"" + ts.front().value
			+ "\" at: column: " + intToString(ts.front().lin)
			+ ", line: " + intToString(ts.front().col));
	}
	return st;
}

/*
 * turn a num into string
 */
std::string Parser::intToString(int num) {
	char a[100];
	sprintf(a, "%d", num);
	return a;
}

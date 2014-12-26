/*
 * Parser.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#define _CRT_SECURE_NO_WARNINGS
#define RULE "Rule_1.txt"
#define TERMINAL "Terminal_list.txt"
#include <string>
#include <cstring>
#include <stack>
#include <fstream>  
#include "Parser.hpp"

Parser::Parser() {
	initTable();
	initTerminal();
	initAction();
	lexer = new Lexer();
}

void Parser::initTable() {
	FILE* file = fopen(RULE, "r");
	if (file == NULL) {
		printf("sssss");
		return;
	}
	char rule[100], *next;
	std::string top, front, content;
	while (fgets(rule, 100, file)) {
		std::list<std::string> lst;
		top = strtok(rule, " ");
		front = strtok(NULL, " ");
		next = strtok(NULL, " ");
		while (next != NULL) {
			lst.push_back(next);
			next = strtok(NULL, " ");
		}
		table[std::make_pair(top, front)] = lst;
	}
	fclose(file);
}
void Parser::initTerminal() {
	FILE* file = fopen(TERMINAL, "r");
	if (file == NULL) {
		printf("sssss");
		return;
	}
	char rule[100];
	std::string id = "id", num = "num";
	while (!feof(file)) {
		fscanf(file, "%s", rule);
		terminal.insert(rule);
	}
	fclose(file);
}

void Parser::initAction() {
	std::for_each(terminal.begin(), terminal.end(),
			[&](const std::string& ter) {
				action[ter] = [](Statement &s, Token t, std::string father) {
					//printf("%s", t.value.c_str());
				};
			});
	action["select"] =
			[](Statement &s, Token t, std::string father) {s.act = QUERY;};
	action["create"] =
			[](Statement &s, Token t, std::string father) {s.act = CREATE;};
	action["insert"] =
			[](Statement &s, Token t, std::string father) {s.act = INSERT;};
	action["delete"] =
			[](Statement &s, Token t, std::string father) {s.act = DELETE;};
	action["id"] = [](Statement &s, Token t, std::string father) {
		if (father == "create_stmt" || father == "insert_stmt"
				|| father == "delete_stmt" || father == "query_stmt") {
			s.table = t.value;
		} else if (father == "decl") {
			Property p;
			p.id = t.value;
			p.default_value = 0;
			s.prop_list.push_back(p);
		} else if (father == "column_list") {
			if (s.act == CREATE) {
				s.key_idx.push_back(t.value);
			} else if (s.act == QUERY) {
				Property p;
				p.id = t.value;
				p.default_value = 0;
				s.prop_list.push_back(p);
			}
		} else if (father == "operand") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["num"] = [](Statement &s, Token t, std::string father) {
		if (father == "default_spec") {
			Property p = s.prop_list.back();
			p.default_value = atoi(t.value.c_str());
			s.prop_list.pop_back();
			s.prop_list.push_back(p);
		} else if (father == "value_list") {
			s.value_list.push_back(atoi(t.value.c_str()));
		} else if (father == "operand") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["<>"] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["=="] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action[">"] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["<"] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action[">="] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["<="] = [](Statement &s, Token t, std::string father) {
		if (father == "rop") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["&&"] = [](Statement &s, Token t, std::string father) {
		if (father == "conjunct_list*") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["||"] = [](Statement &s, Token t, std::string father) {
		if (father == "conjunct_list*") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["!"] = [](Statement &s, Token t, std::string father) {
		if (father == "conjunct_list*") {
			s.treeInsert(t);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["*"] = [](Statement &s, Token t, std::string father) {
		if (father == "select_list") {
			Property p;
			p.id = t.value;
			p.default_value = 0;
			s.prop_list.push_back(p);
		} else {
			throw SDBException("something wrong");
		}
	};
}

void Statement::treeInsert(Token node) {
	if (cond == NULL) {
		cond = new Condition();
	}
	Condition *toInsert = insertNodeSearch(cond);
	switch (node.type) {
	case ID: {
		if (toInsert->lc == NULL) {
			Condition *newNode = new Condition();
			newNode->opd = node.value;
			toInsert->lc = newNode;
		} else {
			Condition *newNode = new Condition();
			newNode->opd = node.value;
			toInsert->rc = newNode;
		}
		break;
	}
	case NUM: {
		if (toInsert->lc == NULL) {
			Condition *newNode = new Condition();
			newNode->opd = node.value;
			toInsert->lc = newNode;
		} else {
			Condition *newNode = new Condition();
			newNode->opd = node.value;
			toInsert->rc = newNode;
		}
		break;
	}
	case OP: {
		/*
		 LT, // <
		 GT, // >
		 NE, // <>
		 EQ, // ==
		 GTE, // >=
		 LTE, // <=
		 AND, // &&
		 OR, // ||
		 NOT, // !
		 */
		if (node.value == "<") {
			toInsert->op = LT;
		} else if (node.value == ">") {
			toInsert->op = GT;
		} else if (node.value == "<>") {
			toInsert->op = NE;
		} else if (node.value == "==") {
			toInsert->op = EQ;
		} else if (node.value == ">=") {
			toInsert->op = GTE;
		} else if (node.value == "<=") {
			toInsert->op = LTE;
		} else if (node.value == "&&") {
			rotate(cond);
			cond->op = AND;
		} else if (node.value == "||") {
			rotate(cond);
			cond->op = OR;
		} else if (node.value == "!") {
			rotate(cond);
			cond->op = NOT;
		}
		break;
	}
	default:
		throw SDBException("abcdefg");
	}
}

Condition* Statement::insertNodeSearch(Condition* &cond) {
	if (cond->lc == NULL || cond->rc == NULL) {
		return cond;
	} else {
		return insertNodeSearch(cond->lc);
	}
}

void Statement::rotate(Condition* &cond) {
	Condition* tmp = cond;
	cond = new Condition();
	cond->rc = tmp;
	cond->lc = new Condition();
}

std::string Parser::getTokenSymbol(Token t) {
	if (t.type == NUM) {
		return "num";
	} else if (t.type == ID) {
		return "id";
	} else {
		return t.value;
	}
}

Statement Parser::Parse(const std::string& s) {
	std::list<Token> ts = lexer->GetTokens(s);
	while (!procedure.empty()) {
		procedure.pop();
	}
	Statement st;
	procedure.push("$");
	procedure.push("ssql_stmt");
	std::string top = procedure.top();
	std::stack<std::string> father;
	father.push("$");
	father.push("ssql_stmt");
	std::string fa;
	fa = father.top();
	while (top != "$") {
		father.pop();
		if (top == "#") {
			procedure.pop();
		} else if (terminal.count(top) > 0) {
			if (ts.front().type != ID && ts.front().type != NUM
					&& ts.front().value != top) {
				throw SDBException("unexpected token");
				break;
			}
			action[top](st, ts.front(), fa);
			ts.pop_front();
			procedure.pop();
		} else {
			procedure.pop();
			std::list<std::string> af = table[std::make_pair(top,
					getTokenSymbol(ts.front()))];
			std::list<std::string>::reverse_iterator i = af.rbegin();
			for (; i != af.rend(); ++i) {
				father.push(top);
				procedure.push(*i);
			}
		}
		fa = father.top();
		top = procedure.top();
	}
	if (top != "$")
		throw SDBException("unknown error");
	return st;
}

Condition* Parser::parseWhere(std::list<Token> ts) {
	//example:
	Condition* c = new Condition(NE);
	return c;
}

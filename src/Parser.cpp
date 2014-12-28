/*
 * Parser.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: thomas
 */

#define RULE "Rule_2.txt"
#define TERMINAL "Terminal_list_2.txt"
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
void Parser::initTerminal() {
	FILE* file = fopen(TERMINAL, "r");
	if (file == NULL) {
		printf("sssss");
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

void Parser::initAction() {
	std::for_each(terminal.begin(), terminal.end(), 
		[&](const std::string& ter){
			action[ter] = [](Statement &s, Token t, std::string father, Polish* &calc) {
				//printf("%s", t.value.c_str());
			};
	});
	action["select"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = QUERY;};
	action["create"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = CREATE;};
	action["insert"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = INSERT;};
	action["delete"] = [](Statement &s, Token t, std::string father, Polish* &calc) {s.act = DELETE;};
	action["where"] = [](Statement &s, Token t, std::string father, Polish* &calc) {calc = new Polish();};
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
			} else if (s.act == QUERY) {
				Property p;
				p.id = t.value;
				p.default_value = 0;
				s.prop_list.push_back(p);
			}
		} else if (calc != NULL && father == "unary") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["num"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "simple_unary") {
			if (calc == NULL) calc = new Polish();
			calc->Insert(t.value);
		} else if (calc != NULL && father == "unary") {
			calc->Insert(t.value);
		} else {
			//throw SDBException("something wrong");
		}
	};
	action["<="] = action[">="] = action["<"] = action[">"] = action["=="] = action["<>"]
	= [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "rop") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["&&"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "conjunct*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["||"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "disjunct*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["!"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "bool") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
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
			throw SDBException("something wrong");
		}
	};
	action["/"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_term*"){
			calc->Insert(t.value);
		} else if (calc != NULL && father == "term*") {
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["+"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_expr*"){
			calc->Insert(t.value);
		} else if (father == "simple_unary"){
			if (calc == NULL) {
				calc = new Polish();
			}
		} else if (calc != NULL && father == "expr*"){
			calc->Insert(t.value);
		} else {
			throw SDBException("something wrong");
		}
	};
	action["-"] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (calc != NULL && father == "simple_expr*"){
			calc->Insert(t.value);
		} else if (father == "simple_unary"){
			if (calc == NULL) {
				calc = new Polish();
			}
			calc->neglect();
		} else if (calc != NULL && father == "expr*"){
			calc->Insert(t.value);
		} else if (calc != NULL && father == "unary"){
			calc->neglect();
		} else {
			throw SDBException("something wrong");
		}
	};	
	action["("] = [](Statement &s, Token t, std::string father, Polish* &calc) {
		if (father == "simple_unary"){
			if (calc == NULL) {
				calc = new Polish();
			}
			calc->Insert(t.value);
		} else if (calc != NULL && father == "bool"){
			calc->Insert(t.value);
		} else if (father == "create_stmt" || father == "insert_stmt"){
		} else {
			throw SDBException("something wrong");
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
		} else {
			throw SDBException("something wrong");
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
			throw SDBException("something wrong");
		}
	};

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
			procedure.pop();
		} else if (terminal.count(top) > 0) {
			if (ts.front().type != ID && ts.front().type != NUM
				&& ts.front().value != top) {
				throw SDBException("unexpected token");
				break;
			}
			//printf("%s    %s   \n", top.c_str(), fa.c_str());
			action[top](st, ts.front(), fa, calc);
			ts.pop_front();
			procedure.pop();
			//printf("next:  %s    %s   \n", ts.front().value.c_str(), procedure.top().c_str());
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
	if (top != "$") throw SDBException("unknown error");
	if (calc != NULL && (st.act == DELETE || st.act == QUERY)) {
		st.cond = calc->buildTree();
	}
	if (ts.size() != 0) {
		printf("haven't test condtion in parser.cpp");
		throw SDBException("unknown error");
	}
	return st;
}


Condition* Parser::parseWhere(std::list<Token> ts) {
	//example:
	Condition* c = new Condition(NE);
	return c;
}

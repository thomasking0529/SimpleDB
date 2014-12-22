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
#include <string.h>
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
		top = strtok(rule, " \n");
		front = strtok(NULL, " \n");
		next = strtok (NULL, " \n");
		while (next != NULL) {
			lst.push_back(next);
			next = strtok (NULL, " \n");
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
		[&](const std::string& ter){
			action[ter] = [](Statement &s, Token t, std::string last) {
				printf("%s", t.value.c_str());
			};
	});/*
	action["select"] = [](Statement &s, Token t, std::string last) {s.act = QUERY;};
	action["create"] = [](Statement &s, Token t, std::string last) {s.act = CREATE;};
	action["insert"] = [](Statement &s, Token t, std::string last) {s.act = INSERT;};
	action["delete"] = [](Statement &s, Token t, std::string last) {s.act = DELETE;};
	action["id"] = [](Statement &s, Token t, std::string last) {
		if (last == ""){

		}
	};*/

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
	while (top != "$") {
		if (top == "#") {
			procedure.pop();
		} else if (terminal.count(top) > 0) {
			if (ts.front().type != ID && ts.front().type != NUM
				&& ts.front().value != top) {
				throw SDBException("unexpected token");
				break;
			}
			action[top](st, ts.front(), top);
			ts.pop_front();
			procedure.pop();
		} else {
			procedure.pop();
			std::list<std::string> af
				= table[std::make_pair(top, getTokenSymbol(ts.front()))];
			std::list<std::string>::reverse_iterator i = af.rbegin();
			for (; i != af.rend(); ++i) {
				procedure.push(*i);
			}
		}
		top = procedure.top();
	}
	if (top != "$") throw SDBException("unknown error");
	return st;
}


Condition* Parser::parseWhere(std::list<Token> ts) {
	//example:
	Condition* c = new Condition(NE);
	return c;
}

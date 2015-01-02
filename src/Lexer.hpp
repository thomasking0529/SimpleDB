/*
 * Lexer.hpp
 *
 *  Created on: Nov 29, 2014
 *      Author: Sun Jiacheng
 */

#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <cstdlib>
#include <iostream>
#include <sstream>

const int MAX_ID_LEN = 64;

#include "Exceptions.hpp"

/*
 * common functions
 */

bool isNum(const std::string& s);
bool isId(const std::string& s);

//Token types
enum TokenType {
	KEYWORD, ID, NUM, OP
};

struct Token {
	//type of token
	TokenType type;
	//start location of token
	int col;
	//value of token, raw string
	int lin;
	std::string value;
	Token(TokenType t, const std::string& v, int c, int l) {
		lin = l;
		col = c;
		type = t;
		value = v;
	}
};

class Lexer {
private:
	//the column number
	int count;
	int line_count;
	std::set<char> symbols;
	std::set<std::string> ops;
	//split input string into a list
	std::list<std::string> split(const std::string& s);
	std::list<std::string> line_split(const std::string& s);
	std::vector<int> locs;
	std::string t;


	void saveTo(const std::string& t, std::list<std::string>& s) {
		s.push_back(t);
		count += t.length();
		locs.push_back(count);
	} 

	void saveTo(std::list<std::string>& s) {
		if(t != "") {
			s.push_back(t);
			count += t.length();
			locs.push_back(count);
			t = "";
		}
	} 
public:
	Lexer();
	/*
	 * use split to get raw input
	 * check illegal tokens and cerr message
	 */
	std::list<Token> GetTokens(const std::string& a);
};

#endif /* LEXER_HPP_ */

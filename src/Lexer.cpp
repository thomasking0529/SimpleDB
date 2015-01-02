/*
 * Lexer.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: Sun Jiacheng
 */

#include "Lexer.hpp"

bool isNum(const std::string& s) {
	if (s[0] >= '0' && s[0] <= '9') {
		return true;
	} else {
		return false;
	}
}

bool isId(const std::string& s) {
	return !isNum(s);
}

Lexer::Lexer() {
	count = 0;
	line_count = 0;
	symbols.insert('+');
	symbols.insert('-');
	symbols.insert('*');
	symbols.insert('/');
	symbols.insert(')');
	symbols.insert('(');
	symbols.insert(',');
	symbols.insert(';');
	symbols.insert('!');
	ops.insert("+");
	ops.insert("-");
	ops.insert("*");
	ops.insert("/");
	ops.insert("!");
	ops.insert("(");
	ops.insert(")");
	ops.insert(",");
	ops.insert("<");
	ops.insert("<>");
	ops.insert("<=");
	ops.insert(">");
	ops.insert(">=");
	ops.insert("=");
	ops.insert("==");
	ops.insert("&&");
	ops.insert("||");
	ops.insert(";");
}

/*EXAMPLE
 * select*from a__ where(1>0&&1<0||1<=0)
 * split:
 *    select
 *    *
 *    from
 *    a__
 *    where
 *    (
 *    1
 *    >
 *    0
 *    &&
 *    1
 *    <
 *    0
 *    ||
 *    1
 *    <=
 *    0
 *    )
 *
 * match keyword first:
 *    select from where, etc.
 *    to_upper
 *
 * match symbols:
 *    * ( > && < || <= )
 *
 * match numbers:
 *    [0-9]+
 *    illegal:
 *       01a
 * match id:
 *    [a-zA-Z_][a-z_0-9]*
 *    no illegal situation: all other symbols matched:
 *        a+/2b -> a + / 2b, 2b matched before
 */

void to_lower(char& a) {
	if (a >= 'A' && a <= 'Z') {
		a = a - 'A' + 'a';
	}
}

bool is_keyword(const std::string& s) {
	std::string t = s;
	std::for_each(t.begin(), t.end(), to_lower);
	if (t == "select" || t == "from" || t == "where" || t == "delete"
			|| t == "table" || t == "create" || t == "int" || t == "values"
			|| t == "primary" || t == "key" || t == "default" || t == "into"
			|| t == "insert") {
		return true;
	}
	return false;
}


std::list<std::string> Lexer::line_split(const std::string& s) {
	size_t last = 0;
	size_t index = s.find_first_of('\n', last);
	std::list<std::string> ret;
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of('\n', last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
	return ret;
}


std::list<Token> Lexer::GetTokens(const std::string& a) {
	line_count = 0;
	locs.clear();
	std::list<std::string> lines = line_split(a);
	std::list<Token> ret;
	for (auto& line : lines) {
		if (line == "\n") continue;
		count = 1;
		line_count++;
		std::list<std::string> tmp = split(line);
		int cc = 0;
		for (auto& t : tmp) {
			if (is_keyword(t)) {
				std::for_each(t.begin(), t.end(), to_lower);
				ret.push_back(Token(KEYWORD, t, locs[cc] - t.size(), line_count));
			}
			else if (ops.find(t) != ops.end()) {
				ret.push_back(Token(OP, t, locs[cc] - t.size(), line_count));
			}
			else if (t == " "){
			}
			else {
				//NUM
				if (t[0] >= '0' && t[0] <= '9') {
					for (int i = 1; i < t.size(); i++) {
						if (!(t[i] >= '0' && t[i] <= '9')) {
							throw(new SDBException("Illegal Tokens"));
						}
					}
					ret.push_back(Token(NUM, t, locs[cc] - t.size(), line_count));
					//ID
				}
				else if ((t[0] >= 'a' && t[0] <= 'z')
					|| (t[0] >= 'A' && t[0] <= 'Z') || t[0] == '_') {
					ret.push_back(Token(ID, t, locs[cc] - t.size(), line_count));
				}
			}
			cc++;
		}
	}
	return ret;
}

std::list<std::string> Lexer::split(const std::string& s) {
	std::list<std::string> ret;
	t = "";
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ' || s[i] == '\t') {
			saveTo(ret);
			count++;
		} else if (s[i] == '\n'){
			saveTo(ret);
			count++;
		} else if (symbols.find(s[i]) != symbols.end()) {
			saveTo(ret);
			saveTo(std::string("") + s[i], ret);
		} else if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')
				|| (s[i] >= '0' && s[i] <= '9')) {
			t += s[i];
			//< <= <>
		} else if (s[i] == '<') {
			saveTo(ret);
			if (s[i + 1] == '>') {
				saveTo("<>", ret);
				i++;
			} else if (s[i + 1] == '=') {
				saveTo("<=", ret);
				i++;
			} else {
				saveTo("<", ret);
			}
			//> >=
		} else if (s[i] == '>') {
			saveTo(ret);
			if (s[i + 1] == '=') {
				saveTo(">=", ret);
				i++;
			} else {
				saveTo(">", ret);
			}
			//= ==
			//= = will be interpreted as two "="
		} else if (s[i] == '=') {
			saveTo(ret);
			if (s[i + 1] == '=') {
				saveTo("==", ret);
				i++;
			} else {
				saveTo("=", ret);
			}
			//&& ||
		} else if (s[i] == '&' || s[i] == '|') {
			saveTo(ret);
			if (s[i + 1] == s[i]) {
				saveTo(std::string("") + s[i] + s[i], ret);
				i++;
			} else {
				std::stringstream ss;
				ss << "Illegal Token at col " << count << " detected.";
				throw(SDBException(ss.str()));
			}
		} else if (s[i] == '!') {
			saveTo(ret);
			saveTo(std::string("") + s[i], ret);
		} else {
			std::stringstream ss;
			ss << "Illegal Token at col " << count << " detected.";
			throw(SDBException(ss.str()));
		}
	}
	saveTo(ret);
	return ret;
}



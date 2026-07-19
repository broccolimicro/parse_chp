#pragma once

#include <parse/parse.h>
#include <parse/syntax.h>
#include "control.h"
#include "expression.h"

namespace parse_chp
{
struct branch;

struct composition : parse::syntax
{
	composition();
	composition(tokenizer &tokens, int level = 0, std::any data={});
	~composition();

	void init();

	vector<branch> branches;
	string region;
	int reset;
	int level;

	static vector<string> precedence;

	void parse(tokenizer &tokens, std::any data={});
	static bool is_next(tokenizer &tokens, int i = 1, std::any data={});
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	string to_string(int prev_level, string tab = "") const;
	parse::syntax *clone() const;
};

struct branch
{
	branch();
	branch(composition sub);
	branch(control ctrl);
	branch(assignment assign);
	~branch();

	composition sub;
	control ctrl;
	assignment assign;

	string to_string(int level, string tab) const;
};
}

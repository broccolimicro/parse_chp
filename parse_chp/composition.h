/*
 * composition.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "control.h"
#include <parse_expression/assignment.h>

#ifndef parse_chp_composition_h
#define parse_chp_composition_h

namespace parse_chp
{
using parse_expression::assignment;

struct branch;

struct composition : parse::syntax
{
	composition();
	composition(tokenizer &tokens, int level = 0, void *data = NULL);
	~composition();

	void init();

	vector<branch> branches;
	string region;
	int reset;
	int level;

	static vector<string> precedence;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
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

#endif

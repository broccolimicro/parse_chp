/*
 * control.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_expression/expression.h>

#ifndef parse_chp_control_h
#define parse_chp_control_h

namespace parse_chp
{
using parse_expression::expression;

struct composition;

struct control : parse::syntax
{
	control();
	control(tokenizer &tokens, void *data = NULL);
	~control();

	bool deterministic;
	bool repeat;
	vector<pair<expression, composition> > branches;
	string region;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif

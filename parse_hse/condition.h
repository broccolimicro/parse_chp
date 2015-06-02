/*
 * condition.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_boolean/guard.h>
#include "parallel.h"

#ifndef parse_hse_condition_h
#define parse_hse_condition_h

namespace parse_hse
{
struct condition : parse::syntax
{
	condition();
	condition(tokenizer &tokens, void *data = NULL);
	~condition();

	bool deterministic;
	vector<pair<parse_boolean::guard, parallel> > branches;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;
};
}

#endif

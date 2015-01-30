/*
 * condition.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "guard.h"
#include "sequence.h"

#ifndef condition_h
#define condition_h

namespace hse
{
struct condition : parse::syntax
{
	condition();
	condition(configuration &config, tokenizer &tokens);
	~condition();

	bool deterministic;
	vector<pair<guard, sequence> > branches;

	void parse(configuration &config, tokenizer &tokens);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "");
};
}

#endif

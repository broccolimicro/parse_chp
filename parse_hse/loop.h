/*
 * loop.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <parse_boolean/guard.h>
#include "sequence.h"

#ifndef parse_hse_loop_h
#define parse_hse_loop_h

namespace parse_hse
{
struct loop : parse::syntax
{
	loop();
	loop(configuration &config, tokenizer &tokens);
	~loop();

	bool deterministic;
	vector<pair<parse_boolean::guard, sequence> > branches;

	void parse(configuration &config, tokenizer &tokens);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif

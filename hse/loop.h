/*
 * loop.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include "guard.h"
#include "sequence.h"

#ifndef loop_h
#define loop_h

namespace hse
{
struct loop : parse::syntax
{
	loop();
	loop(configuration &config, tokenizer &tokens);
	~loop();

	bool deterministic;
	vector<pair<guard, sequence> > branches;

	void parse(configuration &config, tokenizer &tokens);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "");
};
}

#endif
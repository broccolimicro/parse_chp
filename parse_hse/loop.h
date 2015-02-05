/*
 * loop.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>
#include <prs_lang/guard.h>
#include "sequence.h"

#ifndef hse_lang_loop_h
#define hse_lang_loop_h

namespace hse_lang
{
struct loop : parse::syntax
{
	loop();
	loop(configuration &config, tokenizer &tokens);
	~loop();

	bool deterministic;
	vector<pair<prs_lang::guard, sequence> > branches;

	void parse(configuration &config, tokenizer &tokens);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "");
};
}

#endif

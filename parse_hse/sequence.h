/*
 * sequence.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>

#ifndef parse_hse_sequence_h
#define parse_hse_sequence_h

namespace parse_hse
{
struct sequence : parse::syntax
{
	sequence();
	sequence(configuration &config, tokenizer &tokens);
	~sequence();

	vector<parse::syntax*> actions;

	void parse(configuration &config, tokenizer &tokens);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif

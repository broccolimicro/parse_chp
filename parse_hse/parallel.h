/*
 * parallel.h
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse/syntax.h>

#ifndef parse_hse_parallel_h
#define parse_hse_parallel_h

namespace parse_hse
{
struct parallel : parse::syntax
{
	parallel();
	parallel(configuration &config, tokenizer &tokens, int i = 0);
	~parallel();

	vector<parse::syntax*> branches;
	int level;

	void parse(configuration &config, tokenizer &tokens, int i = 0);
	static bool is_next(configuration &config, tokenizer &tokens, int i = 1);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif

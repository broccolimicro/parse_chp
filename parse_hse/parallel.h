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
	parallel(tokenizer &tokens, int i = 0, void *data = NULL);
	~parallel();

	vector<parse::syntax*> branches;
	int level;

	void parse(tokenizer &tokens, int i = 0, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
};
}

#endif

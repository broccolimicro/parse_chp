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
	sequence(tokenizer &tokens, void *data = NULL);
	sequence(const sequence &s);
	~sequence();

	vector<parse::syntax*> actions;
	int reset;
	string region;

	void parse(tokenizer &tokens, void *data = NULL);
	static bool is_next(tokenizer &tokens, int i = 1, void *data = NULL);
	static void register_syntax(tokenizer &tokens);

	string to_string(string tab = "") const;
	parse::syntax *clone() const;

	sequence &operator=(const sequence &copy);
	void clear();
};
}

#endif

/*
 * parallel.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "parallel.h"
#include "sequence.h"
#include "condition.h"
#include "loop.h"

#include <parse_boolean/assignment.h>
#include <parse/default/symbol.h>

namespace parse_hse
{
parallel::parallel()
{
	debug_name = "parallel";
}

parallel::parallel(tokenizer &tokens, void *data)
{
	debug_name = "parallel";
	parse(tokens, data);
}

parallel::~parallel()
{

}

void parallel::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect("||");

	tokens.increment(true);
	tokens.expect<sequence>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		branches.push_back(sequence(tokens, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect("||");

		tokens.increment(true);
		tokens.expect<sequence>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(sequence(tokens, data));
	}

	tokens.syntax_end(this);
}

bool parallel::is_next(tokenizer &tokens, int i, void *data)
{
	return sequence::is_next(tokens, i, data);
}

void parallel::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<parallel>())
	{
		tokens.register_syntax<parallel>();
		tokens.register_token<parse::symbol>();
		sequence::register_syntax(tokens);
	}
}

string parallel::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0)
			result += "||";

		if (branches[i].valid)
			result += branches[i].to_string(tab);
		else
			result += "null";
	}
	return result;
}

parse::syntax *parallel::clone() const
{
	return new parallel(*this);
}

}

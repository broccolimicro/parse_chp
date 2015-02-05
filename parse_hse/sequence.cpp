/*
 * sequence.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "sequence.h"
#include "condition.h"
#include "loop.h"
#include "parallel.h"

#include <prs_lang/assignment.h>
#include <parse/default/symbol.h>

namespace hse_lang
{
sequence::sequence()
{
	debug_name = "sequence";
}

sequence::sequence(configuration &config, tokenizer &tokens)
{
	debug_name = "sequence";
	parse(config, tokens);
}

sequence::~sequence()
{

}

void sequence::parse(configuration &config, tokenizer &tokens)
{
	valid = true;

	tokens.increment(false);
	tokens.expect(";");

	tokens.increment(true);
	tokens.expect<parallel>();

	if (tokens.decrement(config, __FILE__, __LINE__))
		actions.push_back(new parallel(config, tokens, 1));

	while (tokens.decrement(config, __FILE__, __LINE__))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(";");

		tokens.increment(true);
		tokens.expect<parallel>();

		if (tokens.decrement(config, __FILE__, __LINE__))
			actions.push_back(new parallel(config, tokens, 1));
	}
}

bool sequence::is_next(configuration &config, tokenizer &tokens, int i)
{
	return parallel::is_next(config, tokens, i);
}

void sequence::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<sequence>())
	{
		tokens.register_syntax<sequence>();
		tokens.register_token<parse::symbol>();
		parallel::register_syntax(tokens);
	}
}

string sequence::to_string(string tab)
{
	string result = "";
	for (int i = 0; i < (int)actions.size(); i++)
	{
		if (actions[i] != NULL)
		{
			if (i != 0)
				result += "; ";

			result += actions[i]->to_string(tab);
		}
	}
	return result;
}
}

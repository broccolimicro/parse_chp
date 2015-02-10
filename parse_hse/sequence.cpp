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

#include <parse_boolean/assignment.h>
#include <parse/default/symbol.h>

namespace parse_hse
{
sequence::sequence()
{
	debug_name = "sequence";
}

sequence::sequence(tokenizer &tokens, void *data)
{
	debug_name = "sequence";
	parse(tokens, data);
}

sequence::~sequence()
{

}

void sequence::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(";");

	tokens.increment(true);
	tokens.expect<parallel>();

	if (tokens.decrement(__FILE__, __LINE__, data))
		actions.push_back(new parallel(tokens, 1, data));

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(";");

		tokens.increment(true);
		tokens.expect<parallel>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			actions.push_back(new parallel(tokens, 1, data));
	}

	tokens.syntax_end(this);
}

bool sequence::is_next(tokenizer &tokens, int i, void *data)
{
	return parallel::is_next(tokens, i, data);
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

string sequence::to_string(string tab) const
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

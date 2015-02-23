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

#include <parse_boolean/internal_parallel.h>
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
	tokens.expect<parse_boolean::internal_parallel>();
	tokens.expect<condition>();
	tokens.expect<loop>();
	tokens.expect("(");

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found("("))
		{
			tokens.next();
			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<parallel>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				actions.push_back(new parallel(tokens, data));

			if (tokens.decrement(__FILE__, __LINE__, data))
				tokens.next();
		}
		else if (tokens.found<condition>())
			actions.push_back(new condition(tokens, data));
		else if (tokens.found<loop>())
			actions.push_back(new loop(tokens, data));
		else if (tokens.found<parse_boolean::internal_parallel>())
			actions.push_back(new parse_boolean::internal_parallel(tokens, data));
	}

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(";");

		tokens.increment(true);
		tokens.expect<parse_boolean::internal_parallel>();
		tokens.expect<condition>();
		tokens.expect<loop>();
		tokens.expect("(");

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (tokens.found("("))
			{
				tokens.next();
				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<parallel>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					actions.push_back(new parallel(tokens, data));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();
			}
			else if (tokens.found<condition>())
				actions.push_back(new condition(tokens, data));
			else if (tokens.found<loop>())
				actions.push_back(new loop(tokens, data));
			else if (tokens.found<parse_boolean::internal_parallel>())
				actions.push_back(new parse_boolean::internal_parallel(tokens, data));
		}
	}

	tokens.syntax_end(this);
}

bool sequence::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("(", i) || condition::is_next(tokens, i, data) || loop::is_next(tokens, i, data) || parse_boolean::internal_parallel::is_next(tokens, i, data);
}

void sequence::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<sequence>())
	{
		tokens.register_syntax<sequence>();
		tokens.register_token<parse::symbol>();
		condition::register_syntax(tokens);
		loop::register_syntax(tokens);
		parallel::register_syntax(tokens);
		parse_boolean::internal_parallel::register_syntax(tokens);
	}
}

string sequence::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)actions.size(); i++)
	{
		if (i != 0)
			result += ";";

		if (actions[i] != NULL && actions[i]->valid)
			result += actions[i]->to_string(tab);
		else
			result += "null";
	}
	return result;
}

parse::syntax *sequence::clone() const
{
	return new sequence(*this);
}

}

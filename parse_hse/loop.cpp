/*
 * loop.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "loop.h"

#include <parse/default/symbol.h>

namespace parse_hse
{
loop::loop()
{
	debug_name = "loop";
	deterministic = true;
}

loop::loop(configuration &config, tokenizer &tokens)
{
	debug_name = "loop";
	deterministic = true;
	parse(config, tokens);
}

loop::~loop()
{

}

void loop::parse(configuration &config, tokenizer &tokens)
{
	valid = true;

	bool locked = false;
	bool infinite = false;

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect("*[");

	while (!infinite && tokens.decrement(config, __FILE__, __LINE__))
	{
		if (tokens.found("[]"))
		{
			deterministic = true;
			locked = true;
		}
		else if (tokens.found("|"))
		{
			deterministic = false;
			locked = true;
		}
		else if (tokens.found("*["))
		{
			locked = false;
			infinite = true;
		}

		tokens.next();

		tokens.increment(true);
		tokens.expect<sequence>();

		tokens.increment(branches.size() > 0);
		tokens.expect<parse_boolean::guard>();

		if (tokens.decrement(config, __FILE__, __LINE__))
		{
			branches.push_back(pair<parse_boolean::guard, sequence>(parse_boolean::guard(config, tokens), sequence()));

			tokens.increment(true);
			tokens.expect("->");

			if (tokens.decrement(config, __FILE__, __LINE__))
				tokens.next();

			infinite = false;
		}
		else
			branches.push_back(pair<parse_boolean::guard, sequence>(parse_boolean::guard(), sequence()));

		if (tokens.decrement(config, __FILE__, __LINE__))
			branches.back().second.parse(config, tokens);

		if (!infinite)
		{
			tokens.increment(false);
			if (deterministic || !locked)
				tokens.expect("[]");
			if (!deterministic || !locked)
				tokens.expect("|");
		}
	}

	if (tokens.decrement(config, __FILE__, __LINE__))
		tokens.next();
}

bool loop::is_next(configuration &config, tokenizer &tokens, int i)
{
	return tokens.is_next("*[", i);
}

void loop::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<loop>())
	{
		tokens.register_syntax<loop>();
		tokens.register_token<parse::symbol>();
		parse_boolean::guard::register_syntax(tokens);
		sequence::register_syntax(tokens);
	}
}

string loop::to_string(string tab) const
{
	string result = "*[";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0 && deterministic)
			result += "[]";
		else if (i != 0 && !deterministic)
			result += "|";

		if (branches[i].first.valid)
			result += branches[i].first.to_string(tab) + "->";
		result += branches[i].second.to_string(tab);
	}
	result += "]";

	return result;
}
}

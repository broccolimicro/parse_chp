/*
 * condition.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "condition.h"
#include <parse/default/symbol.h>

namespace parse_hse
{
condition::condition()
{
	debug_name = "condition";
	deterministic = true;
}

condition::condition(tokenizer &tokens, void *data)
{
	debug_name = "condition";
	deterministic = true;
	parse(tokens, data);
}

condition::~condition()
{

}

void condition::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	bool locked = false;
	bool wait = false;

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect("[");

	while (!wait && tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found("[]"))
		{
			deterministic = true;
			locked = true;
		}
		else if (tokens.found(":"))
		{
			deterministic = false;
			locked = true;
		}
		else if (tokens.found("["))
		{
			locked = false;
			wait = true;
		}

		tokens.next();

		tokens.increment(false);
		tokens.expect("->");

		tokens.increment(true);
		tokens.expect<parse_boolean::guard>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(pair<parse_boolean::guard, parallel>(parse_boolean::guard(tokens, parse_boolean::guard::OR, data), parallel()));

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			tokens.next();

			tokens.increment(true);
			tokens.expect<parallel>();

			if (tokens.decrement(__FILE__, __LINE__, data) && branches.size() > 0 && !branches.back().second.valid)
				branches.back().second.parse(tokens, data);

			tokens.increment(false);
			if (deterministic || !locked)
				tokens.expect("[]");
			if (!deterministic || !locked)
				tokens.expect(":");

			wait = false;
		}
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
		tokens.next();

	tokens.syntax_end(this);
}

bool condition::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("[", i);
}

void condition::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<condition>())
	{
		tokens.register_syntax<condition>();
		parse_boolean::guard::register_syntax(tokens);
		parallel::register_syntax(tokens);
		tokens.register_token<parse::symbol>();
	}
}

string condition::to_string(string tab) const
{
	string result = "[";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0 && deterministic)
			result += "[]";
		else if (i != 0 && !deterministic)
			result += ":";

		result += branches[i].first.to_string(tab);
		if (branches[i].second.valid)
			result += "->" + branches[i].second.to_string(tab);
	}
	result += "]";

	return result;
}

parse::syntax *condition::clone() const
{
	return new condition(*this);
}
}

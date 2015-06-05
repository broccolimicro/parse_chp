/*
 * loop.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "loop.h"

#include <parse/default/symbol.h>
#include <parse/default/number.h>

namespace parse_hse
{
loop::loop()
{
	debug_name = "loop";
	deterministic = true;
}

loop::loop(tokenizer &tokens, void *data)
{
	debug_name = "loop";
	deterministic = true;
	parse(tokens, data);
}

loop::~loop()
{

}

void loop::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	deterministic = false;
	bool infinite = true;
	bool locked = false;
	bool done = false;

	tokens.increment(false);
	tokens.expect("'");

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect("*[");

	if (tokens.decrement(__FILE__, __LINE__, data))
		tokens.next();

	tokens.push();
	int level = 0;
	while (level >= 0 && infinite)
	{
		string token = tokens.next();
		if (token == "[" || token == "*[")
			level++;
		else if (token == "]")
			level--;
		else if (level == 0 && token == "->")
			infinite = false;
	}
	tokens.pop();

	if (infinite)
	{
		tokens.increment(true);
		tokens.expect<parallel>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(pair<parse_boolean::guard, parallel>(parse_boolean::guard(), parallel(tokens, data)));
	}
	else do
	{
		tokens.increment(false);
		if (!locked || deterministic)
			tokens.expect("[]");
		if (!locked || !deterministic)
			tokens.expect(":");

		tokens.increment(true);
		tokens.expect<parallel>();

		tokens.increment(true);
		tokens.expect("->");

		tokens.increment(true);
		tokens.expect<parse_boolean::guard>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(pair<parse_boolean::guard, parallel>(parse_boolean::guard(tokens, parse_boolean::guard::OR, data), parallel()));
		else
			branches.push_back(pair<parse_boolean::guard, parallel>(parse_boolean::guard(), parallel()));

		if (tokens.decrement(__FILE__, __LINE__, data))
			tokens.next();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.back().second.parse(tokens, data);

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			tokens.next();
			locked = true;
			if (tokens.found("[]"))
				deterministic = true;
			else if (tokens.found(":"))
				deterministic = false;
		}
		else
			done = true;
	} while (!done);

	if (tokens.decrement(__FILE__, __LINE__, data))
		tokens.next();

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(true);
		tokens.expect<parse::number>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			region = tokens.next();
	}

	tokens.syntax_end(this);
}

bool loop::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("*[", i);
}

void loop::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<loop>())
	{
		tokens.register_syntax<loop>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		parse_boolean::guard::register_syntax(tokens);
		parallel::register_syntax(tokens);
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
			result += ":";

		if (branches[i].first.valid)
			result += branches[i].first.to_string(tab) + "->";
		result += branches[i].second.to_string(tab);
	}
	result += "]";

	if (region != "")
		result += "'" + region;

	return result;
}

parse::syntax *loop::clone() const
{
	return new loop(*this);
}

}

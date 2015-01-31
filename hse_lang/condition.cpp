/*
 * condition.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "condition.h"

#include <parse/default/symbol.h>

namespace hse_lang
{
condition::condition()
{
	debug_name = "condition";
	deterministic = true;
}

condition::condition(configuration &config, tokenizer &tokens)
{
	debug_name = "condition";
	deterministic = true;
	parse(config, tokens);
}

condition::~condition()
{

}

void condition::parse(configuration &config, tokenizer &tokens)
{
	valid = true;

	bool locked = false;
	bool wait = false;

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect("[");

	while (!wait && tokens.decrement(config, __FILE__, __LINE__))
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
		else if (tokens.found("["))
		{
			locked = false;
			wait = true;
		}

		tokens.next();

		tokens.increment(false);
		tokens.expect("->");

		tokens.increment(true);
		tokens.expect<prs_lang::guard>();

		if (tokens.decrement(config, __FILE__, __LINE__))
			branches.push_back(pair<prs_lang::guard, sequence>(prs_lang::guard(config, tokens), sequence()));

		if (tokens.decrement(config, __FILE__, __LINE__))
		{
			tokens.next();

			tokens.increment(true);
			tokens.expect<sequence>();

			if (tokens.decrement(config, __FILE__, __LINE__) && branches.size() > 0 && !branches.back().second.valid)
				branches.back().second.parse(config, tokens);

			tokens.increment(false);
			if (deterministic || !locked)
				tokens.expect("[]");
			if (!deterministic || !locked)
				tokens.expect("|");

			wait = false;
		}
	}

	if (tokens.decrement(config, __FILE__, __LINE__))
		tokens.next();
}

bool condition::is_next(configuration &config, tokenizer &tokens, int i)
{
	return tokens.is_next("[", i);
}

void condition::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<condition>())
	{
		tokens.register_syntax<condition>();
		prs_lang::guard::register_syntax(tokens);
		sequence::register_syntax(tokens);
		tokens.register_token<parse::symbol>();
	}
}

string condition::to_string(string tab)
{
	string result = "[";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0 && deterministic)
			result += "[]";
		else if (i != 0 && !deterministic)
			result += "|";

		result += branches[i].first.to_string(tab);
		if (branches[i].second.valid)
			result += "->" + branches[i].second.to_string(tab);
	}
	result += "]";

	return result;
}
}

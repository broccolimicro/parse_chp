/*
 * control.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "control.h"
#include "composition.h"
#include <parse/default/symbol.h>
#include <parse/default/number.h>

namespace parse_chp
{
control::control()
{
	debug_name = "control";
	deterministic = true;
	repeat = false;
}

control::control(tokenizer &tokens, void *data)
{
	debug_name = "control";
	deterministic = true;
	repeat = false;
	parse(tokens, data);
}

control::~control()
{

}

void control::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	bool locked = false;
	bool shortcut = true;

	tokens.increment(false);
	tokens.expect("'");

	tokens.increment(true);
	tokens.expect("]");

	tokens.increment(true);
	tokens.expect("[");
	tokens.expect("*[");

	if (tokens.decrement(__FILE__, __LINE__, data))
		if (tokens.next() == "*[")
			repeat = true;

	tokens.push();
	int level = 0;
	while (level >= 0 && shortcut)
	{
		string token = tokens.next();
		if (token == "[" || token == "*[")
			level++;
		else if (token == "]")
			level--;
		else if (level == 0 && token == "->")
			shortcut = false;
	}
	tokens.pop();

	bool first = true;
	if (shortcut)
	{
		tokens.increment(true);
		if (repeat)
			tokens.expect<composition>();
		else
			tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (tokens.found<composition>())
				branches.push_back(pair<expression, composition>(expression(), composition(tokens, 0, data)));
			else if (tokens.found<expression>())
				branches.push_back(pair<expression, composition>(expression(tokens, 0, data), composition()));
		}
	}
	else do
	{
		if (first)
			first = false;
		else
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
			tokens.next();
		}

		tokens.increment(false);
		if (deterministic || !locked)
			tokens.expect("[]");
		if (!deterministic || !locked)
			tokens.expect(":");

		tokens.increment(true);
		tokens.expect<composition>();

		tokens.increment(true);
		tokens.expect("->");

		tokens.increment(true);
		tokens.expect<expression>();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.push_back(pair<expression, composition>(expression(tokens, 0, data), composition()));

		if (tokens.decrement(__FILE__, __LINE__, data))
			tokens.next();

		if (tokens.decrement(__FILE__, __LINE__, data))
			branches.back().second.parse(tokens, data);
	} while (tokens.decrement(__FILE__, __LINE__, data));

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

bool control::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("*[", i) || tokens.is_next("[", i);
}

void control::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<control>())
	{
		tokens.register_syntax<control>();
		expression::register_syntax(tokens);
		composition::register_syntax(tokens);
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
	}
}

string control::to_string(string tab) const
{
	if (!valid || branches.size() == 0)
		return "skip";

	string result = "";
	if (repeat)
		result += "*";

	result += "[";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (i != 0 && deterministic)
			result += "[]";
		else if (i != 0 && !deterministic)
			result += ":";

		if (branches[i].first.valid)
			result += branches[i].first.to_string(tab);

		if (branches[i].first.valid && branches[i].second.valid)
			result += "->";

		if (branches[i].second.valid)
			result += branches[i].second.to_string(tab);
	}
	result += "]";

	if (region != "")
		result += "'" + region;

	return result;
}

parse::syntax *control::clone() const
{
	return new control(*this);
}
}

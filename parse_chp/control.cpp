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
#include <parse/default/white_space.h>
#include <parse/default/new_line.h>

namespace parse_chp
{
control::control()
{
	debug_name = "chp_control";
	deterministic = true;
	stable = true;
	repeat = false;
	assume = false;
}

control::control(tokenizer &tokens, void *data)
{
	debug_name = "chp_control";
	deterministic = true;
	stable = true;
	repeat = false;
	assume = false;
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
	tokens.expect("[");
	tokens.expect("*[");
	tokens.expect("{");

	if (tokens.decrement(__FILE__, __LINE__, data)) {
		string tok = tokens.next();
		repeat = (tok == "*[");
		assume = (tok == "{");

		if (tok == "{") {
			tokens.increment(true);
			tokens.expect("}");
		} else {
			tokens.increment(true);
			tokens.expect("]");
		}
	}

	if (not assume) {
		tokens.push();
		int level = 0;
		while (level >= 0 and shortcut) {
			string token = tokens.next();
			if (token == "[" or token == "*[")
				level++;
			else if (token == "]")
				level--;
			else if (level == 0 && token == "->")
				shortcut = false;
		}
		tokens.pop();
	}

	bool first = true;
	if (shortcut) {
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
	} else do {
		if (first)
			first = false;
		else {
			if (tokens.found("[]")) {
				deterministic = true;
				locked = true;
			} else if (tokens.found(":")) {
				deterministic = false;
				locked = true;
			} else if (tokens.found("::")) {
				deterministic = false;
				stable = false;
				locked = true;
			}
			tokens.next();
		}

		tokens.increment(false);
		if (deterministic or not locked)
			tokens.expect("[]");
		if ((not deterministic and stable) or not locked)
			tokens.expect(":");
		if ((not deterministic and not stable) or not locked)
			tokens.expect("::");

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
	return tokens.is_next("*[", i) or tokens.is_next("[", i) or tokens.is_next("{", i);
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
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(false);
	}
}

string control::to_string(string tab) const
{
	if (!valid || branches.size() == 0)
		return "skip";

	string result = "";
	if (repeat)
		result += "*";

	result += assume ? "{" : "[";
	for (int i = 0; i < (int)branches.size(); i++) {
		if (i != 0 and deterministic)
			result += "[]";
		else if (i != 0 and not deterministic and stable)
			result += ":";
		else if (i != 0 and not deterministic and not stable)
			result += "::";

		if (branches[i].first.valid)
			result += branches[i].first.to_string(tab);

		if (branches[i].first.valid and branches[i].second.valid)
			result += "->";

		if (branches[i].second.valid)
			result += branches[i].second.to_string(tab);
	}
	result += assume ? "}" : "]";

	if (region != "")
		result += "'" + region;

	return result;
}

parse::syntax *control::clone() const
{
	return new control(*this);
}
}

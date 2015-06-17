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
#include <parse/default/number.h>

namespace parse_hse
{
sequence::sequence()
{
	debug_name = "sequence";
	reset = -1;
}

sequence::sequence(tokenizer &tokens, void *data)
{
	debug_name = "sequence";
	reset = -1;
	parse(tokens, data);
}

sequence::sequence(const sequence &copy) : parse::syntax(copy)
{
	for (int i = 0; i < (int)copy.actions.size(); i++)
		actions.push_back(copy.actions[i]->clone());
	reset = copy.reset;
}

sequence::~sequence()
{
	clear();
}

void sequence::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	tokens.increment(false);
	tokens.expect(";");

	tokens.increment(false);
	tokens.expect("@");

	tokens.increment(true);
	tokens.expect<parse_boolean::assignment>();
	tokens.expect<condition>();
	tokens.expect<loop>();
	tokens.expect("(");
	tokens.expect("skip");

	tokens.increment(false);
	tokens.expect("@");

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();
		reset = 0;
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found("("))
		{
			tokens.next();

			tokens.increment(false);
			tokens.expect("'");

			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<parallel>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				actions.push_back(new parallel(tokens, data));

			if (tokens.decrement(__FILE__, __LINE__, data))
				tokens.next();

			if (tokens.decrement(__FILE__, __LINE__, data))
			{
				tokens.next();

				tokens.increment(true);
				tokens.expect<parse::number>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					((parallel*)actions.back())->region = tokens.next();
			}
		}
		else if (tokens.found("skip"))
			tokens.next();
		else if (tokens.found<condition>())
			actions.push_back(new condition(tokens, data));
		else if (tokens.found<loop>())
			actions.push_back(new loop(tokens, data));
		else if (tokens.found<parse_boolean::assignment>())
			actions.push_back(new parse_boolean::assignment(tokens, parse_boolean::assignment::CHOICE, data));
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();
		if (reset >= 0)
			tokens.error("only one reset token allowed per sequential", __FILE__, __LINE__);
		reset = 1;
	}

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(";");

		tokens.increment(false);
		tokens.expect("@");

		tokens.increment(true);
		tokens.expect<parse_boolean::assignment>();
		tokens.expect<condition>();
		tokens.expect<loop>();
		tokens.expect("(");
		tokens.expect("skip");

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (tokens.found("("))
			{
				tokens.next();

				tokens.increment(false);
				tokens.expect("'");

				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<parallel>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					actions.push_back(new parallel(tokens, data));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();

				if (tokens.decrement(__FILE__, __LINE__, data))
				{
					tokens.next();

					tokens.increment(true);
					tokens.expect<parse::number>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						((parallel*)actions.back())->region = tokens.next();
				}
			}
			if (tokens.found("skip"))
				tokens.next();
			else if (tokens.found<condition>())
				actions.push_back(new condition(tokens, data));
			else if (tokens.found<loop>())
				actions.push_back(new loop(tokens, data));
			else if (tokens.found<parse_boolean::assignment>())
				actions.push_back(new parse_boolean::assignment(tokens, parse_boolean::assignment::CHOICE, data));
		}

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			tokens.next();
			if (reset >= 0)
				tokens.error("only one reset token allowed per sequential", __FILE__, __LINE__);
			reset = (int)actions.size();
		}
	}

	tokens.syntax_end(this);
}

bool sequence::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("@", i) || tokens.is_next("skip", i) || tokens.is_next("(", i) || condition::is_next(tokens, i, data) || loop::is_next(tokens, i, data) || parse_boolean::assignment::is_next(tokens, i, data);
}

void sequence::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<sequence>())
	{
		tokens.register_syntax<sequence>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		condition::register_syntax(tokens);
		loop::register_syntax(tokens);
		parallel::register_syntax(tokens);
		parse_boolean::assignment::register_syntax(tokens);
	}
}

string sequence::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)actions.size(); i++)
	{
		if (reset == i)
			result += "@";

		if (i != 0)
			result += ";";

		bool paren = false;
		string id = "";
		if (actions[i]->is_a<parallel>())
		{
			id = ((parallel*)actions[i])->region;
			paren = ((parallel*)actions[i])->branches.size() > 1 || id != "";
		}

		if (paren)
			result += "(";
		if (actions[i] != NULL && actions[i]->valid)
			result += actions[i]->to_string(tab);
		else
			result += "null";
		if (paren)
			result += ")";

		if (id != "")
			result += "'" + id;
	}

	if (reset == (int)actions.size())
		result += "@";

	if (actions.size() == 0)
		result += "skip";

	return result;
}

parse::syntax *sequence::clone() const
{
	return new sequence(*this);
}

sequence &sequence::operator=(const sequence &copy)
{
	parse::syntax::operator=(copy);

	for (int i = 0; i < (int)actions.size(); i++)
		if (actions[i] != NULL)
			delete actions[i];
	actions.clear();

	for (int i = 0; i < (int)copy.actions.size(); i++)
		actions.push_back(copy.actions[i]->clone());

	reset = copy.reset;

	return *this;
}

void sequence::clear()
{
	for (int i = 0; i < (int)actions.size(); i++)
		if (actions[i] != NULL)
			delete actions[i];
	actions.clear();
	reset = -1;
}

}

/*
 * parallel.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "parallel.h"
#include "sequence.h"
#include "condition.h"
#include "loop.h"

#include <parse_boolean/assignment.h>
#include <parse/default/symbol.h>

namespace parse_hse
{
parallel::parallel()
{
	debug_name = "parallel";
	level = -1;
}

parallel::parallel(tokenizer &tokens, int i, void *data)
{
	debug_name = "parallel";
	level = -1;
	parse(tokens, i, data);
}

parallel::~parallel()
{

}

void parallel::parse(tokenizer &tokens, int i, void *data)
{
	tokens.syntax_start(this);

	level = i;

	vector<string> ops;
	ops.push_back("||");
	ops.push_back(",");

	tokens.increment(false);
	tokens.expect(ops[i]);

	tokens.increment(true);
	if (level == 0)
		tokens.expect<sequence>();
	else
	{
		tokens.expect<parse_boolean::assignment>();
		tokens.expect<condition>();
		tokens.expect<loop>();
		tokens.expect("(");
	}

	if (tokens.decrement(__FILE__, __LINE__, data))
	{
		if (tokens.found<sequence>())
			branches.push_back(new sequence(tokens, data));
		else if (tokens.found<parse_boolean::assignment>())
			branches.push_back(new parse_boolean::assignment(tokens, data));
		else if (tokens.found<condition>())
			branches.push_back(new condition(tokens, data));
		else if (tokens.found<loop>())
			branches.push_back(new loop(tokens, data));
		else if (tokens.found("("))
		{
			tokens.next();
			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<parallel>();

			if (tokens.decrement(__FILE__, __LINE__, data))
				branches.push_back(new parallel(tokens, 0, data));

			if (tokens.decrement(__FILE__, __LINE__, data))
				tokens.next();
		}
	}

	while (tokens.decrement(__FILE__, __LINE__, data))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(ops[i]);

		tokens.increment(true);
		if (level == 0)
			tokens.expect<sequence>();
		else
		{
			tokens.expect<parse_boolean::assignment>();
			tokens.expect<condition>();
			tokens.expect<loop>();
			tokens.expect("(");
		}

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (tokens.found<sequence>())
				branches.push_back(new sequence(tokens, data));
			else if (tokens.found<parse_boolean::assignment>())
				branches.push_back(new parse_boolean::assignment(tokens, data));
			else if (tokens.found<condition>())
				branches.push_back(new condition(tokens, data));
			else if (tokens.found<loop>())
				branches.push_back(new loop(tokens, data));
			else if (tokens.found("("))
			{
				tokens.next();
				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<parallel>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					branches.push_back(new parallel(tokens, 0, data));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();
			}
		}
	}

	tokens.syntax_end(this);
}

bool parallel::is_next(tokenizer &tokens, int i, void *data)
{
	return (parse_boolean::assignment::is_next(tokens, i, data) ||
		    loop::is_next(tokens, i, data) ||
		    condition::is_next(tokens, i, data) ||
		    tokens.is_next("(", i));
}

void parallel::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<parallel>())
	{
		tokens.register_syntax<parallel>();
		tokens.register_token<parse::symbol>();
		sequence::register_syntax(tokens);
		parse_boolean::assignment::register_syntax(tokens);
		loop::register_syntax(tokens);
		condition::register_syntax(tokens);
	}
}

string parallel::to_string(string tab) const
{
	string result = "";
	for (int i = 0; i < (int)branches.size(); i++)
	{
		if (branches[i] != NULL)
		{
			if (i != 0 && level == 0)
				result += "||";
			else if (i != 0 && level == 1)
				result += ",";

			result += branches[i]->to_string(tab);
		}
	}
	return result;
}
}

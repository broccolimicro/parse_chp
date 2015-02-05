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

#include <prs_lang/assignment.h>
#include <parse/default/symbol.h>

namespace hse_lang
{
parallel::parallel()
{
	debug_name = "parallel";
	level = -1;
}

parallel::parallel(configuration &config, tokenizer &tokens, int i)
{
	debug_name = "parallel";
	level = -1;
	parse(config, tokens, i);
}

parallel::~parallel()
{

}

void parallel::parse(configuration &config, tokenizer &tokens, int i)
{
	valid = true;
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
		tokens.expect<prs_lang::assignment>();
		tokens.expect<condition>();
		tokens.expect<loop>();
		tokens.expect("(");
	}

	if (tokens.decrement(config, __FILE__, __LINE__))
	{
		if (tokens.found<sequence>())
			branches.push_back(new sequence(config, tokens));
		else if (tokens.found<prs_lang::assignment>())
			branches.push_back(new prs_lang::assignment(config, tokens));
		else if (tokens.found<condition>())
			branches.push_back(new condition(config, tokens));
		else if (tokens.found<loop>())
			branches.push_back(new loop(config, tokens));
		else if (tokens.found("("))
		{
			tokens.next();
			tokens.increment(true);
			tokens.expect(")");

			tokens.increment(true);
			tokens.expect<parallel>();

			if (tokens.decrement(config, __FILE__, __LINE__))
				branches.push_back(new parallel(config, tokens));

			if (tokens.decrement(config, __FILE__, __LINE__))
				tokens.next();
		}
	}

	while (tokens.decrement(config, __FILE__, __LINE__))
	{
		tokens.next();

		tokens.increment(false);
		tokens.expect(ops[i]);

		tokens.increment(true);
		if (level == 0)
			tokens.expect<sequence>();
		else
		{
			tokens.expect<prs_lang::assignment>();
			tokens.expect<condition>();
			tokens.expect<loop>();
			tokens.expect("(");
		}

		if (tokens.decrement(config, __FILE__, __LINE__))
		{
			if (tokens.found<sequence>())
				branches.push_back(new sequence(config, tokens));
			else if (tokens.found<prs_lang::assignment>())
				branches.push_back(new prs_lang::assignment(config, tokens));
			else if (tokens.found<condition>())
				branches.push_back(new condition(config, tokens));
			else if (tokens.found<loop>())
				branches.push_back(new loop(config, tokens));
			else if (tokens.found("("))
			{
				tokens.next();
				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<parallel>();

				if (tokens.decrement(config, __FILE__, __LINE__))
					branches.push_back(new parallel(config, tokens));

				if (tokens.decrement(config, __FILE__, __LINE__))
					tokens.next();
			}
		}
	}
}

bool parallel::is_next(configuration &config, tokenizer &tokens, int i)
{
	return (prs_lang::assignment::is_next(config, tokens, i) ||
		    loop::is_next(config, tokens, i) ||
		    condition::is_next(config, tokens, i) ||
		    tokens.is_next("(", i));
}

void parallel::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<parallel>())
	{
		tokens.register_syntax<parallel>();
		tokens.register_token<parse::symbol>();
		sequence::register_syntax(tokens);
		prs_lang::assignment::register_syntax(tokens);
		loop::register_syntax(tokens);
		condition::register_syntax(tokens);
	}
}

string parallel::to_string(string tab)
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

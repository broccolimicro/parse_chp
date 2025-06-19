/*
 * composition.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: nbingham
 */

#include "composition.h"

#include <parse/default/symbol.h>
#include <parse/default/number.h>
#include <parse/default/white_space.h>
#include <parse/default/new_line.h>

namespace parse_chp
{

vector<string> composition::precedence;

composition::composition()
{
	debug_name = "composition";
	level = 0;
	reset = -1;
	init();
}

composition::composition(tokenizer &tokens, int level, void *data)
{
	debug_name = "composition";
	this->level = level;
	reset = -1;
	init();
	parse(tokens, data);
}

composition::~composition()
{
}

void composition::init()
{
	if (precedence.size() == 0)
	{
		precedence.push_back("||");
		precedence.push_back(";");
		precedence.push_back(",");
	}
}

void composition::parse(tokenizer &tokens, void *data)
{
	tokens.syntax_start(this);

	bool first = true;
	do
	{
		if (first)
			first = false;
		else
			tokens.next();

		if (precedence[level] == ";")
		{
			tokens.increment(false);
			tokens.expect("@");

			if (tokens.decrement(__FILE__, __LINE__, data))
			{
				if (reset == -1)
					reset = branches.size();
				else
					tokens.error("only one reset allowed per sequence", __FILE__, __LINE__);

				tokens.next();
			}
		}

		tokens.increment(false);
		tokens.expect(precedence[level]);

		tokens.increment(true);
		if (level < (int)precedence.size()-1)
			tokens.expect<composition>();
		else
		{
			tokens.expect<control>();
			tokens.expect<assignment>();
			tokens.expect("(");
			tokens.expect("skip");
		}

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (tokens.found<composition>())
				branches.push_back(branch(composition(tokens, level+1, data)));
			else if (tokens.found<control>())
				branches.push_back(branch(control(tokens, data)));
			else if (tokens.found<assignment>())
				branches.push_back(branch(assignment(tokens, data)));
			else if (tokens.found("skip"))
				tokens.next();
			else if (tokens.found("("))
			{
				tokens.next();

				tokens.increment(false);
				tokens.expect("'");

				tokens.increment(true);
				tokens.expect(")");

				tokens.increment(true);
				tokens.expect<composition>();

				if (tokens.decrement(__FILE__, __LINE__, data))
					branches.push_back(branch(composition(tokens, 0, data)));

				if (tokens.decrement(__FILE__, __LINE__, data))
					tokens.next();

				if (tokens.decrement(__FILE__, __LINE__, data))
				{
					tokens.next();

					tokens.increment(true);
					tokens.expect<parse::number>();

					if (tokens.decrement(__FILE__, __LINE__, data))
						branches.back().sub.region = tokens.next();
				}
			}
		}
	} while (tokens.decrement(__FILE__, __LINE__, data));

	if (precedence[level] == ";")
	{
		tokens.increment(false);
		tokens.expect("@");

		if (tokens.decrement(__FILE__, __LINE__, data))
		{
			if (reset == -1)
				reset = branches.size();
			else
				tokens.error("only one reset allowed per sequence", __FILE__, __LINE__);

			tokens.next();
		}
	}

	tokens.syntax_end(this);
}

bool composition::is_next(tokenizer &tokens, int i, void *data)
{
	return tokens.is_next("@", i) || tokens.is_next("(", i) || control::is_next(tokens, i, data) || assignment::is_next(tokens, i, data);
}

void composition::register_syntax(tokenizer &tokens)
{
	if (!tokens.syntax_registered<composition>())
	{
		tokens.register_syntax<composition>();
		tokens.register_token<parse::symbol>();
		tokens.register_token<parse::number>();
		tokens.register_token<parse::white_space>(false);
		tokens.register_token<parse::new_line>(false);
		control::register_syntax(tokens);
		assignment::register_syntax(tokens);
	}
}

string composition::to_string(string tab) const
{
	return to_string(-1, tab);
}

string composition::to_string(int prev_level, string tab) const {
	if (!valid or branches.size() == 0) {
		return "skip";
	}

	string result = "";
	if (prev_level > level or region != "") {
		result += "(";
	}

	if (reset == 0) {
		result += "@";
	}

	for (int i = 0; i < (int)branches.size(); i++) {
		if (i != 0) {
			result += precedence[level];
		}
		if (reset == i) {
			result += "@";
		}

		result += branches[i].to_string(level, tab);
	}

	if (prev_level > level or region != "") {
		result += ")";
	}

	if (region != "") {
		result += "'" + region;
	}

	return result;
}

parse::syntax *composition::clone() const
{
	return new composition(*this);
}

branch::branch()
{

}

branch::branch(composition sub)
{
	this->sub = sub;
}

branch::branch(control ctrl)
{
	this->ctrl = ctrl;
}

branch::branch(assignment assign)
{
	this->assign = assign;
}

branch::~branch()
{

}

string branch::to_string(int level, string tab) const
{
	if (sub.valid)
		return sub.to_string(level, tab);
	if (ctrl.valid)
		return ctrl.to_string(tab);
	if (assign.valid)
		return assign.to_string(tab);
	else
		return "skip";
}

}

/*
 * factory.h
 *
 *  Created on: Aug 16, 2015
 *      Author: nbingham
 */

#include "composition.h"
#include "control.h"

#ifndef factory_h
#define factory_h

namespace parse_chp
{
	parse::syntax *produce(tokenizer &tokens, void *data)
	{
		return new composition(tokens, data);
	};

	string factory_type()
	{
		return "[" + composition().debug_name + "]";
	}
}

#endif

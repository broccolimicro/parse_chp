#include "expression.h"
#include <parse_expression/precedence.h>

namespace parse_chp {

using parse_expression::precedence_set;
using parse_expression::operation_set;
using parse_expression::operation;

void setup_expressions() {
	if (expression::precedence.empty()) {
		precedence_set result;
		result.push(operation_set::BINARY);
		result.push_back("", "", "|", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "&", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "^", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "==", "");
		result.push_back("", "", "!=", "");
		result.push_back("", "", "<", "");
		result.push_back("", "", ">", "");
		result.push_back("", "", "<=", "");
		result.push_back("", "", ">=", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "||", "");
		
		result.push(operation_set::BINARY);
		result.push_back("", "", "&&", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "<<", "");
		result.push_back("", "", ">>", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "+", "");
		result.push_back("", "", "-", "");

		result.push(operation_set::BINARY);
		result.push_back("", "", "*", "");
		result.push_back("", "", "/", "");
		result.push_back("", "", "%", "");

		result.push(operation_set::UNARY);
		result.push_back("!", "", "", "");
		result.push_back("~", "", "", "");
		result.push_back("(bool)", "", "", "");
		result.push_back("+", "", "", "");
		result.push_back("-", "", "", "");

		result.push(operation_set::MODIFIER);
		result.push_back("", "!", "", "");
		
		result.push(operation_set::UNARY);
		result.push_back("#", "", "", "");
		result.push_back("", "", "", "?");

		result.push(operation_set::MODIFIER);
		result.push_back("", "'", "", "", operation::LITERAL, operation::LABEL);

		result.push(operation_set::MODIFIER);
		result.push_back("", "(", ",", ")", operation::TERM, operation::LITERAL);
		result.push_back("", ".", "", "", operation::LITERAL, operation::LABEL);
		result.push_back("", "[", ":", "]");
		
		result.push(operation_set::MODIFIER);
		result.push_back("", "::", "", "");

		expression::register_precedence(result);
		assignment::lvalueLevel = result.size()-5;
	}
}

}

#include "regex_expression_unit_test.h"
#include "regex_expression.h"

std::string RegexExpressionGenSymbol(std::string str)
{
	RegexExpression re(str.c_str());
	std::string res;
	const char *symbol;
	while ((symbol = re.GetNextSymbol()) != NULL) {
		res += *symbol;
	}

	return res;
}
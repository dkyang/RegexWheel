#include "regex_expression.h"

// re.cc 中
// 不包含'.'
// 通过转义操作符区分正常的'.','\.'
const char * RegexExpression::kSpecialSymbol = "|*().";
const char * RegexExpression::kPriorityString = "(|.*";
const int RegexExpression::kCharacterNumber = 256;
char * RegexExpression::symbol_weight_ = NULL;



bool RegexExpression::IsSpecialSymbol(char ch) {
	for (const char *p = kSpecialSymbol; *p != '\0'; p++) {
		if (ch == *p)
			return true;
	}
	return false;
}

bool RegexExpression::PriorityHigherOrEqual(char lhs, char rhs) {
	if (symbol_weight_ == NULL) {
		symbol_weight_ = new char[kCharacterNumber]; // FIXME: const 256
		memset(symbol_weight_, 0, sizeof(char)*kCharacterNumber);
		//const char *priority_str = "(|.*"; // 优先级字符串，按优先级从低到高排序
		int weight = 0;
		for (const char *p = kPriorityString; *p != '\0'; p++) {
			symbol_weight_[*p] = weight++;
		}
	}

	return symbol_weight_[lhs] >= symbol_weight_[rhs];
}

// 易错，FIXME
const char* RegexExpression::GetNextSymbol() {
	if (is_concatenation_) {
		is_concatenation_ = false;
		return ".";
	}
	// 判断下一个输入是否隐含一个连接符
	is_concatenation_ = NextIsConcatenation();
	if (*cur_pt_ == '\0')
		return NULL;
	else 
		return cur_pt_++;
}

// FIXME!!可能出错
// 判断当前字符与下一个字符间是否隐含一个连接符
// 写测试用例
// 是否覆盖所有情况？
bool RegexExpression::NextIsConcatenation() {
	char cur = *cur_pt_;
	char next = *(cur_pt_+1);

	// 当前为最后一个输入字符
	if (next == '\0')
		return false;

	// ab || a(
	if (!IsSpecialSymbol(cur) && 
		(!IsSpecialSymbol(next) || next == '('))
		return true;

	// )a || )(
	if (cur == ')' && (!IsSpecialSymbol(next) || next == '('))
		return true;

	// *a || *(
	if (cur == '*' && (!IsSpecialSymbol(next) || next == '('))
		return true;

	return false;
}
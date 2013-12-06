#include "regex_expression.h"

// re.cc ��
// ������'.'
// ͨ��ת�����������������'.','\.'
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
		//const char *priority_str = "(|.*"; // ���ȼ��ַ����������ȼ��ӵ͵�������
		int weight = 0;
		for (const char *p = kPriorityString; *p != '\0'; p++) {
			symbol_weight_[*p] = weight++;
		}
	}

	return symbol_weight_[lhs] >= symbol_weight_[rhs];
}

// �״�FIXME
const char* RegexExpression::GetNextSymbol() {
	if (is_concatenation_) {
		is_concatenation_ = false;
		return ".";
	}
	// �ж���һ�������Ƿ�����һ�����ӷ�
	is_concatenation_ = NextIsConcatenation();
	if (*cur_pt_ == '\0')
		return NULL;
	else 
		return cur_pt_++;
}

// FIXME!!���ܳ���
// �жϵ�ǰ�ַ�����һ���ַ����Ƿ�����һ�����ӷ�
// д��������
// �Ƿ񸲸����������
bool RegexExpression::NextIsConcatenation() {
	char cur = *cur_pt_;
	char next = *(cur_pt_+1);

	// ��ǰΪ���һ�������ַ�
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
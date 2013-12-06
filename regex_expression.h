#ifndef _REGEX_EXPRESSION_H_
#define _REGEX_EXPRESSION_H_

#include <iostream>
#include <cstdlib>

class RegexExpression 
{
public:
	// FIXME: ��δ�������
	RegexExpression(const char *str) 
		: str_(str), cur_pt_(str), is_concatenation_(false) {
		if (*str == NULL || *str == '\0') {
			std::cerr << "input string is invalid" << std::endl;
			std::exit(EXIT_FAILURE);
		}
		str_ = str;
	}

	const char *str() const {
		return str_;
	}

	// ���ȼ��Ƚ�
	// ����ֵ > 0��˵��lhs���ȼ���rhs��
	// ˳�������
	// ����ģʽ��
	// FIXME
	// ���ȼ��߱�ջ�����򵯳���
	const char* GetNextSymbol();

	static bool PriorityHigherOrEqual(char lhs, char rhs);
	static bool IsSpecialSymbol(char ch);
	// FIXME: ��̬��Ա�����ĳ�ʼ��
	static const char *kSpecialSymbol;
private:
	const char *str_;
	const char *cur_pt_; //��ǰָ��ָ��λ��
	bool is_concatenation_;
	//const char *special_symbol_;  // ���ַ���������ʾ�������
	// ������symbol_weight_������ڴ治�ͷţ��ȴ���������ͷ�
	static char *symbol_weight_; // һ����256���ַ��������ȼ���ֵ�����Գ�ʼ�������Ƶ���ģʽ������
	static const int kCharacterNumber;
	static const char *kPriorityString;

	bool NextIsConcatenation();
};

#endif
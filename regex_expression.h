#ifndef _REGEX_EXPRESSION_H_
#define _REGEX_EXPRESSION_H_

#include <iostream>
#include <cstdlib>

class RegexExpression 
{
public:
	// FIXME: 如何错误处理？？
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

	// 优先级比较
	// 返回值 > 0，说明lhs优先级比rhs高
	// 顺序遍历？
	// 单例模式？
	// FIXME
	// 优先级高比栈顶高则弹出？
	const char* GetNextSymbol();

	static bool PriorityHigherOrEqual(char lhs, char rhs);
	static bool IsSpecialSymbol(char ch);
	// FIXME: 静态成员变量的初始化
	static const char *kSpecialSymbol;
private:
	const char *str_;
	const char *cur_pt_; //当前指针指向位置
	bool is_concatenation_;
	//const char *special_symbol_;  // 用字符串常量表示特殊符号
	// ！！！symbol_weight_分配的内存不释放，等待程序结束释放
	static char *symbol_weight_; // 一共有256个字符，按优先级赋值，惰性初始化，类似单例模式！！！
	static const int kCharacterNumber;
	static const char *kPriorityString;

	bool NextIsConcatenation();
};

#endif
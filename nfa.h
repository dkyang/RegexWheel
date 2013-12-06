#ifndef _NFA_H_
#define _NFA_H_

#include "fa.h"

#include <list>
#include <vector>
#include <stack>
#include <iostream>
#include <cstdlib>

/*
当嵌套 (或成员) 类只被外围类使用时非常有用; 把它作为外围类作用域内的成员, 而不是去污染外部作用域的同名类. 
嵌套类可以在外围类中做前置声明, 然后在 .cc 文件中定义, 这样避免在外围类的声明中定义嵌套类, 因为嵌套类的定义通常只与实现相关.
*/

/*
TODO：
	StateSet内存交给shared_ptr处理
*/


class RegexExpression;

class NFA : public FA
{
public:
	// 通过正则表达式生成NFA
	// FIXME: 不放在构造函数中
	NFA(RegexExpression *re);

	// for debug
	void PrintNFA();

	//State* 

	~NFA() {
		FreeStates();
	}
private:


	//RegexExpression* re_;

	//const char *input_str_;
	std::stack<char> special_symbol_stack_;
	std::stack<StateSet*> state_set_stack_;

	// FIXME:定义为内联
	void HandleInputLeftBracket() {
		special_symbol_stack_.push('(');
	}

	// 待测试：(a* | b)
	void HandleInputRightBracket();

	void HandleOtherInputSymbol(char symbol);

	// 完成StateSet内存的释放
	void HandelSpecialSymbol(char symbol);

	// FIXME:错误处理
	StateSet * MakeStateSet(StateSet *set1, StateSet *set2, char symbol);

	// 通过一个输入字母生成由两个状态、一条边组成的基本FSM
	StateSet* MakeSimpleStateSet(char alpha);

	// 用指针？
	StateSet* Union(StateSet *set1, StateSet *set2);

	StateSet* Concatenation(StateSet *set1, StateSet *set2);

	StateSet * Closure(StateSet *set);

	// for debug
	// 深度优先遍历输出NFA的各边
	void TraverseNFA(State *state, std::vector<bool>& mark);
};

#endif


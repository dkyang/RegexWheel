#ifndef _NFA_H_
#define _NFA_H_

#include "fa.h"

#include <list>
#include <vector>
#include <stack>
#include <iostream>
#include <cstdlib>

/*
��Ƕ�� (���Ա) ��ֻ����Χ��ʹ��ʱ�ǳ�����; ������Ϊ��Χ���������ڵĳ�Ա, ������ȥ��Ⱦ�ⲿ�������ͬ����. 
Ƕ�����������Χ������ǰ������, Ȼ���� .cc �ļ��ж���, ������������Χ��������ж���Ƕ����, ��ΪǶ����Ķ���ͨ��ֻ��ʵ�����.
*/

/*
TODO��
	StateSet�ڴ潻��shared_ptr����
*/


class RegexExpression;

class NFA : public FA
{
public:
	// ͨ��������ʽ����NFA
	// FIXME: �����ڹ��캯����
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

	// FIXME:����Ϊ����
	void HandleInputLeftBracket() {
		special_symbol_stack_.push('(');
	}

	// �����ԣ�(a* | b)
	void HandleInputRightBracket();

	void HandleOtherInputSymbol(char symbol);

	// ���StateSet�ڴ���ͷ�
	void HandelSpecialSymbol(char symbol);

	// FIXME:������
	StateSet * MakeStateSet(StateSet *set1, StateSet *set2, char symbol);

	// ͨ��һ��������ĸ����������״̬��һ������ɵĻ���FSM
	StateSet* MakeSimpleStateSet(char alpha);

	// ��ָ�룿
	StateSet* Union(StateSet *set1, StateSet *set2);

	StateSet* Concatenation(StateSet *set1, StateSet *set2);

	StateSet * Closure(StateSet *set);

	// for debug
	// ������ȱ������NFA�ĸ���
	void TraverseNFA(State *state, std::vector<bool>& mark);
};

#endif


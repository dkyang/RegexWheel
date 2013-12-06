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

	void GetStateEpsilonClosure(std::vector<std::vector<int> >& eps_vec) {
		eps_vec.resize(state_count_);
		for (std::list<State*>::iterator lit = state_list_.begin(); 
				lit != state_list_.end(); lit++) {
			State *state = *lit;
			std::fill(mark.begin(), mark.end(), false);
			vector<int> eps_closure;
			RecursiveEps(state, mark, eps_closure);
			eps_vec[state->state_] = eps_closure;
		}
	}

	// 得到按状态序号排序的状态数组
	void GetSortStates(std::vector<State*>& state_vec) {
		state_vec.resize(state_count_);
		for (std::list<State*>::iterator lit = state_list_.begin;
				lit != state_list_.end(); lit++) {
			State *state = *lit;
			state_vec[state->state_] = state;
		}
	}

	int GetEndState() {
		return end_state_vec_[0].state_;
	}

	~NFA() {
		FreeStates();
	}

	// for debug
	void PrintNFA();
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

	void RecursiveEps(State *state, std::vector<bool>& mark, std::vector<int>& eps_closure) {
		mark[state->state_] = true;
		eps_closure.push_back(state->state_);

		for (std::vector<Edge*>::iterator eit = state->out_edges_.begin(); 
			eit != state->out_edges_.end(); eit++) {
			Edge *edge = *eit;
			if (!mark[edge->to_->state_] && edge->is_epsilon())
				RecursiveEps(edge->to_, mark, eps_closure);
		}
	}

	// for debug
	// 深度优先遍历输出NFA的各边
	void TraverseNFA(State *state, std::vector<bool>& mark);
};

#endif


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

	// �õ���״̬��������״̬����
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
	// ������ȱ������NFA�ĸ���
	void TraverseNFA(State *state, std::vector<bool>& mark);
};

#endif


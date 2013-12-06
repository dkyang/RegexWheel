#include "nfa.h"
#include "regex_expression.h"

#include <iostream>
#include <assert.h>


NFA::NFA(RegexExpression *re) 
//	: FA()
{
	const char* symbol;
	while ((symbol = re->GetNextSymbol()) != NULL) {
		if (!RegexExpression::IsSpecialSymbol(*symbol)) {
			// ��ǰ�ַ������������
			StateSet *state_set = MakeSimpleStateSet(*symbol);
			state_set_stack_.push(state_set);
		} else {
			// ���������
			switch (*symbol) {
			case '(':
				//special_symbol_stack_.push(*symbol);
				HandleInputLeftBracket();
				break;
			case ')':
				HandleInputRightBracket();
				break;
			default:
				// ���������������
				HandleOtherInputSymbol(*symbol);
				break;
			}
		}
	}

	// FIXME��ȫ�����봦����Ϻ󣬿���ջ�л���Ԫ�أ���������
	while (!special_symbol_stack_.empty()) {
		char c = special_symbol_stack_.top();
		special_symbol_stack_.pop();
		HandelSpecialSymbol(c);
	}

	// ����ջ��һ��ֻ��һ��Ԫ�أ������Ԫ���Ǳ������������NFA����Ϣ
	assert(state_set_stack_.size() == 1);
	StateSet* res_state_set = state_set_stack_.top();
	start_state_ = res_state_set->start_;
	end_state_vec_.push_back(res_state_set->end_);
	res_state_set->end_->is_end_state_ = true; // Ϊ��ֹ״̬

	delete res_state_set; // FIXME: ʹ������ͷ��ڴ棬��Ϊshared_ptr<>
}

// �����ԣ�(a* | b)
void NFA::HandleInputRightBracket() 
{
	char symbol;
	while ((symbol = special_symbol_stack_.top()) != '(') {
		special_symbol_stack_.pop();
		HandelSpecialSymbol(symbol);
	}
	special_symbol_stack_.pop(); // ����ջ��ʣ�µ�������
}

void NFA::HandleOtherInputSymbol(char symbol)
{
	while (!special_symbol_stack_.empty() && 
		RegexExpression::PriorityHigherOrEqual(
		special_symbol_stack_.top(), symbol)) {
			char c = special_symbol_stack_.top();
			special_symbol_stack_.pop();
			HandelSpecialSymbol(c);
	} 
	special_symbol_stack_.push(symbol);
} 

void NFA::HandelSpecialSymbol(char symbol) 
{
	if (symbol == '*') { 
		// ������Ŀ�������
		StateSet *set1 = state_set_stack_.top();
		state_set_stack_.pop();
		StateSet *new_set = MakeStateSet(set1, NULL, symbol);
		state_set_stack_.push(new_set);
		delete set1; // ʹ����ϣ��ͷ��ڴ�
	} else if (symbol == '|' || symbol == '.') { 
		// ����˫Ŀ�������
		StateSet *set1 = state_set_stack_.top();
		state_set_stack_.pop();
		StateSet *set2 = state_set_stack_.top();
		state_set_stack_.pop();
		//!!˫Ŀ��������ȳ�ջ�ķ��ں��棬˳�����Ҫ
		//StateSet *new_set = MakeStateSet(set1, set2, symbol);
		StateSet *new_set = MakeStateSet(set2, set1, symbol); 
		state_set_stack_.push(new_set);
		delete set1;
		delete set2;
	}
}

// FIXME:������
FA::StateSet* NFA::MakeStateSet(StateSet *set1, StateSet *set2, char symbol)
{
	StateSet* res_state_set = NULL;
	switch (symbol) {
	case '|': 
		if (set1 == NULL || set2 == NULL) {
			std::cerr << "state set can't be null when operator is union."
				<< std::endl;
			std::exit(EXIT_FAILURE);
		}
		else 
			res_state_set = Union(set1, set2);
		break;
	case '.':
		if (set1 == NULL || set2 == NULL) {
			std::cerr << "state set can't be null when operator is concatenation."
				<< std::endl;
			std::exit(EXIT_FAILURE);
		}
		else
			res_state_set = Concatenation(set1, set2);
		break;
	case '*':
		if (set1 == NULL) {
			std::cerr << "state set can't be null when operator is closure."
				<< std::endl;
			std::exit(EXIT_FAILURE);
		}
		else
			res_state_set = Closure(set1);
		break;
	default:
		//FIXME
		break;
	}
	return res_state_set;
}

FA::StateSet* NFA::MakeSimpleStateSet(char alpha) 
{
	State *start_state = AllocState();
	State *end_state = AllocState();
	start_state->AddOutEdge(end_state, alpha);
	StateSet *new_set = new StateSet(start_state, end_state);
	return new_set;
}

FA::StateSet* NFA::Union(StateSet *set1, StateSet *set2)
{
	State *start_state = AllocState();
	start_state->AddOutEdge(set1->start_);
	start_state->AddOutEdge(set2->start_);

	State *end_state = AllocState();
	set1->end_->AddOutEdge(end_state);
	set2->end_->AddOutEdge(end_state);

	StateSet *new_set = new StateSet(start_state, end_state);
	return new_set;
}

FA::StateSet* NFA::Concatenation(StateSet *set1, StateSet *set2) 
{
	set1->end_->AddOutEdge(set2->start_);
	StateSet *new_set = new StateSet(set1->start_, set2->end_);
	return new_set;
}

FA::StateSet* NFA::Closure(StateSet *set)
{
	State *start_state = AllocState();
	State *end_state = AllocState();
	start_state->AddOutEdge(end_state);
	start_state->AddOutEdge(set->start_);
	set->end_->AddOutEdge(set->start_);
	set->end_->AddOutEdge(end_state);
	StateSet *new_set = new StateSet(start_state, end_state);
	return new_set;
}


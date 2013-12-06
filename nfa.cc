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
			// 当前字符不是特殊符号
			StateSet *state_set = MakeSimpleStateSet(*symbol);
			state_set_stack_.push(state_set);
		} else {
			// 是特殊符号
			switch (*symbol) {
			case '(':
				//special_symbol_stack_.push(*symbol);
				HandleInputLeftBracket();
				break;
			case ')':
				HandleInputRightBracket();
				break;
			default:
				// 处理其他特殊符号
				HandleOtherInputSymbol(*symbol);
				break;
			}
		}
	}

	// FIXME：全部输入处理完毕后，可能栈中还有元素，继续处理
	while (!special_symbol_stack_.empty()) {
		char c = special_symbol_stack_.top();
		special_symbol_stack_.pop();
		HandelSpecialSymbol(c);
	}

	// 最终栈中一定只有一个元素，且这个元素是保存了最后生成NFA的信息
	assert(state_set_stack_.size() == 1);
	StateSet* res_state_set = state_set_stack_.top();
	start_state_ = res_state_set->start_;
	end_state_ = res_state_set->end_;
	delete res_state_set; // FIXME: 使用完就释放内存，改为shared_ptr<>
}

// 待测试：(a* | b)
void NFA::HandleInputRightBracket() 
{
	char symbol;
	while ((symbol = special_symbol_stack_.top()) != '(') {
		special_symbol_stack_.pop();
		HandelSpecialSymbol(symbol);
	}
	special_symbol_stack_.pop(); // 弹出栈中剩下的左括号
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
		// 处理“单目运算符”
		StateSet *set1 = state_set_stack_.top();
		state_set_stack_.pop();
		StateSet *new_set = MakeStateSet(set1, NULL, symbol);
		state_set_stack_.push(new_set);
		delete set1; // 使用完毕，释放内存
	} else if (symbol == '|' || symbol == '.') { 
		// 处理“双目运算符”
		StateSet *set1 = state_set_stack_.top();
		state_set_stack_.pop();
		StateSet *set2 = state_set_stack_.top();
		state_set_stack_.pop();
		//!!双目运算符，先出栈的放在后面，顺序很重要
		//StateSet *new_set = MakeStateSet(set1, set2, symbol);
		StateSet *new_set = MakeStateSet(set2, set1, symbol); 
		state_set_stack_.push(new_set);
		delete set1;
		delete set2;
	}
}

// FIXME:错误处理
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

//-----------------------debug-------------------------------//
void NFA::PrintNFA()
{
	std::cout << "start state is: " << start_state_->state_ << std::endl;
	std::cout << "end state is: " << end_state_->state_ << std::endl;
	//State* pt = start_state_;
	std::vector<bool> mark(state_count_, false);
	TraverseNFA(start_state_, mark);
}

void NFA::TraverseNFA(State *state, std::vector<bool>& mark)
{
	if (state == end_state_)
		return;
	mark[state->state_] = true;

	for (std::vector<Edge*>::iterator eit = state->out_edges_.begin(); 
		eit != state->out_edges_.end(); eit++) {
			Edge *edge = *eit;
			std::cout << edge->from_->state_ << " -> " << edge->to_->state_
				<< " : ";
			if (edge->is_epsilon()) 
				std::cout << "epsilon" << std::endl;
			else
				std::cout << edge->alpha_ << std::endl;
			if (!mark[edge->to_->state_])
				TraverseNFA(edge->to_, mark);
	}
}
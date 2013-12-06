#include "nfa.h"
#include "regex_expression.h"

#include <iostream>
#include <assert.h>

// Ƕ��������Χ��ͷ�ļ�����������.cc�ļ��ж���
// ��google C++ style�ж�Ƕ�����˵��

// nested class of NFA
class NFA::State 
{
public:
	State(int state) 
		: state_(state)
	{	}

	void AddOutEdge(State *to, char alpha);

	// epsilon-edge
	void AddOutEdge(State *to);

	~State();

	int state_;
	//type: begin, terminal, normal
	std::vector<Edge*> out_edges_;
};

class NFA::Edge
{
public:
	Edge(State *from, State *to, char alpha) 
		: from_(from), to_(to), is_epsilon_(false), alpha_(alpha)
	{	}

	// epsilon edge
	Edge(State *from, State *to) 
		: from_(from), to_(to), is_epsilon_(true)
	{	}

	bool is_epsilon()
	{
		return is_epsilon_;
	}

	State *from_;
	State *to_;
	bool is_epsilon_;
	char alpha_;
};

// �������ݽṹ��������re->NFA��ת�������б����м���
class NFA::StateSet {
public:
	StateSet(State *start, State *end) 
		: start_(start), end_(end) 
	{	}
	State *start_;
	State *end_;
};

void NFA::State::AddOutEdge(State *to, char alpha)
{
	Edge *edge = new Edge(this, to, alpha);
	out_edges_.push_back(edge);
}

// epsilon-edge
void NFA::State::AddOutEdge(State *to) 
{
	Edge *edge = new Edge(this, to);
	out_edges_.push_back(edge);
}

NFA::State::~State() 
{
	// Edge���ڴ����ȫ����State�ڲ������Կ����������������ͷţ�
	// ��֤û���ڴ�й¶
	for (std::vector<Edge*>::iterator vit = out_edges_.begin(); 
		vit != out_edges_.end(); vit++) {
			delete *vit;
			*vit = NULL;
	}
}


NFA::NFA(RegexExpression *re) 
	: state_count_(0)
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
	end_state_ = res_state_set->end_;
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
NFA::StateSet* NFA::MakeStateSet(StateSet *set1, StateSet *set2, char symbol)
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

NFA::StateSet* NFA::MakeSimpleStateSet(char alpha) 
{
	State *start_state = AllocState();
	State *end_state = AllocState();
	start_state->AddOutEdge(end_state, alpha);
	StateSet *new_set = new StateSet(start_state, end_state);
	return new_set;
}

NFA::StateSet* NFA::Union(StateSet *set1, StateSet *set2)
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

NFA::StateSet* NFA::Concatenation(StateSet *set1, StateSet *set2) 
{
	set1->end_->AddOutEdge(set2->start_);
	StateSet *new_set = new StateSet(set1->start_, set2->end_);
	return new_set;
}

NFA::StateSet* NFA::Closure(StateSet *set)
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

NFA::State* NFA::AllocState() 
{
	State *state = new State(state_count_++);
	state_list_.push_back(state); // ��¼���Ա��ͷ��ڴ�
	return state;
}

// �ͷŶ�̬�����State�ڴ�
void NFA::FreeStates()
{
	for (std::list<State*>::iterator lit = state_list_.begin(); 
		lit != state_list_.end(); lit++) {
		delete *lit;
	}
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
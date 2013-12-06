#include "fa.h"

void FA::State::AddOutEdge(State *to, char alpha)
{
	Edge *edge = new Edge(this, to, alpha);
	out_edges_.push_back(edge);
}

// epsilon-edge
void FA::State::AddOutEdge(State *to) 
{
	Edge *edge = new Edge(this, to);
	out_edges_.push_back(edge);
}

FA::State::~State() 
{
	// Edge���ڴ����ȫ����State�ڲ������Կ����������������ͷţ�
	// ��֤û���ڴ�й¶
	for (std::vector<Edge*>::iterator vit = out_edges_.begin(); 
		vit != out_edges_.end(); vit++) {
			delete *vit;
			*vit = NULL;
	}
}

FA::State* FA::start_state() {
	return start_state_;
}

FA::State* FA::AllocState() 
{
	State *state = new State(state_count_++);
	state_list_.push_back(state); // ��¼���Ա��ͷ��ڴ�
	return state;
}

// �ͷŶ�̬�����State�ڴ�
void FA::FreeStates()
{
	for (std::list<State*>::iterator lit = state_list_.begin(); 
		lit != state_list_.end(); lit++) {
			delete *lit;
	}
}
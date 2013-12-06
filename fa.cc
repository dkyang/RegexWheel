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
	// Edge的内存分配全都在State内部，所以可以在析构函数中释放，
	// 保证没有内存泄露
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
	state_list_.push_back(state); // 记录，以便释放内存
	return state;
}

// 释放动态分配的State内存
void FA::FreeStates()
{
	for (std::list<State*>::iterator lit = state_list_.begin(); 
		lit != state_list_.end(); lit++) {
			delete *lit;
	}
}
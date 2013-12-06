#include "fa.h"

#include <iostream>

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
	state_vec_.push_back(state); // 记录，以便释放内存
	return state;
}

// 释放动态分配的State内存
void FA::FreeStates()
{
	for (std::vector<State*>::iterator vit = state_vec_.begin(); 
		vit != state_vec_.end(); vit++) {
			delete *vit;
	}
}

//-----------------------debug-------------------------------//
void FA::PrintFA()
{
	std::cout << "start state is: " << start_state_->state_ << std::endl;
	//std::cout << "end state is: " << end_state_->state_ << std::endl;
	//State* pt = start_state_;
	std::vector<bool> mark(state_count_, false);
	TraverseFA(start_state_, mark);

	std::cout << "end state is: ";
	for (size_t i = 0; i < end_state_vec_.size(); i++)
		std::cout << end_state_vec_[i]->state_ << ' ';
	std::cout << std::endl;
}

void FA::TraverseFA(State *state, std::vector<bool>& mark)
{
	//if (state == end_state_)
	//	return;
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
				TraverseFA(edge->to_, mark);
	}
}
#ifndef _DFA_H_
#define _DFA_H_

#include "fa.h"

class DFA : public FA
{
public:
	void ConstructFromNFA(NFA *nfa) {

		std::vector<std::vector<int> > eps_vec;
		nfa->GetStateEpsilonClosure(eps_vec);
		
		// 似乎不需要转换，state_list_本来就是按序号排序的，
		// 待转换为vector表示
		std::vector<State*> state_vec;
		nfa->GetSortStates(state_vec);


		// FIXME:假设字符个数总共256！！
		std::vector<bool> mark(256, false);
		std::vector<HelperState> active_states;
		State *start_state = AllocState();
		HelperState helper_state(eps_vec[nfa->start_state_]
								start_state);
		active_states.push_back(helper_state);
		start_state_ = start_state;

		for (int cur = 0; cur < active_states.size(); cur++) {
			std::vector<int>& cur_nfa_state = active_states[cur];
			State *from_state = active_states[cur].dfa_state_;
			std::vector<char> alphas;
			std::fill(mark.begin(), mark.end(), false);
			GetCurAlphas(eps, alphas, mark);

			// 遍历所有可能的输入字母
			for (int i = 0; i < alphas.size(); i++) {
				char alpha = alphas[i];
				std::vector<int> res_state;
				// 遍历当前DFA状态包含的所有NFA状态
				for (std::vector<int>::iterator vit = cur_nfa_state.begin(); 
					vit != cur_nfa_state.end(); vit++) {
					State *state = state_list_[*vit];

					//遍历当前NFA状态的所有边
					for (std::vector<Edge*>::iterator eit = state->out_edges_.begin();
						eit != state->out_edges_.end(); eit++) {
						Edge *edge = *eit;
						if (edge->alpha_ == alpha) 
							res_state.push_back(edge->to_->state_);
					}
				}
				if (res_state.empty())
					continue;

				// 标识当前计算得到的状态是否已经存在于active_states中
				bool is_in_active_states = false;
				int j;
				for (j = 0; j <= cur && !is_in_active_states; j++) {
					std::vector<int>& cmp_state = active_states[j].nfa_states_;
					// 首先两个状态的长度应该相同
					if (cmp_state.size() != res_state.size())
						continue;
					int k;
					for (k = 0; k < cmp_state.size(); k++) {
						if (cmp_state[k] != res_state[k])
							break;
					}
					if (k == cmp_state.size())
						is_in_active_states = true;
				}
				// 相等时，active_states[j]就是找到的相同State
				State *to_state;

				// 新的状态不在active_states中，所以要放入active_states
				// 并且要分配新的DFA节点
				if (!is_in_active_states) {
					//active_states.push_back(res_state);
					//state = AllocState();
					to_state = AllocState();
					HelperState new_helper_state(res_state, to_state);
					active_states.push_back(helper_state);
				} else {
					// 已存在active_states;
					to_state = active_states[j].dfa_state_;
				}

				// 为这两个DFA state添加一条边
				//Edge *edge = new Edge(from_state, to_state, alpha);
				from_state->AddOutEdge(to_state);
			}
		}

		int nfa_end_state = nfa->GetEndState();
		// 判断当前DFA状态是不是终止状态
		for (int i = 0; i < active_states.size(); i++) {
			std::vector<int>& nfa_states = active_states[i].nfa_states_;
			for (int j = 0; j < nfa_states.size(); j++) {
				if (nfa_states[j] == nfa_end_state) {
					active_states[i].dfa_state_->is_end_state_ = true;
					end_state_vec_.push_back(active_states[i].dfa_state_);
					break;
				}
			}
		}
	}
private:

	class HelperState
	{
	public:
		HelperState(std::vector<int> nfa_states, State *dfa_state)
			: nfa_states_(nfa_states), dfa_state_(dfa_state)
		{	}
		std::vector<int> nfa_states_;
		State *dfa_state_;
	};

	// 从当前的状态集，获得所有可能的字母集。也就是可能通过输出边字母的并集。
	// 可能很耗时
	// 注意不要重复
	void GetCurAlphas(std::vector<int>& states, std::vector<char>& alphas,
			std::vector<bool>& mark) {
		for (std::vector<int>::iterator vit = states.begin(); 
				vit != states.end(); vit++) {
			State *state = state_list_[*vit];
			for (std::vector<Edge*>::iterator eit = state->out_edges_.begin();
					eit != state->out_edges_.end(); eit++) {
				Edge *edge = *eit;
				if (!edge->is_epsilon && !mark[edge->alpha_]) {
					mark[edge->alpha_] = true;
					alphas.push_back(edge->alpha_);
				}
			}
		}
	}
};
#endif
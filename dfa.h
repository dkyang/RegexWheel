#ifndef _DFA_H_
#define _DFA_H_

#include "fa.h"

class DFA : public FA
{
public:
	void ConstructFromNFA(NFA *nfa) {

		std::vector<std::vector<int> > eps_vec;
		nfa->GetStateEpsilonClosure(eps_vec);
		
		// �ƺ�����Ҫת����state_list_�������ǰ��������ģ�
		// ��ת��Ϊvector��ʾ
		std::vector<State*> state_vec;
		nfa->GetSortStates(state_vec);


		// FIXME:�����ַ������ܹ�256����
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

			// �������п��ܵ�������ĸ
			for (int i = 0; i < alphas.size(); i++) {
				char alpha = alphas[i];
				std::vector<int> res_state;
				// ������ǰDFA״̬����������NFA״̬
				for (std::vector<int>::iterator vit = cur_nfa_state.begin(); 
					vit != cur_nfa_state.end(); vit++) {
					State *state = state_list_[*vit];

					//������ǰNFA״̬�����б�
					for (std::vector<Edge*>::iterator eit = state->out_edges_.begin();
						eit != state->out_edges_.end(); eit++) {
						Edge *edge = *eit;
						if (edge->alpha_ == alpha) 
							res_state.push_back(edge->to_->state_);
					}
				}
				if (res_state.empty())
					continue;

				// ��ʶ��ǰ����õ���״̬�Ƿ��Ѿ�������active_states��
				bool is_in_active_states = false;
				int j;
				for (j = 0; j <= cur && !is_in_active_states; j++) {
					std::vector<int>& cmp_state = active_states[j].nfa_states_;
					// ��������״̬�ĳ���Ӧ����ͬ
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
				// ���ʱ��active_states[j]�����ҵ�����ͬState
				State *to_state;

				// �µ�״̬����active_states�У�����Ҫ����active_states
				// ����Ҫ�����µ�DFA�ڵ�
				if (!is_in_active_states) {
					//active_states.push_back(res_state);
					//state = AllocState();
					to_state = AllocState();
					HelperState new_helper_state(res_state, to_state);
					active_states.push_back(helper_state);
				} else {
					// �Ѵ���active_states;
					to_state = active_states[j].dfa_state_;
				}

				// Ϊ������DFA state���һ����
				//Edge *edge = new Edge(from_state, to_state, alpha);
				from_state->AddOutEdge(to_state);
			}
		}

		int nfa_end_state = nfa->GetEndState();
		// �жϵ�ǰDFA״̬�ǲ�����ֹ״̬
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

	// �ӵ�ǰ��״̬����������п��ܵ���ĸ����Ҳ���ǿ���ͨ���������ĸ�Ĳ�����
	// ���ܺܺ�ʱ
	// ע�ⲻҪ�ظ�
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
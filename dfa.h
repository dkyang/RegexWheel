#ifndef _DFA_H_
#define _DFA_H_

#include "fa.h"

class DFA : public FA
{
public:
	void ConstructFromNFA(NFA *nfa) {

	}
private:
	typedef std::vector<std::vector<int> > TwoDimVec;
	
	TwoDimVec eps_vec_;

	void ComputeEps(NFA* nfa) {
		
	}
};


#endif
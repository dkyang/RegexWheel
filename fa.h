#ifndef _FA_H_
#define _FA_H_

#include <list>
#include <vector>

// NFA��DFA�Ļ���
class FA 
{	
// nested class
public:
	class State;

protected:

	class Edge;
	class StateSet;

public:
	FA() : state_count_(0), start_state_(NULL)
	{	}

	int state_count() {
		return state_count_;
	}

	//class State;
	State *start_state();

	// for debug
	void PrintFA();

	// nested class
	class State 
	{
	public:
		State(int state) 
			: state_(state), is_end_state_(false)
		{	}

		void AddOutEdge(State *to, char alpha);

		// epsilon-edge
		void AddOutEdge(State *to);

		~State();

		int state_;
		//type: begin, terminal, normal
		std::vector<Edge*> out_edges_;
		bool is_end_state_;
	};

protected:
	class Edge
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
	class StateSet {
	public:
		StateSet(State *start, State *end) 
			: start_(start), end_(end) 
		{	}
		State *start_;
		State *end_;
	};

	//bool is_concatenation_;
	int state_count_;

	// ���涯̬��������ݽṹ�������ͷ�
	std::vector<State*> state_vec_;

	//��ʼ״̬����ֹ״̬
	State* start_state_;
	std::vector<State*> end_state_vec_;

	State * AllocState();

	// �ͷŶ�̬�����State�ڴ�
	void FreeStates();

private:
	// for debug
	// ������ȱ������NFA�ĸ���
	void TraverseFA(State *state, std::vector<bool>& mark);

};

#endif
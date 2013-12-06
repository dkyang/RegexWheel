#ifndef _FA_H_
#define _FA_H_

#include <list>
#include <vector>

// NFA��DFA�Ļ���
class FA 
{
public:
	FA() : state_count_(0), start_state_(NULL)
	{	}

protected:

	// nested class of NFA
	// ǰ������
	class State;
	class Edge;
	class StateSet;

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
	std::list<State*> state_list_;

	//��ʼ״̬����ֹ״̬
	State* start_state_;
	//State* end_state_;

	State * AllocState();

	// �ͷŶ�̬�����State�ڴ�
	void FreeStates();
};

#endif
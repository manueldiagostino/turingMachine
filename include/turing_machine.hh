#ifndef TURING_MACHINE_HH
#define TURING_MACHINE_HH

#include <set>
#include <map>
#include <tuple>
#include <string>
#include <deque>
#include <algorithm>
#include <iostream>

class TuringMachine {
public:
	enum struct Moves_ {left, right};

	using State_ = std::string;
	using Symbol_ = std::string;
	using States_ = std::set<State_>;
	using Alphabet_ = std::set<Symbol_>;
	using Key_ = std::pair<State_, Symbol_>;
	using Value_ = std::tuple<State_, Symbol_, Moves_>;
	using Instructions_ = std::map<Key_, Value_>;
	using Memory_ = std::deque<Symbol_>;

	TuringMachine() = default;
	TuringMachine(
		const Alphabet_& alphabet, const States_& states, 
		const State_& initialState, const Instructions_& instructions,
		const Memory_& memory
		);
	TuringMachine(const TuringMachine& other) = default;
	TuringMachine(TuringMachine&& other) = default;
	~TuringMachine() = default;

	TuringMachine& operator =(const TuringMachine& other) = default;
	TuringMachine& operator =(TuringMachine&& other) = default;

	void setAlphabet(Alphabet_&& alphabet);
	void setInitialState(State_&& state);
	void setStates(States_&& states);
	void setInstructions(Instructions_&& instructions_);

	std::pair<States_::iterator, bool> insertState(const State_& state);
	std::pair<Alphabet_::iterator, bool> insertSymbol(const Symbol_& symbol);
	std::pair<Instructions_::iterator, bool> insertInstruction(const Key_& key, const Value_& value);
	States_ ::iterator eraseState(const State_& state);
	Alphabet_::iterator eraseSymbol(const Symbol_& symbol);
	size_t eraseInstruction(Key_& key);

	const Memory_& run();

private:
	Alphabet_ alphabet_;
	States_ states_;
	State_ currentState_;
	Instructions_ instructions_;
	Memory_ memory_;
	Memory_::iterator headPosition_;

	size_t steps{0};
	void print_debug();

protected:
	void moveHead(const Moves_& direction);
	Memory_::iterator changeState();
};

template <typename T>
inline
std::ostream& operator <<(std::ostream& os, std::set<T>& set) {
	os << '{';
	for (auto it=set.begin(); it!=set.end();) {
		os << *it;

		++it;
		if (it != set.end())
			os << ", ";
	}
	os << '}';

	return os;
}

inline
std::ostream& operator <<(
	std::ostream& os,
	const TuringMachine::Moves_ move) {
	
	if (move == TuringMachine::Moves_::left)
		os << "L";
	else
		os << "R";

	return os;
}

inline 
std::ostream& operator <<(
	std::ostream& os,
	const TuringMachine::Value_& value) {
	#define STATE 0
	#define SYMBOL 1
	#define MOVE 2
	
	os 	<< '(';
	os 	<< std::get<STATE>(value) << ", " 
		<< std::get<SYMBOL>(value) << ", " 
		<< std::get<MOVE>(value);
	os 	<< ")";

	return os;
}

inline
std::ostream& operator <<(
	std::ostream& os,
	const TuringMachine::Instructions_& instructions) {
	
	os << '{';
	for (auto it=instructions.begin(); it!=instructions.end();) {
		auto key = it->first;
		auto value = it->second;

		os 	<< "\n\t[";
		os 	<< "(" << key.first << ", " << key.second << "), ";
		os 	<< value;
		/* os 	<< std::get<0>(value) << ", " 
			<< std::get<1>(value) << ", " 
			<< std::get<2>(value) << "]"; */
		os 	<< ']';

		++it;
		if (it != instructions.end())
			os << ", ";
	}
	os << "\n\t}";

	return os;
}

inline
std::ostream& operator <<(
	std::ostream& os,
	const TuringMachine::Memory_& memory) {

	os << '{';
	for (auto it=memory.begin(); it!=memory.end();) {
		os << *it;

		++it;
		if (it != memory.end())
			os << " ";
	}
	os << '}';
	return os;
}

#endif // ! TURING_MACHINE_HH
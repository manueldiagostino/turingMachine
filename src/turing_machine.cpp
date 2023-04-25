#include "../include/turing_machine.hh"

TuringMachine::TuringMachine(
	const Alphabet_& alphabet, const States_& states, 
	const State_& initialState, const Instructions_& instructions,
	const Memory_& memory
	) :
	alphabet_{alphabet},
	states_{states},
	currentState_{initialState},
	instructions_{instructions},
	memory_{memory},
	headPosition_{memory_.begin()} { 

		#ifdef DEBUG
		std::cerr << "Macchina costruita" << std::endl;
		std::cerr << "\tAlphabet: " << alphabet_ << std::endl;
		std::cerr << "\tStates: " << states_ << std::endl;
		std::cerr << "\tInitial state: " << currentState_ << std::endl;
		std::cerr << "\tInstructions: " << instructions_ << std::endl;
		std::cerr << "\tMemory: " << memory_ << std::endl;
		#endif
	}

void
TuringMachine::setAlphabet(Alphabet_&& alphabet) {
	alphabet_ = alphabet;
}

void
TuringMachine::setInitialState(State_&& initialState) {
	currentState_ = initialState;
}

void 
TuringMachine::setStates(States_&& states) {
	states_ = states;
}

void
TuringMachine::setInstructions(Instructions_&& instructions) {
	instructions_ = instructions;
}

std::pair<TuringMachine::States_::iterator, bool>  
TuringMachine::insertState(const State_& state) {
	return states_.insert(state);
}

std::pair<TuringMachine::Alphabet_::iterator, bool>  
TuringMachine::insertSymbol(const Symbol_& symbol) {
	return alphabet_.insert(symbol);
}

std::pair<TuringMachine::Instructions_::iterator, bool>  
TuringMachine::insertInstruction(const Key_& key, const Value_& value) {
	return instructions_.insert(std::make_pair(key, value));
}

TuringMachine::States_::iterator
TuringMachine::eraseState(const State_& state) {
	for (auto it = states_.begin(); it != states_.end(); it++) 
		if (*it == state)
			return it;
	return states_.end();
}

TuringMachine::Alphabet_::iterator
TuringMachine::eraseSymbol(const Symbol_& symbol) {
	for (auto it = alphabet_.begin(); it != alphabet_.end(); it++) 
		if (*it == symbol)
			return it;
	return alphabet_.end();
}

size_t
TuringMachine::eraseInstruction(Key_& key) {
	return instructions_.erase(key);
}

void
TuringMachine::print_debug() {
	std::cerr << std::endl;
	std::cerr << "Memory: " << memory_ << std::endl;
	std::cerr << "CurrentState: " << currentState_<< std::endl;
	std::cerr << "Head: " << *headPosition_ 
			  << " [index: " << headPosition_ - memory_.begin() << ']' << std::endl;
}

void
TuringMachine::moveHead(const Moves_& direction) {
	if (headPosition_ == memory_.begin() && direction == Moves_::left) {
		memory_.push_front("$");
		--headPosition_;
	} else if (headPosition_+1 == memory_.end() && direction == Moves_::right) {
		memory_.push_back("$");
		++headPosition_;
	} else if (direction == Moves_::left) {
		--headPosition_;
	} else if (direction == Moves_::right) {
		++headPosition_;
	}
}

TuringMachine::Memory_::iterator
TuringMachine::changeState() {
	#define STATE 0
	#define SYMBOL 1
	#define MOVE 2
	Value_ next = instructions_.at({currentState_, *headPosition_});

	#ifdef DEBUG
	print_debug();
	std::cerr << "Next: " << next << std::endl;
	#endif

	currentState_ = std::get<STATE>(next);
	headPosition_= memory_.erase(headPosition_); // now head is in the next position
	headPosition_= memory_.insert(headPosition_, std::get<SYMBOL>(next)); // replace the deleted elem
	moveHead(std::get<MOVE>(next));

	#ifdef DEBUG
	print_debug();
	#endif

	return headPosition_;
}

const TuringMachine::Memory_&
TuringMachine::run() {

	try {
		while (true) {
			changeState();
			++steps;
		}
	}
	catch(std::out_of_range& e) {
		
		#ifdef DEBUG
		std::cerr << "************** Computation over **************" << std::endl;
		#endif
	}

	return memory_;
}
#include <iostream>

#include "../include/turing_machine.hh"
#include <vector>

void
test_01() {
	std::cerr << "************** TEST 01 **************" << std::endl;

	TuringMachine::Alphabet_ alphabet({"$", "0", "1"});;
	TuringMachine::States_ states({"q0", "q1", "q2", "q3"});

	TuringMachine::Instructions_ instructions;
	TuringMachine::Key_ k("q0", "$");
	TuringMachine::Value_ v({"q1","0",TuringMachine::Moves_::left});
	instructions.insert({k, v});

	k = std::make_pair("q1", "$");
	v = std::make_tuple("q2","$",TuringMachine::Moves_::right);
	instructions.insert({k, v});

	k = std::make_pair("q1", "1");
	v = std::make_tuple("q1","1",TuringMachine::Moves_::right);
	instructions.insert({k, v});

	k = std::make_pair("q1", "0");
	v = std::make_tuple("q1","0",TuringMachine::Moves_::right);
	instructions.insert({k, v});

	k = std::make_pair("q2", "$");
	v = std::make_tuple("q3","$",TuringMachine::Moves_::right);
	instructions.insert({k, v});

	k = std::make_pair("q2", "1");
	v = std::make_tuple("q2","1",TuringMachine::Moves_::right);
	instructions.insert({k, v});

	k = std::make_pair("q2", "0");
	v = std::make_tuple("q2","0",TuringMachine::Moves_::right);
	instructions.insert({k, v});


	std::vector<std::string> mem(10, "$");
	TuringMachine::Memory_ memory(mem.begin(), mem.end());
	memory[0] = "$";
	memory[1] = "1";
	memory[2] = "1";
	memory[3] = "0";
	memory[4] = "1";
	memory[5] = "1";
	memory[6] = "$";


	TuringMachine tm(
		alphabet, states,
		TuringMachine::State_("q0"), instructions,
		memory
	);

	std::cout << memory << std::endl;
	std::cout << tm.run() << std::endl;
}

int main(int argc, char const *argv[]) {
	test_01();

	return 0;
}
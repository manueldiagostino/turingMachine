# turingMachine

An `C++` implementation of the Turing Machine.

## Configuration file

The default filename for the configuration is `config.tm` (_soon an option to change it..._) and it follows this grammar:

|                    |               |                                                                                                                                                                                                                              |
| ------------------ | ------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| _config_file_      | $\rightarrow$ | **{** _tm_elements_ **}**                                                                                                                                                                                                    |
| _tm_elements_      | $\rightarrow$ | _option_ \| _option_ **,** _tm_elements_                                                                                                                                                                                     |
| _option_           | $\rightarrow$ | **instructions = {** _instruction_list_ **}** \|<br/> **alphabet = {** _symbol_list_ **}** \|<br/> **states = {** _symbol_list_ **}** \|<br/> **initial_state = {** _state_ **}** \|<br/> **memory = {** _symbol_list_ **}** |
| _instruction_list_ | $\rightarrow$ | _tuple_ \| _tuple_ __,__ _instruction_list_                                                                                                                                                                                             |
| _symbol_list_      | $\rightarrow$ | _symbol_ \| _symbol_ __,__ _symbol_list_                                                                                                                                                                                     |
| _tuple_            | $\rightarrow$ | __<__ _state_ __,__ _symbol_ __,__ _state_ __,__ _symbol_ __,__ _move_ __>__                                                                                                                                                 |
| _state_            | $\rightarrow$ | _ID_                                                                                                                                                                                                                         |
| _symbol_           | $\rightarrow$ | _ID_                                                                                                                                                                                                                         |
| _ID_               | $\rightarrow$ | _Char_ __,__ _CharID_                                                                                                                                                                                                        |
| _Char_             | $\rightarrow$ | __a__ \| __b__ \| $\ldots$ \| __z__ \|__0__ \| __1__ \| $\ldots$ \| __9__ \| __$__ \| __%__ \| __#__ \| __&__                                                                                                                |
| _move_             | $\rightarrow$ | __L__ \| __R__ \| __l__ \| __r__                                                                                                                                                                                             |

where terminals are in __bold__ font and nonterminals in _italics_. Vertical bars `|` separate alternatives. 
Keywords `instructions`, `alphabet`, `states`, `initial_state`, `memory` are, at the moment, __case sensitive__. 

### `config.tm` example
```
{
	instructions = {		
		<q0,$,q1,0,L>,
		<q1,$,q2,$,R>,
		<q1,1,q1,1,R>,
		<q1,0,q1,0,R>,
		<q2,$,q3,$,R>,
		<q2,1,q2,1,R>,
		<q2,0,q2,0,R>
	},

	alphabet = {
		$,0,1
	},

	memory = {
		$,1,1,0,1,1,$
	},

	states = {
		q0,q1,q2,q3
	},

	initial_state = {
		q0
	}
}
```

## Usage
After writing the config file, you have to create a `TouringMachine tm` object, a `ConfigLoader cl` and call the function 
```
void loadConfig(TuringMachine& turingMachine, const std::string& fileName);
``` 
then simply 
```
std::cerr << tm.getMemory() << std::endl;
tm.run();
std::cerr << tm.getMemory() << std::endl;
```
 to print the output.


## How to compile
Inside the `test` folder type:
```
g++ -Wall -Wextra -I../include ../src/configLoader.cpp ../src/turingMachine.cpp <your_file>.cpp
```
or
```
g++ -DDEBUG -Wall -Wextra -I../include ../src/configLoader.cpp ../src/turingMachine.cpp <your_file>.cpp
```
to get a step-by-step computation.
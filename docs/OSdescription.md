# Operating System and Code Execution

## Task Resources
The microcontroller's dynamic memory can store 2048 bytes of data. Let's say half of that is used by the operating system itself, then 1024 bytes can be used as user memory for programs. I want programs to be able to call other programs, to use them as functions for example. Programs will be able to allocate chunks of memory within these 1024 available bytes. 

## Task Abilities
Programs will be able to use the kernel for certain operations using system calls : 
* Input using buttons
* Output using the screen display
* Input/Output using on-board pins
* File access
* Execution of other programs

## Emulated Programming Language

Programming language is based on *Instruction List* (IL)
**General instructions** are written in **uppercase** whereas **system calls** are written in **lowercase**.

R : Register Address
I : Integer

### General Instructions


#### Load and Store Instructions

|Keyword | Description |
|--------|-------------|
|`LD I/R`| Load value into `accumulator` (`acc`)
|`ST R`  | Store `acc` value into specified register

#### Logic Instructions

|Keyword | Description |
|--------|-------------|
|`AND`| Perform an AND operation with `acc`
|`OR`| Perform an OR operation with `acc`
|`NOT`| Perform a NOT operation with `acc`
|`XOR`| Perform an XOR operation with `acc`

#### Arithmetic Instructions

|Keyword | Description |
|--------|-------------|
|`ADD I/R`| Add value to `acc`
|`SUB I/R`| Substract value from `acc`
|`MUL I/R`| Muliply value with `acc`
|`DIV I/R`| Divide value by `acc` (integer)
|`MOD I/R`| Return the remainder of the division of `acc` by `I/R`

#### Comparison Instructions

|Keyword | Description |
|--------|-------------|
|`GT I/R`| Test if `acc` is greater than `I/R`
|`EQ I/R`| Test if `acc` is equal to `I/R`
|`LT I/R`| Test if `acc` is lesser than `I/R`

#### Jump Instructions

|Keyword | Description |
|--------|-------------|
|`JMP L` | Jump to label `L`
|`JMPC L`| Jump if test is `TRUE` 
|`JMPNC L`| Jump if test is `FALSE`

### System calls :

#### General

|Keyword  |Description |
|---------|------------|
|`slp I/R`  |sleep for a specified amount of time (I milliseconds)
|`ext`		|exit program

#### Screen Display

|Keyword  |Description |
|---------|------------|
|`pch I1 I2`|print char `I1` on the screen at slot `I2`
|`pst I1 I2`|print string of length `I2` stored in memory at `I1` 

#### Button Input

|Keyword  |Description |
|---------|------------|
|`gch`		|get raw input
|`tch R`	|translate raw input to char
|`gst I1 I2`		|get string of size `I2` and write at address `I1`

#### GPIO

|Keyword  |Description |
|---------|------------|
|`rpn I`	|read pin `I`
|`wpn I`  |write pin `I`

#### File system

|Keyword  |Description |
|---------|------------|
|`opn STR`|open file at path `STR`
|`clo`		|close file 
|`ldd I/R I`|load next `I/R` bytes of data from current file onto memory at adress `I`
|`opp STR`|prepare execution of program at path `STR` by allocating its memory. Values can be entered before its execution
|`exe`    |execute loaded program
|`pss`    |end current program then execute loaded program
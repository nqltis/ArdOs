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

Programming language is based on *Instruction List* (IL).
**General instructions** are written in **uppercase** whereas **system calls** are written in **lowercase**.

R : Register Address
I : Integer

### General Instructions


#### Load and Store Instructions

|Code |Keyword | Description |
|-----|--------|-------------|
|`1`  |`LD I/R`| Load value into `accumulator` (`acc`)
|`2`  |`ST R`  | Store `acc` value into specified register

#### Logic Instructions

|Code |Keyword | Description |
|-----|--------|-------------|
|`3`  |`AND`| Perform an AND operation with `acc`
|`4`  |`OR` | Perform an OR operation with `acc`
|`5`  |`NOT`| Perform a NOT operation with `acc`
|`6`  |`XOR`| Perform an XOR operation with `acc`

#### Arithmetic Instructions

|Code |Keyword | Description |
|-----|--------|-------------|
|`7`  |`ADD I/R`| Add value to `acc`
|`8`  |`SUB I/R`| Substract value from `acc`
|`9`  |`MUL I/R`| Muliply value with `acc`
|`10` |`DIV I/R`| Divide value by `acc` (integer)
|`11` |`MOD I/R`| Return the remainder of the division of `acc` by `I/R`

#### Comparison Instructions

|Code |Keyword | Description |
|-----|--------|-------------|
|`12` |`GT I/R`| Test if `acc` is greater than `I/R`
|`13` |`EQ I/R`| Test if `acc` is equal to `I/R`
|`14` |`LT I/R`| Test if `acc` is lesser than `I/R`

#### Jump Instructions

|Code |Keyword | Description |
|-----|--------|-------------|
|`15` |`JMP L` | Jump to label `L`
|`16` |`JMPC L`| Jump if test is `TRUE` 
|`17` |`JMPNC L`| Jump if test is `FALSE`

### System calls :

#### General

|Code |Keyword  |Description |
|-----|---------|------------|
|`-1`|`ext`		|exit program
|`-2`|`slp I/R`  |sleep for a specified amount of time (I milliseconds)

#### Screen Display

|Code |Keyword  |Description |
|-----|---------|------------|
|`-3`|`pch I1 I2`|print char `I1` on the screen at slot `I2`
|`-4`|`pst I1 I2`|print string of length `I2` stored in memory at `I1` 

#### Button Input

|Code |Keyword  |Description |
|-----|---------|------------|
|`-5`|`gch`		|get raw input
|`-6`|`tch R`	|translate raw input to char
|`-7`|`gst I1 I2`		|get string of size `I2` and write at address `I1`

#### GPIO

|Code |Keyword  |Description |
|-----|---------|------------|
|`-8`|`rpn I`	|read pin `I`
|`-9`|`wpn I`  |write pin `I`

#### File system

|Code |Keyword  |Description |
|-----|---------|------------|
|`-10`|`opn STR`|open file at path `STR`
|`-11`|`clo`		|close file 
|`-12`|`ldd I/R I`|load next `I/R` bytes of data from current file onto memory at adress `I`
|`-13`|`opp STR`|prepare execution of program at path `STR` by allocating its memory. Values can be entered before its execution
|`-14`|`exe`    |execute loaded program
|`-15`|`pss`    |end current program then execute loaded program
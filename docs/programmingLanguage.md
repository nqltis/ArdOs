# Emulated Programming Language

Programming language is based on Instruction List (IL)


|Keyword | Description |
|--------|-------------|
|`LD I/R`| Load value into `accumulator` (`acc`)
|`ST R`  | Store `acc` value into specified register

|Keyword | Description |
|--------|-------------|
|`AND`| Perform an AND operation with `acc`
|`OR`| Perform an OR operation with `acc`
|`NOT`| Perform a NOT operation with `acc`
|`XOR`| Perform an XOR operation with `acc`

|Keyword | Description |
|--------|-------------|
|`ADD I/R`| Add value to `acc`
|`SUB I/R`| Substract value from `acc`
|`MUL I/R`| Muliply value with `acc`
|`DIV I/R`| Divide value by `acc` (integer)
|`MOD I/R`| Return the remainder of the division of `acc` by `I/R`

|Keyword | Description |
|--------|-------------|
|`GT I/R`| Test if `acc` is greater than `I/R`
|`EQ I/R`| Test if `acc` is equal to `I/R`
|`LT I/R`| Test if `acc` is lesser than `I/R`

|Keyword | Description |
|--------|-------------|
|`JMP L` | Jump to label `L`
|`JMPC L`| Jump if test is `TRUE` 
|`JMPNC L`| Jump if test is `FALSE`
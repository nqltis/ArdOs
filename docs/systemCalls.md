# System calls :

R : Register Address
I : Integer
#### General

|Keyword  |Description |
|---------|------------|
|`slp I`  |sleep for a specified amount of time (I milliseconds)
|`ext`		|exit program

#
#### Screen Display

|Keyword  |Description |
|---------|------------|
|`pch I1 I2`|print char `I1` on the screen at slot `I2`
|`pst I1 I2`|print string of length `I2` stored in memory at `I1` 

#
#### Button Input

|Keyword  |Description |
|---------|------------|
|`gch`		|get raw input
|`tch R`	|translate raw input to char
|`gst I1 I2`		|get string of size `I2` and write at address `I1`

#
#### GPIO

|Keyword  |Description |
|---------|------------|
|`rpn I`	|read pin `I`
|`wpn I`  |write pin `I`

#
#### File system

|Keyword  |Description |
|---------|------------|
|`opn STR`|open file at path `STR`
|`clo`		|close file 
|`lod I/R I`|load next `I/R` bytes of data from current file onto memory at adress `I`
|`exe STR`|execute program at path `STR`
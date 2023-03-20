# Data Organization

This file system aims to be very simple and lightweight. There is no file owner, no timestamp, no permissions, just a file name and pointers to related files (see #File Hierarchy). 

The memory is split into three areas : 
 - File headers
 - Block map
 - Block area

## File headers

The file headers are currently 6-bytes wide. Here are the meaning of each of these bytes : 

- 1 : File flags
- 2 : Data Pointer
- 3 : Parent File Pointer
- 4 : Child File Pointer
- 5 : Previous File Pointer
- 6 : Next File Pointer

### File Flags

The most significant bit of the flags byte is used to mark the file header as free (0) or reserved (1). The second bit marks directories. The third bit marks executables. Other bits do not have a purpose yet.

### Data Pointer

This byte represents a data block ID (See ##Data Blocks). A file only points to one data block, the others are chained together from this first data block. The file name is located in the first data block of a file, so even directories have an allocated data block.

### Related Files Pointers

The other bytes are pointers to `parent`, `child`, `previous` and `next` files (See #File Hierarchy), specifically pointers to their file headers. A file header pointer corresponds to the target file's ID. That means the file header of ID 13 is at memory address 6*13 (13th 6-byte header)

## Block Map

The block map keeps track of available and reserved data blocks. Each bit of the block map represents a data block. 1 is reserved and 0 is free.

## Block Area

The Block Area contains the data blocks, which stores files' data. The first two bytes of the block store previous block's and next block's ID. A null ID is considered not linked to another block. This is not a problem because the block of ID 0 is reserved for the root directory.

File names are stored in the first data block as the first string of the file (until the first null byte).

# File hierarchy

To easily implement the concept of file hierarchy. Each file/directory may have four related files :
```
          +------+
          |Parent|
          | File |
          +------+
              |
+------+  +-------+  +------+
| Prev |__|Current|__| Next |
| File |  | File  |  | File |
+------+  +-------+  +------+
              |
          +------+
          |Child |
          | File |
          +------+
```
Note : A file normaly cannot be related to four other files at once.

A directory registers only one of the file it contains. This file is registered as the directory's child file. All other files in the directory are chained together with the next/previous file relations. They do however all have the directory as their parent file.

## Example :
```
Usual representation :

dir/
 +---dir2/
 |    +---file4
 |    \---file5
 +---file1
 +---file2
 \---file3

Headers links :

+------+
| dir  |
+------+     dir        dir        dir
    |         |          |          |
+------+  +-------+  +-------+  +-------+
| dir2 |--| file1 |--| file2 |--| file3 |
+------+  +-------+  +-------+  +-------+
    |
    |        dir2
    |         |
+------+  +-------+
| dir2 |--| file1 |
+------+  +-------+
```
Note that this representation can be used 'illegaly' to implement simlinks for example.

## Root directory and invalid IDs
When a file do not have a child, next or previous file, the pointer is 0. The header with ID 0 does exist and stores the root directory. This is not a problem because the root directory cannot be a next, previous or child file of any file. It can however be a parent file, but every file has to have a parent file so this is not a problem.

# File operations

When a file is opened using `open()`, `index` and `block` attributes are initialized. 

The `read()` function returns the byte pointed by `index` and then increments `index`. If end of file is reached, `index` is not incremented.

The `write()` function writes the byte given as argument into memory at the address pointed by `index` and then increments `index`. If end of file is reached, a new block is reserved.


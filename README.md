# PSU CMPSC 311 Assignments Combination
<b>This is the combination of cmpsc 311 assignments in PSU.</b>

## Assignment #2 - CRUD Device Driver
### Overview
All remaining assignments for this class are based upon you providing an easily used set of functionsso that an application that uses your code can easily talk to an external block storage device like a hard drive(HDD). This device already has its own pre-defined set of functions that allows communication with it.However, they are tedious to use (a common problem with hardware) and not as abstracted as programmerswould prefer. Thus, we are going to translate them to mimic the standard C file commands (open, close, read,write, and seek) so that communicating to the device is easier for others. In other words, your application isacting like a driver for the HDD device. Our functions are called hdd_open, hdd_write, hdd_read, hdd_seek,and hdd_close.<br>
<div align=center><img src=https://github.com/Ca11me1ce/Image-Repo/blob/master/cmpsc311_images/QQ%E6%88%AA%E5%9B%BE20180611153731.png>
</div><br>


### What is the block storage device?
The block storage device does not actually exist. It is a virtual device (modeled in code). Pretend the virtual device is some external HDD for your own understanding. The block storage device stores variable sized blocks of data (termed blocks), which your application will write to in this project. A common misconception is that you can just write data directly to a storage device. This is not true. For memory fragmentation and other reasons, writing to a normal disk drive must be done by writing to blocks. Initially, there are no available blocks. Thus, there is nowhere to write data to on the device. However, you can specify the creation of a block and its size. Once a block exists, you can write data to it and then read the data it holds(or update the data if desired). A unique integer value references each block and is automatically assigned to it during its creation.

How you create blocks, and read/write to them will be discussed shortly, but first understand the following: the point of this project is so that your coded functions (hdd_open, hdd_read, hdd_write, etc.) abstract away the complexity of having to deal with the device’s block storage directly. Within your functions you will have to communicate to the blocks in a very specific way, but whoever uses your functions Page 1 of 7will have an easier time using the HDD device, because your functions resemble the standard open, read, write, etc. C functions. (i.e. you are creating an Application Programming Interface). Note, a common misconception is that you are supposed to use standard C functions, but you are NOT meant to use the standard open, read, write, C functions. Your functions just RESEMBLE them. It is highly recommended you know how those C functions are used so you are familiar with what we are trying to emulate.


### The functions you NEED to write (i.e., the filesystem driver)
In this assignment, another application (a.k.a., the unit-tests provided to you) will call your code to evaluate your application by trying to save, read, and write files to the block storage and validate your results. Unit-testing will use your functions to do so, hence do not change function names (you will be penalized). For this assignment (HINT: but not future ones, so plan ahead), there will only ever be one file open at a time, and furthermore, each file that the user wants to write to the device will always fit within the maximum possible size of a block (HDD_MAX_BLOCK_SIZE). Thus, in this assignment, there is a one-to-one mapping of files to blocks.

When your code is called, hdd_open will be the first function called and will be given a filename. Your function code must return a number, also called a file handle, which will uniquely refer to that particular file across all your other functions. For instance, when the user wants to write something to a file on the device, your hdd_write function will be called with the first parameter being the corresponding file handle you previously returned in hdd_open. If the user tries to call any of your functions (besides hdd_open) with a file that is not open, you must return -1 (i.e. unsuccessful) and handle appropriately. You must keep track of whether or not a file is open using your own data structure.

You can assume that when a user opens a file for the first time via hdd_open, that there is no pre- existing data in the block storage about that file. The application will need to call hdd_write in order to write anything about this file for the first time to a block. Then the application will need to call hdd_read to get back any data that has been written to that file. When the application has finished using a file (i.e. read/write), it calls hdd_close. For this assignment, calling hdd_close will delete all the contents of the file in the block storage (HINT: future assignments will keep contents). Lastly, where the application starts reading from and writing to in a file is determined by the current seek position. The seek position is automatically placed at the end of the last read/write operation. The hdd_seek function places the seek position wherever the user would like within the file (again, it is recommended to understand the standard seek C function).

As a programmer, it is up to you to decide how to implement these functions. However, the functions must maintain the file contents in exactly the same way as a normal filesystem would (meaning do not re- order the bytes of data). The functions that you are to implement are declared in hdd_file_io.h and should be implemented in hdd_file_io.c as follows:

|Function|Description|
|---|---
|hdd_open|[1] This call opens a file (i.e., sets any initially needed metadata in your data structure) and returns a UNIQUE integer file handle (to be assigned by you).<br>[2] For this assignment, the file can be assumed to be non-existent on the device.<br>[3] You should initialize the device here (but only the first time this function is called i.e. singleton pattern).<br>[4] The function returns -1 on failure and UNIQUE integer on success.|
|hdd_close|[1] This call closes the file referenced by the file handle.<br>[2] For this assignment, you are to delete all contents stored in the device’s blocksassociated with this file when it is closed.<br>[3] The function returns -1 on failure and 0 on success.|
|hdd_read|[1] This call reads a count number of bytes from the current position in the file and places them into the buffer called data.<br>[2] The function returns -1 on failure or the number of bytes read if successful.<br>[3] If there are not enough bytes to fulfill the read request, it should only read as many bytes that are available and return the number of bytes read.|
|hdd_write|[1] This call writes a count number of bytes at the current position in the file associated with the file handle fh from the buffer data.<br>[2] The function returns -1 on failure, or the number of written read if successful.<br>[3] When number of bytes to written extends beyond the size of the block, a new block of a larger size should be created to hold the file.|
|hdd_seek|[1] This call changes the current seek position of the file associated with the file handle fh to the position loc.<br>[2] The function returns -1 on failure (like seeking out range of the file) and 0 on success.|

A central constraint to be enforced on your code is that you cannot maintain any file content or length information in any data structures after your functions have returned–all such data must be stored by the device and its blocks. Assumptions you can make for this assignment:<br>
* No file will become larger than the maximum block size (HDD_MAX_BLOCK_SIZE).<br>
* Your program will never have more than one file open at a time for this assignment.<br>


### How to communicate with the device?
In order to communicate with the device, there are four functions:
```c
int32_t hdd_initialize();
int32_t hdd_read_block_size(HddBlockID bid);
int32_t hdd_delete_block(HddBlockID bid);
HddBitResp hdd_data_lane(HddBitCmd command, void * data);
```
You will not be able to see the internals of these functions (they are stored in a static library (.a) provided to you), but you can see the function declarations in the hdd_driver.h file. The first three (relatively simpler) functions are described below:

|Function|Description|
|---|---
|hdd_initialize|This must be called only once throughout the entire program execution called (i.e. singleton pattern) and be called before any of the other three functions. This function initializes the device for communication. It returns 1 on success and -1 on failure.|
|hdd_read_block_size|This function expects a block ID and returns its block size (in bytes). You must read the size of a block that exists (i.e., one you have already created) or it will return an error if the block does not exist. This function returns the block length on success and -1 on failure.|
|hdd_delete_block|This function requires a block ID and deletes it and all data associated with it on the device. You must delete a block that exists or it will return an error if the block does not exist. This function returns 1 on success and -1 on failure. Note that a deleted block’s ID may be recycled again for use.|

The hdd_data_lane function, the most complicated function of the four has been designed for your application to transfer data to and from the device. The function allows you to create a block (and give it data), read data from a block, and overwrite a block with new data. First examine the HddBitCmd parameter and the HddBitResp return value. They are defined in the hdd_driver.h file as:
```c
typedef uint64_t HddBitCmd;
typedef uint64_t HddBitResp;
```
They are both just 64-bits of information. DO NOT THINK THEY ARE MEANT TO BE INTERPRETED AS 64-BIT INTEGERS EVEN THOUGHT THEIR TYPES ARE 64-BIT INTEGERS. Yes, these parameters are of 64-bit integer type, but in the hdd_data_lane function, those 64-bits are taken in as a word and divided into sections (using bitwise operators) that each have their own meanings (figure below). The lower 32 bits represent a parameter called Block ID, the next bit represents a parameter called R, and so on. See the below figure:
<div align=center><img src=https://github.com/Ca11me1ce/Image-Repo/blob/master/cmpsc311_images/64bit_block.png>
</div><br>

These subdivisions can be thought of as a compact way for one function to have many parameters within just one parameter. Essentially, the HddBitCmd parameter allows you to specify a block to transfer data to or from, and that data is pointed to by the data parameter, which points to either the actual data to transmit or to where the received data should be stored. How the data parameter is used depends on the HddBitCmd’s fields. The exact meanings of the parameter, HddBitCmd, and the returned value, HddBitResp, are listed below:

* Block ID<br>
  * In HddBitCmd: This is the block identifier of the block you are executing a command on. If the block does not yet exist, when trying to create one, leave this field as all 0s.<br>
  * In HddBitResp: hdd_data_lane returns the created block’s id if HddBitCmd’s Op field was HDD_BLOCK_CREATE (otherwise, it returns the same block ID you gave in the HddBitCmd).<br>

* Op - Opcode<br>
  * In HddBitCmd: This is the request type of the command you are trying to execute. The value can be one of HDD_BLOCK_CREATE , HDD_BLOCK_READ , or HDD_BLOCK_OVERWRITE (see next section for meaning of these).<br>
  * In HddBitResp: It will be the same Op as what you sent in HddBitCmd (thus, not useful).<br>

* Block Size<br>
  * In HddBitCmd: This is the length of the block you request to read from, overwrite, or create. This is always the number of bytes in the data parameter that are read from and written to.<br>
  * In HddBitResp: It will be the same block size as what you sent in HddBitCmd (thus, not useful).<br>

* Flags - These are unused for this assignment (set to 0 in HddBitCmd).<br>

* R- Result code:<br>
  * In HddBitCmd: Not used (set to 0).<br>
  * In HddBitResp: This is the success status of the command execution, where 0 (zero) signifies success, and 1 signifies failure. You must check the success value for each bus operation even though nothing should be failing.<br>

The Op’s values can be found in hdd_driver.h, and below summarizes what they mean to help you understand
how to use them in your application:<br>
* HDD_BLOCK_CREATE - This command creates a block whose size is defined in the Block Size field of the HddBitCmd. The data buffer passed to hdd_data_lane should point to the start location of the data bytes to be transferred. After completion, the data has now been saved to the newly created block on the device. If successful, the operation will return the new block ID in the HddBitResp’s Block ID field.<br>
* HDD_BLOCK_READ - This command reads a block (in its entirety) from the device and copies its contents into the passed data buffer. The Block Size field should be set to the exact size of the block you’re trying to read from (thus, you must read the entire block, not just parts of it). The data buffer should have enough allocated memory to store the entire block.<br>
* HDD_BLOCK_OVERWRITE - This command will overwrite the contents of a block. Note that the block size CAN NEVER change. Thus, the call will fail unless the data buffer sent in is the same size as the original block created. Just like in HDD_BLOCK_CREATE, the data buffer should point to the start location of the data bytes to be transferred.<br>

### General Compilation and Running Instructions
1. From your virtual machine, download the starter source code provided for this assignment. Move the tgz file to your assignment directory.<br>
2. Install this dependency you will need via the terminal:<br>
```bash
% sudo apt-get install libgcrypt11-dev
```
3. Have the terminal open to the directory where the .tgz starter code file is located. Now unpackage the contents of the file:<br>
```bash
% tar xvfz assign2-starter.tgz
```
4. You should be able to use the Makefile provided to build the program without modification thus just compile by typing:<br>
```bash
% make clean; make
```
5. To get started, focus on hdd_file_io.c. This file contains code templates for the functions you MUST fill in as described above.<br>
6. Add appropriate English comments to your functions stating what the code is doing. All code must be correctly (and consistently) indented. Use your favorite text editor to help with this process! You will lose points if you do not comment and indent appropriately. Graders and TAs will need to understand your comments to follow your code and grade. Do not forget to add comments to the Makefile. The Makefile structure has been explained in class.<br>
7. Built into hdd_file_io.c file is a function called hddIOUnitTest, which is automatically called when you run the program to check for its correctness. The main() function simply calls the function hddIOUnitTest . Page 5 of 7If you have implemented your code correctly, it should run to completion successfully. To test the program, you execute the simulated filesystem using the -u and -v options, as:<br>
```bash
./hddsim -u -v
```
If the program completes successfully, the following should be displayed as the last log entry:<br>
<div align=center><b>HDD unit tests completed successfully.</b><br></div>


## Assignment #3 - CRUD Device Driver
### Overview
All remaining assignments for this class are based on the creation and extension of a user-space device driver for a
filesystem built on top of a object storage device. At the highest level, you will translate file system commands into
storage array commands. These file system commands include open, read, write, and close for files that are written
to your file system driver. These operations perform the same as the normal UNIX I/O operations, with the caveat
that they direct file contents to the object storage device instead of the host filesystem.<br>

## Assignment #4 – Network Attached Storage
The last assignment will extend the device driver you implemented in the previous
assignment #3. You will extend your device driver to communicate with the HDD over a
network. You will need to modify code from your previous assignment. While a superficial
reading of this assignment made lead you to believe that this assignment work will be easy, it
is not. Please give yourself ample time to complete the code and debugging. Note that
debugging this assignment will require you to debug your program interacting with a server
program executing in another terminal window, so it can take some time.<br>

### Overview
This assignment is separated into two programs: the client program which you will
complete the code for and the server program which is provided to you. To run the program,
you will run the provided server executable in one terminal window and your compiled client
executable in another. You will just use a local network connection to connect the two
programs running on your machine (i.e., your computer will be both the client and the server—
you’re not actually communicating over the internet or between different computers though
it’s theoretically possible with minor tweaks to this project).<br>

The challenge of this assignment is that you will be sending all parameters of
hdd_data_lane over a network socket (along with receiving responses through that same
socket). In fact, you’ll be replacing hdd_data_lane with your own function that sends the
hdd_data_lane parameters to and from the server. You must start with your hdd_file_io.c
code from assignment #3. The assignment requires you to perform the following steps (after
getting the starter code and copying over needed files from your implementation of the
previous assignment):<br>

1. Slightly modify your hdd_file_io.c to remove 3 functions<br>
2. Minor naming change in hdd_file_io.c and adding an include<br>
3. Bulk of the project: hdd_client_operation<br>

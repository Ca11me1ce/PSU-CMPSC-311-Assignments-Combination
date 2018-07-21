# PSU CMPSC 311 Assignments Combination
<b>This is the combination of cmpsc 311 assignments in PSU.</b>

## Assignment #2 - CRUD Device Driver
### Overview
All remaining assignments for this class are based upon you providing an easily used set of functions so that an application that uses your code can easily talk to an external block storage device like a hard drive(HDD). This device already has its own pre-defined set of functions that allows communication with it. However, they are tedious to use (a common problem with hardware) and not as abstracted as programmerswould prefer. Thus, we are going to translate them to mimic the standard C file commands (open, close, read,write, and seek) so that communicating to the device is easier for others. In other words, your application isacting like a driver for the HDD device. Our functions are called hdd_open, hdd_write, hdd_read, hdd_seek,and hdd_close.<br>
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

You drive will support:<br>
* Tracking multiple open files simultaneously, by using a provided file table<br>
* File data that persists between runs of a program, by implementing the basic filesystem operations of “format,” “mount,” and “unmount”<br>

To help support your implementation, several new features (described in detail in the next section) are provided for you by the HDD interface and libcrud.a:<br>
* Definitions of a file allocation table structure and file table to use in your code<br>
* A special “priority object” in the object store which can be accessed at any time<br>
* New HDD_FORMAT and HDD_CLOSE operations and an enhanced HDD_INIT command to support the format, mount, and unmount operations<br>


### INIT, FORMAT, and CLOSE Commands
The HDD command set has been augmented to allow object contents to be made persistent by saving the data to a file called hdd_content.crd. You do not need to create this file or write to it yourself. Instead, the object store will manipulate the content file when you issue the following requests:<br>
* HDD_INIT: In addition to initializing the object store, this command will now also load any saved state from hdd_content.crd (if that file exists). As in the previous assignment, you must issue an INIT request exactly once, before any other command is sent to the HDD hardware.<br>
* HDD_FORMAT: This new command will delete hdd_content.crd and all objects in the object store (including the priority object).<br>
* HDD_CLOSE: This new command will save the contents of the object store to the hdd_content.crd file, creating it if it does not exist.<br>
These new commands will be useful when implementing the new format, mount, and unmount features.<br>


### Implement three new functions to handle operations involving the file allocation table:
1. hdd_format: Reinitializes the filesystem and creates an empty file allocation table. This func- tion
should perform a normal HDD_INIT followed by a HDD_FORMAT, initialize the file allo- cation
table with zeros (signifying that all slots are unused), and save the table by creating a priority object
containing the table data.
2. hdd_mount: Loads an existing saved filesystem into the object store and file table. This function
should first perform a normal HDD_INIT if it has not already been run. Then it should locate the
file allocation table (by reading the priority object) and copy its contents into the hdd_file_table
structure.
3. hdd_unmount: Saves all changes to the filesystem and object store. This function should store the
current file table back in the storage device by updating the priority object. Once that is done, it should
issue a HDD_CLOSE request to the device, which will write out the persistent state file and shut down
the virtual hardware.


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

### 1. Slightly modify your hdd_file_io.c to remove 3 functions<br>
Before delving into the networking code, this assignment simplifies (in a sense) the API you’ve been using to communicate with the device. Instead of having all of the following functions:<br>
* hdd_data_lane<br>
* hdd_read_block_size<br>
* hdd_delete_block<br>
* hdd_initialize<br>

We’ll now just have one:<br>
* hdd_data_lane<br>

To remove the three old functions, just make the following replacements:<br>
* int hdd_initialize() becomes:<br>
  * HddBitResp hdd_data_lane(HddBitCmd cmd, void * data) where<br>
    * HddBitCmd cmd = all 0s except the op field is HDD_DEVICE and the flags field is HDD_INIT<br>
    * void * data = NULL<br>
    * Just check HddBitResp’s R bit to now determine success or failure of initialization<br>
* hdd_delete_block becomes:<br>
  * HddBitResp hdd_data_lane(HddBitCmd cmd, void * data) where<br>
    * [When deleting any block except the meta block] HddBitCmd cmd = all 0s except the op field is HDD_DELETE and the block ID field is the block you’d like to delete<br>
    * [When deleting the meta block] HddBitCmd cmd = all 0s except the op field is HDD_DELETE and the flags field is HDD_META_BLOCK<br>
    * void * data = NULL<br>
    * Just check HddBitResp’s R bit for success or failure<br>
* hdd_read_block_size becomes<br>
  * Nothing. You can finally save the size of a block.<br>

Note your other hdd_data_lane calls don’t need changing unless you’ve been manually typing in 3 for your op code instead of HDD_DEVICE. If you have been, simply replace all occurrences of 3 with HDD_DEVICE.<br>


### 2. Minor naming change in hdd_file_io.c and adding an include<br>
We’re now going to replace all hdd_data_lane function calls with the name hdd_client_operation. The hdd_client_operation function is defined in a new file hdd_client.c as follows:<br>
```c
HddBitResp hdd_client_operation(HddBitCmd cmd, void * data);
```
You can see it accepts and returns the same values as hdd_data_lane. Abstractly, this function performs the same function as the original hdd_data_lane function it is replacing. What it does specifically will be explained in the next section. For now, just make the name change and include hdd_network.h in your hdd_file_io.c file.<br>


### 3. Bulk of the project: hdd_client_operation<br>
The remainder of the assignment is your implementation of the hdd_client_operation function in the hdd_client.c file. The idea is that you will coordinate with a server via a network protocol to transfer the commands and data from the client to the server. The server will execute the commands and modify the HDD storage accordingly. Thus, the client code you’re writing is just a messenger for what used to be hdd_data_lane commands.<br>

The first time you want to send a message to the server, you have to connect. Connect to address HDD_DEFAULT_IP and port HDD_DEFAULT_PORT, both of which are defined in the hdd_network.h file.

To transfer the the HddBitCmd commands, you send the 64-bit request values over the network and receive the 64-bit response values. Note that you need to convert the 64 bit- values into network byte order before sending them and converting them to host byte order when receiving them. The functions htonll64 and ntohll64 are used to perform these conversions respectively and are provided for you in the cmpsc311_util.h file.

Note that extra data needs to be sent or received for certain HddBitCmd’s (i.e. when reading from a block or writing to a block), but not for all of them. See Table 1 for information about which requests should send and receive buffers.

|Op Field of HddBitCmd|Flags Field of HddBitCmd|What You Send|What You Receive|
|---|---|---|---
|HDD_DEVICE|HDD_INIT<br>HDD_FORMAT<br>HDD_SAVE_AND_CLOSE|HddBitCmd (only)|HddBitResp (only)|
|HDD_BLOCK_CREATE|HDD_NULL_FLAG<br>HDD_META_BLOCK|HddBitCmd and bytes of block|HddBitResp (only)|
|HDD_BLOCK_OVERWRITE|HDD_NULL_FLAG<br>HDD_META_BLOCK|HddBitCmd and bytes of block|HddBitResp (only)|
|HDD_BLOCK_READ|HDD_NULL_FLAG<br>HDD_META_BLOCK|HddBitCmd (only)|HddBitResp and bytes of block|
|HDD_BLOCK_DELETE|HDD_NULL_FLAG<br>HDD_META_BLOCK|HddBitCmd (only)|HddBitResp (only)|

<div align=center><b>Table 1: HDD request messages: data sent and received with each HddBitCmd and flag pair</b></div><br>


On sends that require sending a buffer, you simply send the buffer immediately after the 64-bit value. On receives, you first receive the 64-bit response value, convert it to host byte order, and extract the op type and block size. If the command requires a buffer be received, then you should receive data of length equal to that returned in the HddBitResp’s block size.<br>

Note that the last thing you should do in your client program is after a HDD_SAVE_AND_CLOSE op type and is to disconnect from the server (i.e., close the socket).<br>

To assist with connecting to the network and sending/receiving data, see the network slides posted on Canvas under assignment #4. Read them all, but specifically see Slide 34 for an overview and slide 49 for sample code demonstrating how you’d connect and send/receive bytes.<br>


### Testing
<b>[Note: please run “sudo apt-get install libgcrypt20-dev” before testing. Probably won’t matter for most of you but it might for a few others]</b><br>
Overall, the testing is very similar to the previous assignment. One of the differences is that you’ll need to open two windows and execute the client in one and the server in the other. To run the server, use the command:<br>
```bash
./hdd_server –v
```
The server will run continuously without being restarted (unless you send the server something
unexpected that causes it to halt). That is, the client can run several workloads by connecting to
the server and sending commands. To run the client, use the command:<br>
```bash
./hdd_client -v <test arguments>
```
The first phase of testing the program is performed by using the unit test function for the hdd_file_io interface. The main function provided to you simply calls the function hddIOUnitTest. As before it’s with the following flags:<br>
```bash
./hdd_client -u –v
```
If the program completes successfully, the following should be displayed as the last log entry:<br>
<div align=center><b>HDD unit tests completed successfully.</b></div><br>

The second phase of testing will run workloads just like before, but now there’s three. These will test the mounting and unmounting of just like before and will save the state of the block storage to hdd_conent.svd. Make sure you run these commands sequentially as seen below:<br>
```bash
./hdd_client -v workload-one.txt
./hdd_client -v workload-two.txt
./hdd_client -v workload-three.txt
```
If the program completes successfully, the following should be displayed as the last log entry for each workload:<br>
<div align=center><b>HDD simulation completed successfully.</b></div><br>

The last phase of testing will extract the saved files from the filesystem. To do this, you will use the –x option:<br>
```bash
./hdd_client -v -x simple.txt
```

This should extract the file simple.txt from the device and write it to your current directory. Next, use the diff command to compare the contents of the file with the original version simple.txt.orig distributed with the original code:<br>
```bash
diff simple.txt simple.txt.orig
```

If they are identical, diff will give no output (i.e, no differences). Repeat these commands to extract and compare the content for the files raven.txt, hamlet.txt, penn-state-alma- mater.txt, firecracker.txt, and solitude.txt, and o44.txt (note the o44.txt is new). Just like before don’t try to extract the same file twice without first deleting the newly created .txt file.<br>

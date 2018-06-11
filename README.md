# PSU CMPSC 311 Assignments Combination
This is the combination of cmpsc 311 assignments in PSU.

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

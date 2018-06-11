# PSU CMPSC 311 Assignments Combination
This is the combination of cmpsc 311 assignments in PSU.

## Assignment #2 - CRUD Device Driver
### Overview
All remaining assignments for this class are based upon you providing an easily used set of functions
so that an application that uses your code can easily talk to an external block storage device like a hard drive
(HDD). This device already has its own pre-defined set of functions that allows communication with it.
However, they are tedious to use (a common problem with hardware) and not as abstracted as programmers
would prefer. Thus, we are going to translate them to mimic the standard C file commands (open, close, read,
write, and seek) so that communicating to the device is easier for others. In other words, your application is
acting like a driver for the HDD device. Our functions are called hdd_open, hdd_write, hdd_read, hdd_seek,
and hdd_close.<br>
<div align=center><img src=https://github.com/Ca11me1ce/Image-Repo/blob/master/cmpsc311_images/QQ%E6%88%AA%E5%9B%BE20180611153731.png>
</div>

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

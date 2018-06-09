# PSU CMPSC 311 Assignments Combination
This is the combination of cmpsc 311 assignments in PSU.

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

#
# CMPSC311 - Fall 2017
# Assignment #4 Makefile
#

# Variables
CC=gcc  #set program for compiling C programs
LINK=gcc#set the link way to gcc compiler
CFLAGS=-c -Wall -I. -fpic -g#Variable->INCLUDES, Value-> -I. {local directory}
LINKFLAGS=-L. -g#link to depfile
LINKLIBS=-lcrud -lgcrypt #link to the library

# Files to build
#compile hdd_client_objfiles

HDD_CLIENT_OBJFILES=   hdd_sim.o \
                        hdd_file_io.o  \
                        hdd_client.o \
                    
TARGETS=    hdd_client#target is producing executable file
             
                    
# Suffix rules
#add the extension to the object files
.SUFFIXES: .c .o

#file of different extention with different operation
.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions
#make all the top_level targets the makefile knows about
all : $(TARGETS) 
    
#gcc the c source file to produce object file
hdd_client: $(HDD_CLIENT_OBJFILES)
	$(LINK) $(LINKFLAGS) -o $@ $(HDD_CLIENT_OBJFILES) $(LINKLIBS) 

# Cleanup 
clean:
	rm -f $(TARGETS) $(HDD_CLIENT_OBJFILES)

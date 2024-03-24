# Start of make file

# User interface 
stdfs : stdfs.o filesystem.o disk.o
	g++ -o stdfs stdfs.o filesystem.o disk.o

# stdfs object
stdfs.o : stdfs.cpp filesystem.h
	g++ -c stdfs.cpp 

# FileSystem object
filesystem.o : filesystem.cpp filesystem.h
	g++ -c filesystem.cpp

# Disk object
disk.o : disk.c disk.h
	g++ -c disk.c

# Clean action
clean : 
	rm -f *.o stdfs.out stdfs

# End of Make file

#stdfs.input ultimate test case
#------------------------------
# CLEAR

# OPEN a1 w x3df
# WRITE 0 26 #This is test file a1#

# CLOSE 0
# CAT a1 x3df
# OPEN a1 r x3dd
# OPEN a1 r x3df
# READ 0 10

# LSEEK 0 10 0
# READ 0 10
# READ 0 20
# LSEEK 0 0 2
# WRITE 0 10 # More Junk again#

# CLOSE 0
# CAT a1 x3aa
# CAT a1 x3df
#
 # File name:
 # Makefile
 # -------------------------------------------------
 # Line Counter ( version 1.0 ) (Available for Linux and Mac OS)
 # by :
 #		Amin Aghaee
 #      http://ce.sharif.edu/~aghaee/
 # Sharif University of Technology
 # Computer engineering department
 # -------------------------------------------------
#

CC=g++

all:	main

main:	line_cnt.cpp
		$(CC) line_cnt.cpp -o LCounter

clean:	
		rm LCounter

PHONY:	all
		clean
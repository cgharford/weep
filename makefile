# Copyright 2015, University of North Carolina
# Author: Nathan Otterness (otternes@cs.unc.edu)
#
# This makefile is responsible for building the weep executable.
.PHONY: all clean

all: weep

weep: weep.o ptw_searcher.o
	g++ -O3 -o weep ptw.o weep.o ptw_searcher.o -lpcap

ptw_searcher.o: ptw_searcher.cc ptw_searcher.h
	g++ -c -O3 -Wall -Werror -o ptw_searcher.o ptw_searcher.cc

weep.o: weep.cc rc4.h
	g++ -c -O3 -Wall -Werror -o weep.o weep.cc

clean:
	rm -f weep.o
	rm -f ptw_searcher.o
	rm -f weep


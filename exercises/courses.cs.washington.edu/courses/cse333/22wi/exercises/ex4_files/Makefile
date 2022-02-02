# Copyright ©2022 Justin Hsia.  All rights reserved.  Permission is
# hereby granted to students registered for University of Washington
# CSE 333 for use solely during Winter Quarter 2022 for purposes of
# the course.  No other use, copying, distribution, or modification
# is permitted without prior written consent. Copyrights for
# third-party components of this work must be honored.  Instructors
# interested in reusing these course materials should contact the
# author.

# NOTE: This Makefile purposely uses explicit commands for transparency
# into how we compile and use modules in testing and client code.
# Future Makefiles will make heavy use of automatic variables:
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

# default target
all: test_ro_file ex4

# main executable
ex4: ex4.o ro_file.o
	gcc -Wall -g -std=c17 -o ex4 ex4.o ro_file.o

# sample test
test_ro_file: test_ro_file.o ro_file.o
	gcc -Wall -g -std=c17 -o test_ro_file test_ro_file.o ro_file.o

# individual source files
ro_file.o: ro_file.c ro_file.h
	gcc -Wall -g -std=c17 -c ro_file.c

test_ro_file.o: test_ro_file.c ro_file.h
	gcc -Wall -g -std=c17 -c test_ro_file.c

ex4.o: ex4.c ro_file.h
	gcc -Wall -g -std=c17 -c ex4.c

# phony target - delete built files (including MacOS debug files)
clean:
	rm -rf ex4 test_ro_file *.o *~ *.dSYM

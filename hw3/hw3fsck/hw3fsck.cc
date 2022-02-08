/*
 * Copyright ©2022 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2022 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <cstdlib>   // for EXIT_SUCCESS, EXIT_FAILURE, free().
#include <iostream>  // for std::cout, std::cerr, etc.

#include "./FileIndexChecker.h"

using std::cerr;
using std::endl;
using hw3::FileIndexChecker;

static void Usage(char* prog_name) {
  cerr << "Usage: " << prog_name << " indexfile" << endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  if (argc < 2) Usage(argv[0]);

  // Checking starts here.
  FileIndexChecker fic(argv[1]);
  fic.CheckFile();

  return EXIT_SUCCESS;
}

/*
 * Copyright ©2022 Justin Hsia.  All rights reserved. Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2022 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "ro_file.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define TEST_FAILED -1
#define TEST_SUCCESS 0

// Tries to read a few bytes from test.txt and verifies they are correct.
// Uses ro_open, ro_read, ro_close.
// Returns non-zero value on error.
static int simpleTest();
// More complicated test using test2.txt.
// Uses ro_open, ro_seek, ro_read, ro_tell, ro_close.
// Returns non-zero value on error.
static int advancedTest();

// Runs simpleTest and advancedTest to test implemented functions in ro_file.c
int main() {
  if (simpleTest()) {
    return EXIT_FAILURE;
  }
  if (advancedTest()) {
    return EXIT_FAILURE;
  }
  printf("All tests passed\n");
  return EXIT_SUCCESS;
}

int simpleTest() {
  RO_FILE* f = ro_open("test.txt");
  if (f == NULL) {
    fprintf(stderr, "error opening file: %s\n", "test.txt");
    return TEST_FAILED;
  }
  char str[15];
  ro_read(str, 14, f);
  str[14] = '\0';

  char* expected = "Hi, my name is";
  if (strcmp(str, expected)) {
    fprintf(stderr, "incorrect string read\n\texpected: %s\n\tactual: %s\n",
            expected, str);
    ro_close(f);
    return TEST_FAILED;
  }

  ro_close(f);
  return TEST_SUCCESS;
}

int advancedTest() {
  RO_FILE* f = ro_open("test2.txt");
  if (f == NULL) {
    fprintf(stderr, "error opening file: %s\n", "test2.txt");
    return TEST_FAILED;
  }
  char str[2];
  char expected[2];
  int offset = TEST_SUCCESS;

  while (offset != 94) {
    if (ro_seek(f, offset, SEEK_SET)) {
      fprintf(stderr, "error seeking to pos: %d\n", offset);
      ro_close(f);
      return TEST_FAILED;
    }

    int temp = ro_read(str, 1, f);
    str[1] = '\0';
    if (1 != temp) {
      fprintf(stderr, "error read more than 1 byte, read returned: %d\n", temp);
      ro_close(f);
      return TEST_FAILED;
    }

    expected[0] = (char) (offset + 33);
    expected[1] = '\0';

    if (strcmp(str, expected)) {
      fprintf(stderr, "incorrect string read\n\texpected: %s\n\tactual: %s\n",
              expected, str);
      ro_close(f);
      return TEST_FAILED;
    }

    temp = ro_tell(f);
    if (offset+1 != temp) {
      fprintf(stderr, "incorrect pos from ro_tell\n\texpected: %d\n\tactual: "
              "%d\n", offset+1, temp);
      ro_close(f);
      return TEST_FAILED;
    }

    offset += 2;
  }

  ro_close(f);
  return TEST_SUCCESS;
}

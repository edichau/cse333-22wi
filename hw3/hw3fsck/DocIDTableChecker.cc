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

#include "./DocIDTableChecker.h"

#include <assert.h>     // for assert()
#include <arpa/inet.h>  // for ntohl(), etc.
#include <stdint.h>     // for uint32_t, etc.
#include <string>

extern "C" {
  #include "libhw2/DocTable.h"
}
#include "../Utils.h"
#include "./CheckerUtils.h"

using std::string;

namespace hw3 {

// The constructor for DocIDTableChecker calls the constructor
// of HashTableChecker(), its superclass. The superclass takes
// care of taking ownership of f and using it to extract and
// cache the number of buckets within the table.
DocIDTableChecker::DocIDTableChecker(FILE* f, int32_t offset, int32_t len,
                                     const string& context)
  : HashTableChecker(f, offset, len), context_(context) { }

bool DocIDTableChecker::CheckElement(int32_t element_offset,
                                     int32_t bucket_number) {
  bool result = true;
  // seek to the start of the element
  assert(fseek(file_, element_offset, SEEK_SET) == 0);

  // read the docID
  DocID_t doc_id;
  assert(fread(&doc_id, sizeof(DocID_t), 1, file_) == 1);
  doc_id = ntohll(doc_id);

  // make sure the docID is in the right bucket
  result &= CheckEQ64(bucket_number, doc_id % num_buckets_,
            "[DocID table] DocID % num_buckets == bucket_number");

  // read in the number of positions
  int32_t num_pos;
  assert(fread(&num_pos, sizeof(int32_t), 1, file_) == 1);
  num_pos = ntohl(num_pos);
  result &= CheckLT32(num_pos, 1000000, "[DocID table] num_positions");

  // loop through and check the positions
  int32_t prev_pos;
  for (int32_t i = 0; i < num_pos; i++) {
    int32_t cur_pos;
    assert(fread(&cur_pos, sizeof(int32_t), 1, file_) == 1);
    cur_pos = ntohl(cur_pos);
    if (i > 0) {
      result &= CheckLT32(prev_pos,
                cur_pos,
                "word position[i] < word position[i+1]");
    }
    prev_pos = cur_pos;
  }
  return result;
}
}  // namespace hw3

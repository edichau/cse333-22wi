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

#include "./DocTableChecker.h"

#include <assert.h>     // for assert()
#include <arpa/inet.h>  // for ntohl(), etc.
#include <stdint.h>     // for uint32_t, etc.
#include <string>
#include <cctype>       // for isascii

extern "C" {
  #include "../libhw2/DocTable.h"
}
#include "../Utils.h"  // for ntohll()
#include "./CheckerUtils.h"

using std::string;

namespace hw3 {

// The constructor for DocTableChecker its superclass constructor,
// which takes ownership of f, and uses it to extract and cache
// the number of buckets within the table.
DocTableChecker::DocTableChecker(FILE* f, int32_t offset, int32_t len)
  : HashTableChecker(f, offset, len) { }

bool DocTableChecker::CheckElement(int32_t element_offset,
                                   int32_t bucket_number) {
  bool result = true;
  // Read in the doc_id
  uint64_t doc_id;
  assert(fseek(file_, element_offset, SEEK_SET) == 0);
  assert(fread(&doc_id, sizeof(DocID_t), 1, file_) == 1);
  doc_id = ntohll(doc_id);

  // Make sure the doc_id makes sense for this bucket.
  result &= CheckEQ32(bucket_number, doc_id % num_buckets_,
            "[DocTable] DocID % num_buckets == bucket_number");

  // Read in the file_name length.
  int16_t fn_len;
  assert(fread(&fn_len, sizeof(int16_t), 1, file_) == 1);
  fn_len = ntohs(fn_len);
  CheckLT16(fn_len, 1024, "[DocTable] file_name length");

  // Read in the file_name.
  char file_name[fn_len + 1];
  file_name[fn_len] = '\0';
  assert(fread(&file_name[0], fn_len, 1, file_) == 1);

  // Make sure the file_name is all ASCII.
  for (int i = 0; i < fn_len; i++) {
    result &= CheckEQ16(1, (isascii(file_name[i]) ? 1 : 0),
                        ToString("[DocTable] isascii(file_name)[%d]", i));
  }
  return result;
}

}  // namespace hw3

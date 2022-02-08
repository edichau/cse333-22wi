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

#include "./IndexTableChecker.h"

#include <assert.h>     // for assert().
#include <arpa/inet.h>  // for ntohl(), ntohs().
#include <stdint.h>     // for uint32_t, etc.
#include <cctype>       // for isascii, etc.
#include <sstream>      // for std::stringstream.
#include <string>       // for std::string.

#include "../Utils.h"
#include "./CheckerUtils.h"
#include "./DocIDTableChecker.h"

using std::string;

namespace hw3 {

// The constructor for IndexTableChecker calls the constructor of
// HashTableChecker(), its superclass. The superclass takes care of
// taking ownership of f and using it to extract and cache the number
// of buckets within the table.
IndexTableChecker::IndexTableChecker(FILE* f, int32_t offset, int32_t len)
  : HashTableChecker(f, offset, len) { }

bool IndexTableChecker::CheckElement(int32_t element_offset,
                                     int32_t bucket_number) {
  bool result = true;

  // Read in the word length.
  int16_t word_len;
  assert(fseek(file_, element_offset, SEEK_SET) == 0);
  assert(fread(&word_len, sizeof(int16_t), 1, file_) == 1);
  word_len = ntohs(word_len);
  result &= CheckLT16(word_len, 8192,
            ToString("[IndexTable] unreasonably long word in bucket[%d]",
                     bucket_number));

  // Read in the docID table length.
  int32_t doc_id_table_len;
  assert(fread(&doc_id_table_len, sizeof(int32_t), 1, file_) == 1);
  doc_id_table_len = ntohl(doc_id_table_len);
  result &= CheckLT32(element_offset + kWordLenSize + kDocIDTableLengthSize +
            word_len, offset_ + len_,
            ToString("[IndexTable] element_end < indextable_end in bucket[%d]",
                     bucket_number));

  // Read in the word.
  char word[word_len + 1];
  word[word_len] = '\0';
  assert(fread(&word[0], word_len, 1, file_) == 1);

  // Make sure the word is all ascii and lower case.
  for (int i = 0; i < word_len; i++) {
    result &= CheckEQ16(1, (isascii(word[i]) ? 1 : 0),
              ToString("[IndexTable] isascii(word)[%d] in bucket[%d]",
                       i, bucket_number));
    if (!isalpha(word[i])) {
      continue;
    }
    result &= CheckEQ16(1, (islower(word[i]) ? 1 : 0),
              ToString("[IndexTable] islower(word)[%d] in bucket[%d]",
                     i, bucket_number));
  }

  DocIDTableChecker ditc(FileDup(file_),
                         element_offset + kWordLenSize + kDocIDTableLengthSize
                         + word_len,
                         doc_id_table_len,
                         ToString("DocIDtable for word '%s',"
                                  " embedded in bucket[%d]",
                                  word, bucket_number));
  result &= ditc.Check(element_offset + kWordLenSize + kDocIDTableLengthSize +
             word_len, doc_id_table_len);
  return result;
}

}  // namespace hw3

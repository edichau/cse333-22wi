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

#include "./HashTableChecker.h"

#include <arpa/inet.h>        // for ntohl().
#include <assert.h>           // for assert().
#include <iostream>

#include "../Utils.h"
#include "./CheckerUtils.h"

namespace hw3 {

HashTableChecker::HashTableChecker(FILE* f, int32_t offset, int32_t len)
  : file_(f), offset_(offset), len_(len) { }

HashTableChecker::~HashTableChecker() {
  // fclose our (FILE*).
  fclose(file_);
  file_ = NULL;
}

bool HashTableChecker::Check(int32_t offset, int32_t len) {
  // fread() the number of buckets in this hashtable from its
  // "num_buckets" field.
  bool result = true;

  assert(fseek(file_, offset_, SEEK_SET) == 0);
  assert(fread(&num_buckets_, sizeof(num_buckets_), 1, file_) == 1);

  // Convert the num_buckets_ from network (on-disk) representation to
  // host (in-memory) byte ordering using ntohl().
  num_buckets_ = ntohl(num_buckets_);

  // Check that the number of buckets is reasonable.
  result &= CheckLT32(num_buckets_ * kBucketRecordSize,
                      len, "num_buckets < len(table)");

  // Loop through all of the bucket records, checking each bucket.
  bool found_first_nonempty = false;
  int32_t prev_chain_len, prev_bucket;

  for (int32_t i = 0; i < num_buckets_; i++) {
    int32_t cur_chain_len, cur_bucket;

    // read the bucket records
    assert(fseek(file_, offset_ + kBucketLengthSize + (kBucketRecordSize * i),
                 SEEK_SET) == 0);
    assert(fread(&cur_chain_len, sizeof(int32_t), 1, file_) == 1);
    assert(fread(&cur_bucket, sizeof(int32_t), 1, file_) == 1);
    cur_chain_len = ntohl(cur_chain_len);
    cur_bucket = ntohl(cur_bucket);

    // check against prev bucket offset for sanity
    if (found_first_nonempty && (cur_chain_len > 0)) {
      result &= CheckLT32(prev_bucket, cur_bucket,
                  ToString("bucket_rec[%d].position < bucket_rec[%d].position",
                  i, i + 1));
      prev_chain_len = cur_chain_len;
      prev_bucket = cur_bucket;
    }

    // if this is the first non-empty bucket, read its offset from the
    // bucket record to make sure it lines up with the number of
    // records we have.
    if ((cur_chain_len > 0) && (!found_first_nonempty)) {
      found_first_nonempty = true;

      assert(fseek(file_, offset_ + kBucketLengthSize, SEEK_SET) == 0);
      assert(fread(&prev_chain_len, sizeof(int32_t), 1, file_) == 1);
      prev_chain_len = ntohl(prev_chain_len);
      assert(fread(&prev_bucket, sizeof(int32_t), 1, file_) == 1);
      prev_bucket = ntohl(prev_bucket);

      result &= CheckEQ32(offset_ + kBucketLengthSize +
                kBucketRecordSize*num_buckets_, prev_bucket,
                "position of the first non-empty bucket (expected to be "
                "(table start) + (4) + (8*num_buckets))");
    }

    // Seek to the bucket, make sure there are three element position
    // records that make sense.
    int32_t table_end = offset + len;
    CheckLT32(cur_bucket, table_end + 1,
              ToString("bucket_rec[%d].position < table_end+1", i));
    for (int j = 0; j < cur_chain_len; j++) {
      // read the element position
      int32_t element_pos;
      assert(fseek(file_, cur_bucket + j * kElementPositionSize,
                   SEEK_SET) == 0);
      assert(fread(&element_pos, sizeof(int32_t), 1, file_) == 1);
      element_pos = ntohl(element_pos);
      result &= CheckLT32(element_pos, table_end + 1,
                ToString("bucket[%d].element[%d].position < table_end+1",
                i, j));

      // read the element itself
      result &= CheckElement(element_pos, i);
    }
  }  // end loop over all buckets
  return result;
}

}  // namespace hw3

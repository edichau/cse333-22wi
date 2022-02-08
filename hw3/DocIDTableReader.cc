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

#include "./DocIDTableReader.h"

#include <list>      // for std::list
#include <cstdio>    // for (FILE*)

extern "C" {
  #include "libhw1/CSE333.h"
}

using std::list;

namespace hw3 {

// The constructor for DocIDTableReader calls the constructor
// of HashTableReader(), its base class. The base class takes
// care of taking ownership of f and using it to extract and
// cache the number of buckets within the table.
DocIDTableReader::DocIDTableReader(FILE* f, IndexFileOffset_t offset)
  : HashTableReader(f, offset) { }

bool DocIDTableReader::LookupDocID(
     const DocID_t& doc_id, list<DocPositionOffset_t>* const ret_val) const {
  // Use the base class's `LookupElementPositions` function to
  // walk through the docIDtable and get back a list of offsets
  // to elements in the bucket for this docID.
  auto elements = LookupElementPositions(doc_id);

  // If the list of elements is empty, we're done.
  if (elements.empty())
    return false;

  // Iterate through all of elements, looking for our docID.
  for (IndexFileOffset_t& curr_element : elements) {
    // STEP 1.
    // Slurp the next docid out of the current element.
    DocIDElementHeader curr_header;


    // Is it a match?
    if (curr_header.doc_id == doc_id) {
      // STEP 2.
      // Yes!  Extract the positions themselves, appending to
      // std::list<DocPositionOffset_t>.  Be sure to push in the right
      // order, adding to the end of the list as you extract
      // successive positions.


      // STEP 3.
      // Return the positions list through the output parameter,
      // and return true.

      return true;
    }
  }

  // We failed to find a matching docID, so return false.
  return false;
}

list<DocIDElementHeader> DocIDTableReader::GetDocIDList() const {
  // This will be our returned list of docIDs within this table.
  list<DocIDElementHeader> doc_id_list;

  // Go through *all* of the buckets of this hashtable, extracting
  // out the docids in each element and the number of word positions
  // for the each docid.
  for (int i = 0; i < header_.num_buckets; i++) {
    // STEP 4.
    // Seek to the next BucketRecord.  The "offset_" member
    // variable stores the offset of this docid table within
    // the index file.


    // STEP 5.
    // Read in the chain length and bucket position fields from
    // the bucket_rec.
    BucketRecord bucket_rec;


    // Sweep through the next bucket, iterating through each
    // chain element in the bucket.
    off_t element_offset;
    for (int j = 0; j < bucket_rec.chain_num_elements; j++) {
      // Seek to chain element's position field in the bucket header.
      element_offset = bucket_rec.position + j * sizeof(ElementPositionRecord);
      Verify333(fseek(file_, element_offset, SEEK_SET) == 0);

      // STEP 6.
      // Read the next element position from the bucket header.
      // and seek to the element itself.
      ElementPositionRecord element_pos;


      // STEP 7.
      // Read in the docid and number of positions from the element.
      DocIDElementHeader element;


      // Append it to our result list.
      doc_id_list.push_back(element);
    }
  }

  // Done!  Return the result list.
  return doc_id_list;
}

}  // namespace hw3

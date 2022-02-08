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

#include "./WriteIndex.h"

#include <cstdio>    // for (FILE*).
#include <cstring>   // for strlen(), etc.

// We need to peek inside the implementation of a HashTable so
// that we can iterate through its buckets and their chain elements.
extern "C" {
  #include "libhw1/CSE333.h"
  #include "libhw1/HashTable_priv.h"
}
#include "./LayoutStructs.h"
#include "./Utils.h"

namespace hw3 {
//////////////////////////////////////////////////////////////////////////////
// Helper function declarations and constants

static constexpr int kFailedWrite = -1;

// Helper function to write the docID --> filename mapping from the
// DocTable `dt` into file `f`, starting at byte offset `offset`.
// Returns the size of the written DocTable or a negative value on error.
static int WriteDocTable(FILE* f, DocTable* dt, IndexFileOffset_t offset);

// Helper function to write the MemIndex `mi` into file `f`, starting
// at byte offset `offset`. 
// Returns the size of the written MemIndex or a negative value on error.
static int WriteMemIndex(FILE* f, MemIndex* mi, IndexFileOffset_t offset);

// Helper function to write the index file's header into file `f`.
//
// Returns the size of the written Header or a negative value on error.
//
// FYI: Will atomically write the kMagicNumber as the very last thing, so
// that if we crash part way through writing an index file, it won't contain
// a valid kMagicNumber and the rest of HW3 will know to report an error.
static int WriteHeader(FILE* f, int doctable_bytes, int memidx_bytes);

// Function pointer used by WriteHashTable() to write a HashTable's
// HTKeyValue_t element into the index file at a specified byte offset.
//
// Arguments:
// - f: the file to write into
// - offset: the byte offset into "f", at which we should write the
//   element
// - kv: a pointer to the key value pair to be interpreted and written
//
// Returns:
// - the number of bytes written, or a negative value on error.
typedef int (*WriteElementFn)(FILE* f, IndexFileOffset_t offset,
                              HTKeyValue_t* kv);

// Writes a HashTable into the index file at a specified byte offset.
//
// Writes a header (BucketListHeader), a list of bucket records (BucketRecord),
// then the bucket contents themselves (using a content-specific instance of
// WriteElementFn).
//
// Since this function can write any HashTable, regardless of its contents,
// it is the core functionality of the file.
//
// Arguments:
// - f: the file to write into
// - offset: the byte offset into "f", at which we should write the hashtable
// - ht: the hashtable to write
// - fn: a function that serializes a single HTKeyValue_t.  Needs to be
//       specific to the hashtable's contents.
//
// Returns:
// - the number of bytes written, or a negative value on error
static int WriteHashTable(FILE* f, IndexFileOffset_t offset, HashTable* ht,
                          WriteElementFn fn);

// Helper function used by WriteHashTable() to write a BucketRecord (ie, a
// `bucket_rec` within the hw3 diagrams).
//
// Arguments:
// - f: the file to write into
// - offset: the byte offset into "f", at which we should write the
//   BucketRecord
// - numElts: the number of elements in the bucket.  Used to initialize
//   the BucketRecord's contents
// - bucketOffset: the offset at which the bucket (not the BucketRecord) is
//   located in "f".
//
// Returns:
// - the number of bytes written, or a negative value on error
static int WriteHTBucketRecord(FILE* f, IndexFileOffset_t offset,
                               int32_t num_elts,
                               IndexFileOffset_t bucket_offset);

// Helper function used by WriteHashTable() to write out a bucket.
//
// Remember that a bucket consists of a LinkedList of elements. Thus, this
// function writes out a list of ElementPositionRecords, describing the
// location (as a byte offset) of each element, followed by a series of
// elements thesmelves (serialized using an element-specific WriteElementFn).
//
// Arguments:
//   - f: the file to write into
//   - offset: the byte offset into "f", at which we should write the bucket
//   - li: the bucket's contents
//   - fn: a function that serializes a single HTKeyValue_t -- stored as
//         the list's `LLPayload_t` -- within the LinkedList.
//
// Returns:
//   - the number of bytes written, or a negative value on error
static int WriteHTBucket(FILE* f, IndexFileOffset_t offset, LinkedList* li,
                         WriteElementFn fn);



//////////////////////////////////////////////////////////////////////////////
// "Writer" functions
//
// Functions that comply with the WriteElementFn signature, to be used when
// writing hashtable elements to disk.

// Writes an element of the IdToName table from a DocTable into
// a specified file at a specified offset.
static int WriteDocidToDocnameFn(FILE* f, IndexFileOffset_t offset,
                                 HTKeyValue_t* kv);


// Writes an element of the MemIndex into a
// a specified file at a specified offset.
static int WriteWordToPostingsFn(FILE* f, IndexFileOffset_t offset,
                                 HTKeyValue_t* kv);

// Writes an element of an inner postings table into
// a specified file at a specified offset.
static int WriteDocIDToPositionListFn(FILE* f, IndexFileOffset_t offset,
                                      HTKeyValue_t* kv);



//////////////////////////////////////////////////////////////////////////////
// WriteIndex

int WriteIndex(MemIndex* mi, DocTable* dt, const char* file_name) {
  // Do some sanity checking on the arguments we were given.
  Verify333(mi != nullptr);
  Verify333(dt != nullptr);
  Verify333(file_name != nullptr);

  // fopen() the file for writing; use mode "wb+" to indicate binary,
  // write mode, and to create/truncate the file.
  FILE* f = fopen(file_name, "wb+");
  if (f == nullptr) {
    return kFailedWrite;
  }

  // Remember that the format of the index file is a header, followed by a
  // doctable, and then lastly a memindex.
  //
  // We write out the doctable and memindex first, since we need to know
  // their sizes before we can calculate the header.  So we'll skip over
  // the header for now.
  IndexFileOffset_t cur_pos = sizeof(IndexFileHeader);

  // Write the document table.
  int dt_bytes = WriteDocTable(f, dt, cur_pos);
  if (dt_bytes == kFailedWrite) {
    fclose(f);
    unlink(file_name);  // delete the file
    return kFailedWrite;
  }
  cur_pos += dt_bytes;

  // STEP 1.
  // Write the memindex.


  // STEP 2.
  // Finally, backtrack to write the index header and write it.


  // Clean up and return the total amount written.
  fclose(f);
  return cur_pos;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static int WriteDocTable(FILE* f, DocTable* dt, IndexFileOffset_t offset) {
  // Break the DocTable abstraction in order to grab the docid->filename
  // hash table, then serialize it to disk.
  return WriteHashTable(f, offset, DT_GetIDToNameTable(dt),
                        &WriteDocidToDocnameFn);
}

static int WriteMemIndex(FILE* f, MemIndex* mi, IndexFileOffset_t offset) {
  // Use WriteHashTable() to write the MemIndex into the file.  You'll
  // need to pass in the WriteWordToPostingsFn helper function as the
  // final argument.
  return WriteHashTable(f, offset, mi, &WriteWordToPostingsFn);
}

static int WriteHeader(FILE* f, int doctable_bytes, int memidx_bytes) {
  // STEP 3.
  // We need to calculate the checksum over the doctable and index
  // table.  (Note that the checksum does not include the index file
  // header, just these two tables.)
  //
  // Use fseek() to seek to the right location, and use a CRC32 object
  // to do the CRC checksum calculation, feeding it characters that you
  // read from the index file using fread().
  // Seek to the start of the doctable.
  CRC32 crc;



  // Write the header fields.  Be sure to convert the fields to
  // network order before writing them!
  IndexFileHeader header(kMagicNumber, crc.GetFinalCRC(),
                         doctable_bytes, memidx_bytes);
  header.ToDiskFormat();

  if (fseek(f, 0, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  if (fwrite(&header, sizeof(IndexFileHeader), 1, f) != 1) {
    return kFailedWrite;
  }

  // Use fsync to flush the header field to disk.
  Verify333(fsync(fileno(f)) == 0);

  // We're done!  Return the number of header bytes written.
  return sizeof(IndexFileHeader);
}

static int WriteHashTable(FILE* f, IndexFileOffset_t offset, HashTable* ht,
                          WriteElementFn fn) {
  // Write the HashTable's header, which consists simply of the number of
  // buckets.
  BucketListHeader header(ht->num_buckets);
  header.ToDiskFormat();
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  if (fwrite(&header, sizeof(BucketListHeader), 1, f) != 1) {
    return kFailedWrite;
  }

  // The byte offset of the next BucketRecord we want to write.  Remember that
  // the bucket records are located after the bucket header.
  IndexFileOffset_t record_pos = offset + sizeof(BucketListHeader);

  // The byte offset of the next bucket we want to write.  Reember that
  // the buckets are placed after the bucket header and the entire list
  // of BucketRecords.
  IndexFileOffset_t bucket_pos = offset + sizeof(BucketListHeader)
      + (ht->num_buckets) * sizeof(BucketRecord);

  // Iterate through the hashtable contents, first writing each bucket record
  // (ie, the BucketRecord) and then jumping forward in the file to write
  // the bucket contents itself.
  //
  // Be sure to handle the corner case where the bucket's chain is
  // empty.  For that case, you still have to write a record for the
  // bucket, but you won't write a bucket.
  for (int i = 0; i < ht->num_buckets; i++) {
    // STEP 4.


  }

  // Calculate and return the total number of bytes written.
  return bucket_pos - offset;
}

static int WriteHTBucketRecord(FILE* f, IndexFileOffset_t offset,
                               int32_t num_elts,
                               IndexFileOffset_t bucket_offset) {
  // STEP 5.
  // Initialize a BucketRecord in network byte order.


  // fseek() to where we want to write this record.
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }

  // STEP 6.
  // Write the BucketRecord.


  // Calculate and return how many bytes we wrote.
  return sizeof(BucketRecord);
}

static int WriteHTBucket(FILE* f, IndexFileOffset_t offset, LinkedList* li,
                         WriteElementFn fn) {
  int num_elts = LinkedList_NumElements(li);
  if (num_elts == 0) {
    // Not an error
    // nothing to write
    return 0;
  }

  // The byte offset of the next ElementPositionRecord we want to write.
  IndexFileOffset_t record_pos = offset;

  // The byte offset of the next element we want to write.  Remember that
  // the elements are placed after the entire list of ElementPositionRecords.
  IndexFileOffset_t element_pos = offset
      + sizeof(ElementPositionRecord) * num_elts;

  // Iterate through the list contents, first writing each entry's (ie, each
  // payload's) ElementPositionRecord and then jumping forward in the file
  // to write the element itself.
  //
  // Be sure to write in network order, and use the "fn" argument to write
  // the element (ie, the list payload) itself.
  LLIterator* it = LLIterator_Allocate(li);
  Verify333(it != nullptr);
  for (int i = 0; i < num_elts; i++) {
    // STEP 7.
    // fseek() to the where the ElementPositionRecord should be written,
    // then fwrite() it in network order.



    // STEP 8.
    // Write the element itself, using fn.



    // Advance to the next element in the chain, updating our offsets.
    record_pos += sizeof(ElementPositionRecord);
    element_pos += 1;  // you probably want to change this
    LLIterator_Next(it);
  }
  LLIterator_Free(it);

  // Return the total amount of data written.
  return element_pos - offset;
}


//////////////////////////////////////////////////////////////////////////////
// "Writer" functions

// This write_element_fn is used to write a doc_id->doc_name mapping
// element, i.e., an element of the `doctable` table.
static int WriteDocidToDocnameFn(FILE* f, IndexFileOffset_t offset,
                                 HTKeyValue_t* kv) {
  // STEP 9.
  // determine the file name length
  int16_t file_name_bytes = kFailedWrite;  // you may want to change this


  // fwrite() the docid from `kv`.  Remember to convert to
  // disk format before writing.
  DoctableElementHeader header(kv->key, file_name_bytes);
  header.ToDiskFormat();

  // fseek() to the provided offset and then write the header.
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  if (fwrite(&header, sizeof(DoctableElementHeader), 1, f) != 1) {
    return kFailedWrite;
  }

  // STEP 10.
  // fwrite() the file name.  We don't write the null-terminator from the
  // string, just the characters, since we've already written a length
  // field for the string.


  // STEP 11.
  // calculate and return the total amount written.
  return kFailedWrite;  // you may want to change this
}

// This write_element_fn is used to write a DocID + position list
// element (i.e., an element of a nested docID table) into the file at
// offset 'offset'.
static int WriteDocIDToPositionListFn(FILE* f,
                                      IndexFileOffset_t offset,
                                      HTKeyValue_t* kv) {
  // Extract the docID from the HTKeyValue_t.
  DocID_t doc_id = static_cast<DocID_t>(kv->key);

  // Extract the positions LinkedList from the HTKeyValue_t and
  // determine its size.
  LinkedList* positions = static_cast<LinkedList*>(kv->value);
  int num_positions = LinkedList_NumElements(positions);

  // STEP 12.
  // Write the header, in disk format.
  // You'll need to fseek() to the right location in the file.


  // Loop through the positions list, writing each position out.
  DocIDElementPosition position;
  LLIterator* it = LLIterator_Allocate(positions);
  Verify333(it != nullptr);
  for (int i = 0; i < num_positions; i++) {
    // STEP 13.
    // Get the next position from the list.


    // STEP 14.
    // Truncate to 32 bits, then convert it to network order and write it out.


    // Advance to the next position.
    LLIterator_Next(it);
  }
  LLIterator_Free(it);

  // STEP 15.
  // Calculate and return the total amount of data written.
  return kFailedWrite;  // you may want to change this
}

// This write_element_fn is used to write a WordPostings
// element into the file at position 'offset'.
static int WriteWordToPostingsFn(FILE* f,
                                 IndexFileOffset_t offset,
                                 HTKeyValue_t* kv) {
  // Extract the WordPostings from the HTKeyValue_t.
  WordPostings* wp = static_cast<WordPostings*>(kv->value);
  Verify333(wp != nullptr);

  // STEP 16.
  // Prepare the wordlen field.
  int16_t word_bytes = kFailedWrite;  // you may want to change this

  // Write the nested DocID->positions hashtable (i.e., the "docID
  // table" element in the diagrams).  Use WriteHashTable() to do it,
  // passing it the wp->postings table and using the
  // WriteDocIDToPositionListFn helper function as the final parameter.
  int ht_bytes = WriteHashTable(f,
                              offset + sizeof(WordPostingsHeader) + word_bytes,
                              wp->postings,
                              &WriteDocIDToPositionListFn);

  if (ht_bytes == kFailedWrite) {
    return kFailedWrite;
  }

  // STEP 17.
  // Write the header, in network order, in the right place in the file.
  WordPostingsHeader header(word_bytes, ht_bytes);



  // STEP 18.
  // Write the word itself, excluding the null terminator, in the right
  // place in the file.



  // STEP 19.
  // Calculate and return the total amount of data written.
  return kFailedWrite;  // you may want to change this
}
}  // namespace hw3

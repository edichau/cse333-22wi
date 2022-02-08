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

#include "./FileIndexChecker.h"

#include <assert.h>     // for assert
#include <sys/types.h>  // for stat()
#include <sys/stat.h>   // for stat()
#include <unistd.h>     // for stat()
#include <iostream>     // for cout, endl
#include "../Utils.h"   // for class CRC32.
#include "./CheckerUtils.h"
#include "./DocTableChecker.h"
#include "./IndexTableChecker.h"

using std::cout;
using std::endl;
using std::hex;
using std::string;

namespace hw3 {

FileIndexChecker::FileIndexChecker(const string& file_name) {
  // Stash a copy of the index file's name.
  file_name_ = file_name;

  // Open a (FILE*) associated with file_name.  Crash on error.
  file_ = fopen(file_name_.c_str(), "rb");
  assert(file_ != nullptr);

  // Make the (FILE*) be unbuffered.  ("man setbuf")
  setbuf(file_, nullptr);

  // read in the doctable_size_ and index_size_ fields.
  int offset = kMagicNumberOffset + sizeof(uint32_t) + sizeof(uint32_t);
  fseek(file_, offset, SEEK_SET);

  // Read the doctable size field from the index file.
  assert(fread(&doctable_size_, sizeof(doctable_size_), 1, file_) == 1);
  doctable_size_ = ntohl(doctable_size_);

  // Read the index size field from the index file.
  assert(fread(&index_size_, sizeof(index_size_), 1, file_) == 1);
  index_size_ = ntohl(index_size_);
}

bool FileIndexChecker::CheckFile() {
  // Is the file we are checking good?
  bool fileGood = true;

  cout << "hw3fsck'ing " << file_name_ << endl;

  fileGood &= this->CheckHeader();
  fileGood &= this->CheckDocTable();
  fileGood &= this->CheckIndexTable();

  cout << "  done" << endl;

  return fileGood;
}

bool FileIndexChecker::CheckHeader() {
  bool header_good = true;

  cout << "  checking the header..." << endl;

  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags format_flags = cout.flags();
  cout << hex;

  // Read the magic number header field from the index file, verify
  // that it is correct.  Warn if not.
  cout << "    checking the magic number..." << endl;
  uint32_t magic_number;
  assert(fseek(file_, kMagicNumberOffset, SEEK_SET) == 0);
  assert(fread(&magic_number, sizeof(magic_number), 1, file_) == 1);
  magic_number = ntohl(magic_number);
  header_good &= CheckEQ32(kMagicNumber, magic_number, "magic number");

  // Read the checksum header field from the index file.
  uint32_t checksum;
  assert(fread(&checksum, sizeof(checksum), 1, file_) == 1);
  checksum = ntohl(checksum);

  // Make sure the index file's length lines up with the header fields.
  cout << "    checking file size against table offsets..." << endl;
  struct stat f_stat;
  assert(stat(file_name_.c_str(), &f_stat) == 0);
  header_good &= CheckEQ32(f_stat.st_size - kFileIndexHeaderSize,
                        doctable_size_ + index_size_,
                        "doctable_size + index_size");
  if ((ntohl(doctable_size_) + ntohl(index_size_)) ==
      f_stat.st_size - kFileIndexHeaderSize) {
    cout << "Note: you likely forgot an endianness conversion, ";
    cout << "since ntohl(doctable_size_) + ntohl(index_size_) == ";
    cout << "filesize - 16" << endl;
  }

  // Re-calculate the checksum, make sure it matches that in the header.
  cout << "    recalculating the checksum..." << endl;
  fseek(file_, kFileIndexHeaderSize, SEEK_SET);
  CRC32 crc;
  constexpr int kBufSize = 512;
  uint8_t buf[kBufSize];
  int32_t bytes_left = f_stat.st_size - kFileIndexHeaderSize;
  while (bytes_left > 0) {
    int bytes_read = fread(&buf[0], sizeof(uint8_t), kBufSize, file_);
    assert(bytes_read > 0);
    for (int i = 0; i < bytes_read; i++) {
      crc.FoldByteIntoCRC(buf[i]);
    }
    bytes_left -= bytes_read;
  }
  header_good &= CheckEQ32(crc.GetFinalCRC(), checksum, "checksum");

  // set the flags of cout to what they were before we changed them here.
  cout.flags(format_flags);
  return header_good;
}

bool FileIndexChecker::CheckDocTable() {
  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags format_flags = cout.flags();
  cout << hex;

  // Start checking the doc table
  cout << "  checking the doctable..." << endl;
  DocTableChecker dtc(FileDup(file_),
                      kFileIndexHeaderSize,
                      doctable_size_);
  bool doctable_good = dtc.Check(kFileIndexHeaderSize, doctable_size_);

  // set the flags of cout to what they were before we changed them here.
  cout.flags(format_flags);
  return doctable_good;
}

bool FileIndexChecker::CheckIndexTable() {
  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags format_flags = cout.flags();
  cout << hex;

  // Start checking the index table
  cout << "  checking the index table..." << endl;
  IndexTableChecker itc(FileDup(file_),
                        kFileIndexHeaderSize + doctable_size_,
                        index_size_);
  bool index_good = itc.Check(kFileIndexHeaderSize + doctable_size_, index_size_);
  // set the flags of cout to what they were before we changed them here.
  cout.flags(format_flags);
  return index_good;
}

FileIndexChecker::~FileIndexChecker() {
  // Close the (FILE*).
  assert(fclose(file_) == 0);
}

}  // namespace hw3

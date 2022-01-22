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

extern "C" {
  #include "./CrawlFileTree.h"
}

#include "gtest/gtest.h"
extern "C" {
  #include "./DocTable.h"
  #include "./MemIndex.h"
}

#include "./test_suite.h"

static char *AppendString(const char* prefix, const char* suffix) {
  int pre_len = strlen(prefix);
  int suf_len = strlen(suffix);
  char *copy = static_cast<char*>(malloc(pre_len + suf_len + 1));
  strncpy(copy, prefix, pre_len);
  strncpy(&copy[pre_len], suffix, suf_len);
  copy[pre_len + suf_len] = '\0';
  return copy;
}

namespace hw2 {

TEST(Test_CrawlFileTree, ReadsFromDisk) {
  HW2Environment::OpenTestCase();
  bool res;
  DocTable* doc_table;
  MemIndex* index;

  // Test that it detects a valid directory.
  res = CrawlFileTree(const_cast<char*>("./test_tree/bash-4.2/support"),
                      &doc_table,
                      &index);
  ASSERT_EQ(true, res);
  HW2Environment::AddPoints(10);
  DocTable_Free(doc_table);
  MemIndex_Free(index);
  HW2Environment::AddPoints(10);

  // Test that it detects a non-existant directory.
  res = CrawlFileTree(const_cast<char*>("./nonexistent/"),
                      &doc_table,
                      &index);
  ASSERT_EQ(false, res);
  HW2Environment::AddPoints(10);

  // Test that it rejects files (instead of directories).
  res = CrawlFileTree(const_cast<char*>("./test_suite.c"),
                      &doc_table, &index);
  ASSERT_EQ(false, res);
  HW2Environment::AddPoints(10);
}

TEST(Test_CrawlFileTree, ChecksArgs) {
  DocTable* doc_table;
  MemIndex* index;

  // Test that CrawlFileTree can be called with any combination of NULLs
  ASSERT_EQ(false, CrawlFileTree(NULL, &doc_table, &index));
  ASSERT_EQ(false, CrawlFileTree(const_cast<char *>(""), &doc_table, &index));
  ASSERT_EQ(false,
            CrawlFileTree(const_cast<char*>("./test_tree"), NULL, &index));
  ASSERT_EQ(false,
            CrawlFileTree(const_cast<char*>("./test_tree"), &doc_table, NULL));
}

TEST(Test_CrawlFileTree, CheckDocTable) {
  HW2Environment::OpenTestCase();
  int res;
  DocTable* doc_table;
  MemIndex* index;

  int i;
  DocID_t doc_id;
  char* dir_path;
  const char* doc_name;

  const char* directory = "./test_tree/bash-4.2/doc/";

  // Note that this does not include some files since those files don't contain
  // ascii characters and should be skipped when handled.
  const char* files[] = {"FAQ", "INTRO", "Makefile.in", "README", "article.ms",
                          "article.ps", "article.txt", "bash.0", "bash.1",
                          "bash.html", "bash.ps", "bashbug.0", "bashbug.1",
                          "bashbug.ps", "bashref.html", "bashref.info",
                          "bashref.ps", "bashref.texi", "builtins.0",
                          "builtins.1", "builtins.ps", "fdl.texi", "fdl.txt",
                          "htmlpost.sh", "infopost.sh", "rbash.0", "rbash.1",
                          "rbash.ps", "rose94.ps", "texinfo.tex",
                          "version.texi"};

  // Crawl the test tree.
  res = CrawlFileTree(const_cast<char*>(directory),
                      &doc_table,
                      &index);
  HW2Environment::AddPoints(10);

  ASSERT_EQ(1, res);
  ASSERT_EQ(31, DocTable_NumDocs(doc_table));
  HW2Environment::AddPoints(10);

  for (i = 0; i < 31; i++) {
    dir_path = AppendString(directory, files[i]);
    doc_id = DocTable_GetDocID(doc_table, dir_path);
    ASSERT_NE(INVALID_DOCID, doc_id);
    doc_name = DocTable_GetDocName(doc_table, doc_id);
    ASSERT_NE(static_cast<const char*>(NULL), doc_name);
    ASSERT_STREQ(doc_name, dir_path);
    free(dir_path);
  }

  DocTable_Free(doc_table);
  MemIndex_Free(index);
  HW2Environment::AddPoints(10);
}

}  // namespace hw2


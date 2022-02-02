// Copyright 2022 Edison Chau - edichau@uw.edu
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#define MAX_INT 2147483647

void DumpBytes(void* pData, int32_t byteLen);
void CopyAndSort(uint8_t a[], uint8_t b[], int len);

int main(int argc, char *argv[]) {
  int32_t int_val = 1;
  float   float_val = 1.0f;
  uint8_t arr_unsorted[] = {3, 2, 0, 8, 17, 6, 10, 7, 8, 1, 12};
  uint8_t arr_sorted[]   = {0, 0, 0, 0,  0, 0,  0, 0, 0, 0,  0};

  DumpBytes(&int_val, sizeof(int_val));
  DumpBytes(&float_val, sizeof(float_val));
  DumpBytes(arr_unsorted, sizeof(arr_unsorted));
  CopyAndSort(arr_unsorted, arr_sorted, sizeof(arr_unsorted));
  DumpBytes(arr_sorted, sizeof(arr_sorted));

  return EXIT_SUCCESS;
}

// dumps bytes by printing them
void DumpBytes(void* pData, int byteLen) {
  uint8_t *ptr = (uint8_t *) pData;
  // prints the number of bytes and the starting pointer
  printf("The %d bytes starting at %p are:", byteLen, pData);

  //prints out all the bytes as exactly two digits each in lowercase hexadecimal
  for (int i = 0; i < byteLen; i++) {
    printf(" %02" PRIx8 "%s", *(ptr + i), (i == (byteLen - 1) ? "\n" : ""));
  }
}

// copys and sorts array a to array b.
void CopyAndSort(uint8_t a[], uint8_t b[], int len){
  // creates and initializes an array that stores which indicies have been the min
  int minIndices[len];
  for (int i = 0; i < len ; i++){ 
    minIndices[i] = 0; 
  } 
  // current to keep track of 
  int curr = 0;
  // index of the minimum
  int minInd = 0;

  while (curr < len) {
    // find the min and add to array
      int min = MAX_INT;
      for (int i = 0; i < len; i++) {
            if (a[i] < min && minIndices[i] != 1) {
              minInd = i;
              min = a[i];
            }
      }
      minIndices[minInd] = 1;
      b[curr] = min;
      curr++;
    }
}

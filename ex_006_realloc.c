#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main () {
  uint32_t *arr1;
  size_t arr1_bytesize = sizeof(uint32_t);
  uint32_t *arr2 = NULL;
  size_t arr2_bytesize = (sizeof(uint32_t)<<1);
  uint32_t i = 0;
  arr1 = (uint32_t*) malloc(arr1_bytesize);
  if (arr1 == NULL) {
    printf("memory allocation error 1\n");
    return 0;
  }
  arr1[0] = 0x1000;
  arr2 = (uint32_t*) realloc(arr1, arr2_bytesize);
  if (arr2 == NULL) {
    printf ("memory allocation error 2\n");
    free(arr1);
    return 0;
  }
  for (i = 0; i < (arr2_bytesize>>2); ++i) {
    printf("%d\n", arr2[i]);
  }
  if (arr1 == arr2) {
    printf("addresses are equal, but may not be\n");
    free(arr1);
  } else {
    printf("addresses are NOT equal, arr1 is freed by realloc");
    free(arr2);
  }
  return 0;
}

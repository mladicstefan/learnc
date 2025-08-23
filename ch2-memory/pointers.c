#include <stdio.h>
#include <stdlib.h>

int main(){
  void* generic = malloc(sizeof(int));
  int* typed = generic;
  *typed = 42;

  printf("%p\n", generic);
  printf("%p\n", typed);
  return 0;
}

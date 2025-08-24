#include <stdio.h>
#include <stdlib.h>

void pass_by_value(int x)
{
  x= 42; 
  printf("Pass by value: %p\n", x);
}

void pass_by_pointer(int* x)
{
  *x= 42; //We change the actual value, not make a copy
  // x = 42; WATCH OUT! This line would CHANGE THE POINTER, not the value. Which is a huge vulnerability!
  printf("Pass by pointer: %p\n", x);
}

int main()
{
  void* generic = malloc(sizeof(int)); // Malloc returns a void* which is generic (can be used for any type)
  int* typed = generic; // We cast to int* before accesing
  
  printf("Original value: %d\n", *typed);
    
  // Pass by value - dereference the pointer to get the value
  pass_by_value(*typed);  // Pass the VALUE (42)
  printf("After pass_by_value: %d\n", *typed);  // Still 42!
  
  // Pass by pointer - pass the pointer itself  
  pass_by_pointer(typed);  // Pass the POINTER
  printf("After pass_by_pointer: %d\n", *typed);
  
  free(typed);
  return 0;
}

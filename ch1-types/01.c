#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// INTEGERS - Whole numbers only
// Unsigned are positive only. Generally use signed to avoid errors, 
// but for MAC addresses or IP addresses where only positive is possible, 
// using unsigned avoids errors

int8_t   small_int = -128;     // 8 bits:  -128 to 127
uint8_t  small_pos = 255;      // 8 bits:  0 to 255

int16_t  medium_int = -32000;  // 16 bits: -32,768 to 32,767
uint16_t medium_pos = 65000;   // 16 bits: 0 to 65,535

int32_t  big_int = -2000000;   // 32 bits: -2 billion to +2 billion
uint32_t big_pos = 4000000;    // 32 bits: 0 to 4 billion

int64_t  huge_int = -9000000000LL;    // 64 bits: massive range
uint64_t huge_pos = 18000000000ULL;   // 64 bits: stupidly large

// FLOATING POINT - Numbers with decimals 
// (IEEE 754 standard unified everything, so we don't need stdfloat.h)
float    decimal = 3.14f;      // 32 bits: ~7 decimal digits precision
double   precise = 3.141592653589793; // 64 bits: ~15 decimal digits precision

void size_does_matter()
{
  struct demo{
      char a;      // 1 byte
      int b;       // 4 bytes
      short c;     // 2 bytes
      char d;      // 1 byte
  };
  printf("%zu\n",sizeof(struct demo));

  struct de {
      int b;       // 4 bytes
      short c;     // 2 bytes
      char a;      // 1 byte
      char d;      // 1 byte
  };
  printf("%zu\n",sizeof(struct de));

  struct diae {
      char a;      // 1 byte
      char d;      // 1 byte
      short c;     // 2 bytes
      int b;       // 4 bytes
  };
  printf("%zu\n",sizeof(struct diae));
}

int main()
{

  char letter = 'A';
  printf("%c\n", letter);  // Prints: A
  printf("%d\n", letter);  // Prints: 65

  char a = 'A';
  char b = a + 1;        // b = 66, which displays as 'B'
  char lower = 'A' + 32; // Convert to lowercase 'a'
  
  //print all ASCII characters.
  for (int i = 32; i <= 126; i++){
    printf("%c", i);
  }
  printf("\n");
  int32_t array[4] = {1,2,3,4};

  printf("Memory adress of the array: %p\n", array);
  printf("Memory adress of the first element: %p\n", &array[0]);
  printf("Address of non-existent array[5]: %p\n", &array[5]);
  
  size_does_matter();
  return 0;
}

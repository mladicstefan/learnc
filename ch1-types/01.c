
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

  return 0;
}

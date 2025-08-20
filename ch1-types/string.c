#include <stdio.h>
#include <string.h>

void do_unsafe_shit(){
   
   char source[] = "Hello world!";
   char safe_buffer[20];
   char unsafe_buffer [5];
   strcpy(safe_buffer, source); //Ok, since source fits in safe_buffer
   printf("Safe copy: '%s'\n", safe_buffer);
   
   strcpy(unsafe_buffer, source); //UNDEFINED BEHAVIOUR!
   printf("Unsafe buffer after overflow: '%s'\n", unsafe_buffer);
   printf("Memory corruption! unsafe_buffer[5-12] overwrote adjacent memory!\n");
   
   //the way you do bounds checking in C:
   //size_t is an unsigned int type for sizes, and the return type of these functions
   if(strlen(source) >= sizeof(unsafe_buffer)){
     printf("AAAAAHHHHHHH!!!!!!! LOST SHAREHOLDER VALUEEE!!!! SWITCHING TO JAVASCRIPT\n");
   }
   
   // strcat: appends source to end of destination
   // DANGER: No bounds checking, assumes dest has space
   char small_greeting[8] = "Hello ";    // Only 8 bytes total (7 chars + \0)
   char world[] = "World";
   printf("Before strcat overflow: '%s'\n", small_greeting);
   strcat(small_greeting, world);         // BUFFER OVERFLOW: 12 bytes into 8-byte buffer
   printf("After strcat overflow: '%s' (overwrote %zu bytes past buffer!)\n", 
          small_greeting, strlen("Hello World") + 1 - 8);
   
   // memcpy: copies raw memory bytes
   // DANGER: No overlap checking, can corrupt data
   char overlap_buffer[] = "abcdefgh";
   printf("Before memcpy overlap: '%s'\n", overlap_buffer);
   memcpy(overlap_buffer + 2, overlap_buffer, 6);  // UNDEFINED: overlapping regions
   printf("After memcpy overlap: '%s' (corrupted by overlapping copy!)\n", overlap_buffer);
}

int main() {
    char *ptr1 = "hello";
    char *ptr2 = "hello";        // Same literal!
    char arr1[] = "hello";
    char arr2[] = "hello";
    
    printf("String literal addresses:\n");
    printf("ptr1: %p\n", (void*)ptr1);
    printf("ptr2: %p\n", (void*)ptr2);     // Same address!
    
    printf("\nArray addresses:\n");
    printf("arr1: %p\n", (void*)arr1);
    printf("arr2: %p\n", (void*)arr2);     // Different addresses!
    
    char str[] = "Hello";
    printf("Garbage memory adress, undefined behaviour: %p\n", str[6]);
    do_unsafe_shit();
    return 0;
  
}

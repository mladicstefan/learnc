# C Types

First of all, congratulations on surviving the last chapter—that was a lot to take in. Especially if you've never programmed in C before. As you can see by now, my philosophy is a bit different. Every other course on the internet teaches you to "program" (when it only teaches you syntax, which is the least difficult part of programming, but they present it as the hardest part to earn money), while what I'm trying to do is teach you the tools that will make you a great programmer. That's why I don't really focus on syntax. It doesn't matter—you'll pick it up.

So, as I was saying, types. You might say *"Oh oh! I know this one! Okay, so we have int, float, string!!!"* and if you're clever, *"and we have short, long, and char"*. Congratulations! You get a cookie. 🍪

I'll even let you have **two cookies** if you can answer the next question: **"How many bits is an int in C?"** I'll even allow you to use Google.

If your answer is 16 bits—you're wrong.

If your answer is 32 bits—you're wrong.

**The answer is: both.**

Let's consult the K&R "C Programming Language" book (second edition), written by the creators of C themselves:

> *"Short is often 16 bits long, and int either 16 or 32 bits. Each compiler is free to choose appropriate sizes for its own hardware, subject only to the restriction that shorts and ints are at least 16 bits, longs are at least 32 bits, and short is no longer than int, which is no longer than long."*

Wait, what? **Often?** **Either?** **FREE TO CHOOSE?** **THE COMPILER HAS FREE WILL???**

Okay, okay, I'll even do you one better: **C doesn't have a string type.** It has a `char*`.

*"Whaaaat? I'm confused. I consulted my star chart about C types, but apparently Jupiter wasn't aligned with my compiler."*

Don't worry, you're currently experiencing typical symptoms of **"what the fuck"**. Doctors recommend you take a cigarette break and repeat this sentence in your head several times: *"Somebody stupider than me learned this."* If you do not have a cigarette, don't worry—C will make you start smoking. If you already smoke, C will make you switch to something stronger.

Welcome to C types, where the rules are made up and the bit counts don't matter.

## Okay, I promise no more tricks—we're actually going to demystify this now

Welcome back, I hope you had a nice cigarette. Now, to work.

The reason that C types are "often" or "either" is actually really simple: **computer architecture differences**. When you download software, I'm sure you have seen the different architecture options: x86-64 (and no, that is not read 86 by 64), which is what most personal computers and servers run on; ARM (Raspberry Pi, phones, Mac, due to power efficiency); RISC-V; etc. What those names really mean is the difference in instruction sets.

Here's a simple example of how different architectures handle the same operation:

```asm
# x86-64 (CISC - Complex Instruction Set)
mov $42, %rax          # Move 64-bit value to register

# ARM (RISC - Reduced Instruction Set)  
mov x0, #42             # Move value to register (simpler syntax)
```

It also has something to do with whether the machine is Little Endian vs Big Endian.

So you see, every variable has an address in memory: `0x12345678`, which is just a hexadecimal number. But you see, the address that I have written is using Big Endian, which means that memory addresses are formatted like we read, from left to right, most significant byte first. So you might think that we actually use Big Endian on most machines? Right? Right...? Oh fu—

Nope, we use Little Endian. Thank you Intel for releasing the 8086 processor in 1978 and deciding that bytes should be stored backwards. When Intel dominated the PC market, Little Endian became the standard despite being completely counter-intuitive.

If you think this doesn't matter, let me tell you about the time I spent 3 hours debugging a networking issue on a personal project, because of this exact bullshit. Here's the code snippet that finally worked (this is C++, C doesn't have std::runtime_error):

```cpp
//set socket domain (IPV4, IPV6 ...)
address.sin_family = domain;
//convert port to network byte order (Big Endian VS Small Endian)
address.sin_port = htons(port);
// set ip to bind to, convert ip addr to network byte order
address.sin_addr.s_addr = htonl(interface);
//set socket fd
sock = socket(domain,type,protocol);
if (sock < 0) {
    throw std::runtime_error("socket() failed: " + std::string(std::strerror(errno)));
}
```
See those htons() and htonl() calls? That's me having to manually translate between my little endian x86 machine and the big endian network protocol(thanks intel). So now, decades later, every network programmer getsto deal with this mess.
What those functions do:

htons() = "host to network short" - converts 16-bit port numbers
htonl() = "host to network long" - converts 32-bit addresses

Without them? Your server thinks port 80 is actually port 20480. Good luck debugging that.

The size of the variable depends on the architecture because different CPUs have different **word sizes**—basically, how much data they can chew through in one bite. A 16-bit CPU naturally works with 16-bit integers,a 32-bit CPU prefers 32-bit integers, and a 64-bit CPU can handle larger integers efficiently. It's all about what the CPU can process in one instruction cycle.

## The Solution

Tired of playing Russian roulette with integers? Well, somebody else was too, so they made `stdint.h`.

```c
#include <stdint.h>

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
```

Now when you write `uint32_t`, you know for damn sure it's 32 bits whether you're on a Raspberry Pi, your laptop, or some ancient embedded system.

## Chars, Arrays, Strings

## What Are Chars?

**Chars are 8-bit integers pretending to be letters.**

```c
char letter = 'A';
printf("%c\n", letter);  // Prints: A
printf("%d\n", letter);  // Prints: 65
```

The single quotes `'A'` are just syntax sugar for the number 65. Your CPU stores 65 in 8 bits, and printf decides whether to show it as a letter or number based on `%c` vs `%d`.

**ASCII mapping:** Every character has a number. `'A'` = 65, `'B'` = 66, `'0'` = 48, etc.

**Chars do math:**
```c
char a = 'A';
char b = a + 1;        // b = 66, which displays as 'B'
char lower = 'A' + 32; // Convert to lowercase 'a'
``` 
```c
//print all ASCII characters
for (int i = 32; i <= 126; i++) {
    printf("%c ", i);
}
```
Most programs today use UTF-8 encoding, which can represent any character (like 🍪 or 中文) by using multiple char bytes per character, while ASCII only fits English letters in single bytes.

When K&R says *"character constants participate in numeric operations just as any other integers"*, this is what they mean. To your CPU, there's no difference between `char` and `int8_t` - they're both 8-bit integers. The "character" part only exists when you print them with `%c`.
to be continued...

## Arrays

Okay, let's start this off with an easy question. I assume you know what arrays are, so what is the type of an array?

You've probably gotten so used to my questions by now that you know the answer isn't simple. If not, I hope you get used to it fast. So, there can be a few guesses made but let's first take a trip into lala land where everything is dandy (Python) and see how arrays are handled there.

If you take a look inside `array.py` you can see we declare an array:
(you can run the example yourself with `python3 array.py`)

### Let's create a Python list:
```python
def main():
    x: list[int] = [1, 2, 3, 4]
```

### Let's print the entire array:
```python
    print(f"x: {x}")
```
```bash
x: [1, 2, 3, 4]
```

### Let's print the memory address of the list object:
```python
    print(f"id(x): {hex(id(x))}")
```
```bash
id(x): 0x7f547ad01f80
```

### Let's print the memory address of the first element:
```python
    print(f"id(x[0]): {hex(id(x[0]))}")
```
```bash
id(x[0]): 0x7f547baef330
```
**Notice: The list object and its first element have completely different memory addresses!**

### Let's check what type this array actually is:
```python
    print(f"type(x): {type(x)}")
```
```bash
type(x): <class 'list'>
```

### Let's try to access an out-of-bounds element:
```python
    try:
        print(f"Trying to access out of bound element: {x[232]}")
    except Exception as e:
        raise e
```
```bash
IndexError: list index out of range
```

In Python, arrays are complex objects with their own memory addresses, built-in methods like `len()`, and safety features. C arrays are just raw memory with no protection whatsoever.

### Let's now do it in C:
```c 
int32_t array[4] = {1,2,3,4};
printf("Memory address of the array: %p\n", array);
printf("Memory address of the first element: %p\n", &array[0]); // %p is pointer printf formatting
// & is memory address operator, we will talk more about it when we talk about pointers
```
```bash 
Memory address of the array: 0x7ffd83b46430
Memory address of the first element: 0x7ffd83b46430 
```

**Aha! They're the same!**

In C, an array name IS just a pointer to the first element. There's no separate "array object" like in Python - `array` and `&array[0]` are literally the same address. C arrays are just contiguous blocks of memory with no metadata, no methods, and no safety features. The "array" is nothing more than the memory location where the first element lives.

### Let's try accessing an element that doesn't exist:
```c
printf("Address of non-existent array[5]: %p\n", (void*)&array[5]);
```
```bash
Address of non-existent array[5]: 0x7ffd83b46444                                                                                                   
```

**C doesn't care that `array[5]` doesn't exist** - it just calculates where it would be (20 bytes past the start) and gives you that address. This points to whatever random memory happens to be there.

### Why Array Indexing Starts at 0

Ever wondered why counting in programming starts at 0? The answer lies in how arrays actually work under the hood.

When we access an array element like `array[3]`, we're not doing some magical indexing operation. We're doing **pointer arithmetic**:

```c
// These are identical:
array[3]
*(array + 3)    // Take array pointer, move 3 elements forward, dereference
```

The `array[i]` syntax is just syntactic sugar for `*(array + i)`. When `i = 0`, you get `*(array + 0)` which simplifies to `*array` - the first element. Zero-based indexing makes the math clean and direct.

The compiler automatically handles element sizing - `array + 3` doesn't mean "add 3 bytes," it means "move 3 elements forward." For an `int32_t` array, that's actually moving 12 bytes (3 × 4 bytes per int).

### The Security Implications

This lack of bounds checking is exactly how buffer overflow exploits work - attackers deliberately access memory beyond array boundaries to corrupt other variables or inject malicious code. It's one of the most common sources of security vulnerabilities in C programs.

```c
int32_t array[4] = {1, 2, 3, 4};
int32_t secret = 42;

// Whoops, overwrote the secret variable
array[4] = 999;  // No error, just corrupts whatever comes after
```

You'll understand more about arrays when we cover memory management and pointers, but for now this is enough. For dynamic arrays like in Python, we'll need to implement a vector type (which comes built-in with C++ and Rust).

## Strings

## Prepare yourself, this is going to be a bit complex

Okay, I'm going to lay off the sarcasm a bit now since this is really important.

C strings are the source of countless memory vulnerabilities and security exploits, and we are going to do a deep dive on it.

**A C string is an array of bytes terminated with a null character `\0` which marks the end of the string.**

## Lvalue & Rvalue

**lvalues** are "things you can assign to", **rvalues** are "things you assign".

```c 
int x = 5;
//  ^   ^
//  |   rvalue (temporary value)
//  lvalue (can be assigned to and you manage its lifetime)

x = 10;     // x is lvalue (can receive assignment)
5 = x;      // ERROR: 5 is rvalue (can't be assigned to)
```

So for strings, an rvalue is a string literal like `"Hello World!"`. That is a literal—it is not mutable. 

So if strings are arrays of bytes and arrays decay into pointers, then this code segment is the same, right?

```c 
char* str1 = "Hello World!";
char str2[] = "Hello World!";
```

**Unfortunately not.** Let's take a deep dive through a practical example. You can look at `string.c`:

```c 
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
```

**Output:**
```bash 
String literal addresses:
ptr1: 0x559ea4762004
ptr2: 0x559ea4762004

Array addresses:
arr1: 0x7ffd0931b18c
arr2: 0x7ffd0931b192
```

## What's Actually Happening

Here's what's happening: `ptr1` and `ptr2` both point to the **same string literal** stored in read-only memory (`.rodata` section). This is like the number `5` in our earlier example—it's an rvalue that can't be modified.

If you try to modify `ptr1` by dereferencing it (`*ptr1 = 'H'`), you'll get a **SIGSEGV** crash because you're trying to write to read-only memory. It's exactly like trying to do `5 = x`—the compiler/OS says "nope, that's read-only."

However, `arr1` and `arr2` are **copies** of the string literal stored in writable memory (stack), so they have different addresses and can be modified safely. They're true lvalues that you own and control.

## C String Security Nightmare

I've included a function `do_unsafe_shit()` in `string.c`. Take a look at the code, and compile it to see what has cost taxpayers millions of dollars.

## Complex Types

Structs (aligment) sum types, Enums (dumb ADT's, State machines etc...), Union Types()

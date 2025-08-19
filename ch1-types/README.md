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

If you think this doesn't matter, let me tell you about the time I spent 3 hours debugging a networking issue because of this exact bullshit. Here's the code snippet that finally worked (this is C++, C doesn't have std::runtime_error):

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
See those htons() and htonl() calls? That's me having to manually translate between my little endian x86 machine and the big endian network protocol. Because apparently, in 1978, Intel and the networking standards committee couldn't agree on which direction bytes should go. So now, decades later, every network programmer gets to deal with this mess.
What those functions do:

htons() = "host to network short" - converts 16-bit port numbers
htonl() = "host to network long" - converts 32-bit addresses

Without them? Your server thinks port 80 is actually port 20480. Good luck debugging that.

The size of the variable depends on the architecture because different CPUs have different **word sizes**—basically, how much data they can chew through in one bite. A 16-bit CPU naturally works with 16-bit integers, a 32-bit CPU prefers 32-bit integers, and a 64-bit CPU can handle larger integers efficiently. It's all about what the CPU can process in one instruction cycle.

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

to be continued...

This chapter will be either really boring or really exciting to you, either way it's really important. 
We're going to be talking about C tooling (Compiler, Linker,Debugger)

```c
#include <stdio.h>
int main(){
    printf("Hello world!\n");
    return 0;
}
```

This is the only code we will write in this chapter. If you consider this elementary, in 2 minutes you won't.

I will be very sarcastic in this chapter, I promise you that it's not meant to make you feel belittled, but it's so that you as the reader can understand fully what we (including myself) have been taking for granted.

But this isn't even code right now—this is merely text. Your CPU doesn't understand text; it understands voltage patterns. Between this human-readable code and electrons flowing through silicon, there are multiple layers of translation. The compiler is your translator.

# Compiler

## WTF is a compiler?

This is a question I've thought I had the answers to several times, so let's look at the definition:

"A program that translates another program written in a high-level language into machine language so that it can be executed."

Seems simple enough, but let me ask you this: How is a compiler for a language written *before* the language exists?

Well, it's written in another language. Okay smart ass, but what if there are no other languages?

The first compiler was written in binary. The assembly compiler was written in fucking binary—by hand, with toggle switches and punch cards. While you can't center a div, Grace Hopper and her team were literally flipping bits to create the foundation of everything you use today.

This is called **bootstrapping**. Once you have an assembler, you can write a simple C compiler in assembly. Once you have that C compiler, you can rewrite it in C itself (which is exactly what happened).

## How it works

The compiler is essentially a 6-stage translator.

### Stage 1: Tokenization

You probably know what tokenization is (thank you Sam Altman), but if you don't, it's essentially dividing words smartly. The compiler "tokenizes" the words inside your C file into possible states. A state is essentially a way that data is, so a word can be a keyword (`int`, `bool`, `char`) or an operator (`+`, `-`, `/`, `*`) etc...

### Stage 2: Building the AST (Abstract Syntax Tree)

The parser takes your stream of tokens and organizes them into a hierarchical tree structure that represents the grammatical structure of your program.

From tokens to tree structure:
Your tokens: `[int] [x] [=] [10] [+] [y] [*] [3] [;]`

Gets organized into an AST like:
```
Assignment
├── Variable: x
└── BinaryOp: +
    ├── Number: 10
    └── BinaryOp: *
        ├── Variable: y
        └── Number: 3
```

**Important:** This is where syntax errors happen.

### Stage 3: Semantic Analysis

Ensures the program makes semantic sense - type checking, scope resolution, variable declarations, etc.

What it catches:
- Type mismatches (`int x = "hello"`)
- Undeclared variables
- Scope violations and function signature mismatches
- And many more things...

### Stage 4: Intermediate Code Generation 

This is where shit gets spicy.

**The core problem:** The AST is still too "high-level" and machine-independent. You need something that's:
- Easier to optimize than the AST
- Easier to translate to machine code than the AST
- But still machine-independent

**Enter Three-Address Code (TAC):**

Your AST:
```
Assignment
├── Variable: a
└── BinaryOp: +
    ├── Variable: b
    └── BinaryOp: *
        ├── Variable: c
        └── Variable: d
```

Gets "flattened" into TAC:
```
t1 = c * d      // temporary variable t1 holds intermediate result
t2 = b + t1     // temporary variable t2 holds next intermediate result  
a = t2          // final assignment
```

Why not go straight to machine code? Because with intermediate code, you only need one optimizer instead of N optimizers for N target machines.

### Stage 5: Compiler Optimizations (optional)

Improves efficiency by removing unnecessary operations and optimizing resource usage. Modern compilers do hundreds of optimization passes. *Cough, rust, cough.*

### Stage 6: Fucking finally, machine code...

Converts optimized intermediate code into target machine code, handling register allocation and instruction selection.

**Object files:** The output of this stage is an **object file** (`.o` on Unix, `.obj` on Windows) - basically machine code that's almost ready to run, except it has unresolved references to external functions and libraries. Think of it as a puzzle piece that knows its shape but doesn't know where it fits in the bigger picture.

## Let's actually see this in action

Time to get our hands dirty. Let's compile our hello world step by step and examine what the compiler actually produces:

### Step 1: Compile to object file
```bash
gcc -c hello.c -o hello.o
```
The `-c` flag tells GCC to compile but **not link** - we want to see the raw object file.

### Step 2: Examine the object file with objdump
```bash
objdump -d hello.o
```
This disassembles the object file and shows you the actual machine instructions your C code became.

You'll see something like:
```assembly
0000000000000000 <main>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   48 8d 05 00 00 00 00    lea    0x0(%rip),%rax
   b:   48 89 c7                mov    %rax,%rdi
   e:   e8 00 00 00 00          call   13 <main+0x13>
  13:   b8 00 00 00 00          mov    $0x0,%eax
  18:   5d                      pop    %rbp
  19:   c3                      ret
```

### Step 3: Check for undefined symbols
```bash
objdump -t hello.o | grep UND
```
This will show you all the **undefined symbols** - functions your code calls but aren't defined in this object file:
```
0000000000000000         *UND*  0000000000000000 puts
```

Wait, `puts`? Where's `printf`? 

That's GCC being sneaky - it optimized your `printf("Hello world!\n")` into `puts("Hello world!")` because `puts` is faster and simpler when you're just printing a string with no format specifiers. The compiler said "I can do this better" and just... did.

See that? Your object file **knows** it needs `puts` but has no fucking clue where it is. That's the linker's job.

---

Great, so that little seemingly harmless `#include <stdio.h>` actually requires the **linker**, which is a delightfully convoluted symbol-resolution nightmare that makes the compiler look like a simple calculator.

# Linker

This is the phase where your "simple" hello world becomes a clusterfuck of symbol resolution, memory layout, and relocations.

The linker has **two primary jobs**:

## 1. Symbol Resolution

Think of this like a massive matching game with millions of pieces.

**The Problem:**
- Your code says: "Hey, I want to call `printf`"
- The linker's job: "Find me the ONE CORRECT `printf` function out of thousands of symbols"

**Why it's complex:**
- Your hello world references 50+ symbols, real apps need 10,000+
- **Order matters**: `gcc main.o libA.a libB.a` gives different results than `gcc main.o libB.a libA.a`
- Static libraries only pull in functions you actually need, but figuring out what's "needed" requires scanning everything first
- Circular dependencies: libA needs libB, libB needs libA

## 2. Relocation

Your code thinks `main` is at address `0x0`, but it might actually load at `0x401000`.

The linker assigns real memory addresses to everything and updates every reference to use the new addresses.

## Static vs Dynamic Linking

### Static Linking
All library code gets copied into your executable.
- **Pro**: Self-contained, faster execution
- **Con**: Huge executables, memory waste

### Dynamic Linking
Libraries are loaded at runtime when needed.
- **Pro**: Smaller executables, shared memory
- **Con**: Runtime overhead, dependency hell

### Lazy Loading
Dynamic linking doesn't resolve all symbols at startup - symbols are only resolved when first called. Your program might reference 1000+ functions but only call 10% of them, so lazy loading = faster startup.

---

The linker is solving graph traversal problems with circular dependencies just to figure out "when I say `printf`, which actual function am I talking about?"
###

## Debugger

So, your program is crashing, segfaulting, or just doing weird shit. Life would be much easier if you were a 10x JavaScript developer—you could just slap `console.log()` everywhere and prompt AI to fix it. However, we find ourselves in a world where that doesn't work.

A debugger is essentially a program that monitors and controls the execution of another program.
But let's say you're a debugger and you want to control another process completely, with that kind of power, it's only time until you do something malevolent. You want to control another program's execution. But operating systems don't let random programs mess with each other -that would be chaos.

That is why the OS exposes this functionality throught the ptrace() syscall. For a deeper explanation on PTRACE and syscalls, check out [this excelent video](https://www.youtube.com/watch?v=engduNoI6DE&t)

## Getting Started with GDB

Let's compile our hello world program with debug information:

```bash
gcc -o 00 00.c -g
```

**That `-g` flag is crucial**—it tells the compiler to include debug symbols in the executable.
### Starting a Debug Session

```bash
❯ gdb hello
GNU gdb (GDB) 16.3
Copyright (C) 2024 Free Software Foundation, Inc.
...
Reading symbols from 00...
(gdb)
```

**What just happened:**
- GDB loaded your program (but didn't run it yet)
- It read the debug symbols from your executable
- You're now in GDB's command prompt, ready to set up debugging

### Setting Breakpoints and Assembly View

Let's say we're hunting for a bug. We know it's roughly in the `main` function, so we'll set a breakpoint there and examine the assembly:

```bash
(gdb) break main
Breakpoint 1 at 0x113d: file hello.c, line 4.
(gdb) layout next
```

**What these commands do:**
- `break main`: Sets a breakpoint at the start of the main function
- `layout next`: Switches to assembly view so you can see the actual machine instructions

## Understanding the GDB Interface

Looking at the screenshots:
[dbg1](../assets/dbg1.png)
**Image 1 (Before execution):**
- **Top panel**: Shows the assembly instructions of your program
- **Breakpoint marker**: The `B+>` indicates where execution will pause
- **Assembly breakdown**: You can see the actual x86-64 instructions your C code became
- **Key instructions**:
  - `push %rbp`: Setting up stack frame
  - `call puts@plt`: The actual call to `puts` (optimized from `printf`)
  - `mov $0x0,%eax`: Setting return value to 0

[dbg2](../assets/dbg2.png)
**Image 2 (After stepping):**
- **Program counter moved**: Notice the highlighting moved to the next instruction
- **Stack operations**: You can see the function prologue executing step by step
- **Real-time execution**: Each `next` command advances one C line, which corresponds to multiple lines of assembly 

## Why This Matters

**What you're seeing in these screenshots:**
1. **Your C code transformed**: The simple `printf` became multiple assembly instructions
2. **Real memory addresses**: Those `0x555555555xxx` addresses are where your code lives in memory
3. **CPU state**: You can see exactly what the processor is doing at each step

This level of detail is both overwhelming and essential. When your program mysteriously crashes, this is often the only way to figure out what the hell went wrong.ut it's incredibly powerful. Every C programmer needs to know basic GDB because **printf debugging only gets you so far**.

Modern IDEs like VS Code and CLion provide prettier interfaces, but they're all using the same underlying debugging principles.
We will learn more about GDB as we go but for now, this is enough.
# You finished the hardest chapter, congradulations.

Now, we'll write some code, I promise.

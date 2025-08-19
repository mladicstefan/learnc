This chapter will be either really boring or really exciting to you, either way it's really important. 
We're going to be talking about C tooling (Compiler, Linker,Debugger)

```c
#include <stdio.h>
int main(){
    printf("Hello world!\n");
    int x = 0;
    x = 2;
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
gcc -c 00.c -o 00.o
```
The `-c` flag tells GCC to compile but **not link** - we want to see the raw object file.

### Step 2: Examine the object file with objdump
```bash
objdump -d 00.o
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
objdump -t 00.o | grep UND
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

# Debugger

So, your program is crashing, segfaulting, or just doing weird shit. Life would be much easier if you were a 10x JavaScript developer—you could just slap `console.log()` everywhere and prompt AI to fix it. However, we find ourselves in a world where that doesn't work.

**When do you actually need a debugger?** When printf debugging becomes impossible - race conditions, memory corruption, crashes that happen deep in library code, or when you need to inspect the exact state of 50+ variables simultaneously. Basically, when your program is being a mysterious asshole.

A debugger is essentially a program that monitors and controls the execution of another program.

But let's say you're a debugger and you want to control another process completely—pause it, inspect its memory, modify variables, even inject code. With that kind of power, it's only a matter of time until you do something malevolent. Imagine if any random program could mess with your browser's memory or read your password manager's data. That would be chaos.

This is where CPU protection rings come into play. Your normal programs run in **Ring 3** (user space) with limited privileges—they can't directly access hardware or mess with other processes. The operating system kernel runs in **Ring 0** (kernel space) with god-like powers over the entire system. This separation is what keeps your computer from being a free-for-all nightmare.

But debuggers need to break these rules. They need Ring 0 privileges to control other processes. Since we can't just hand out kernel access to every program (security disaster), the OS provides a controlled gateway: the **ptrace() syscall**. This syscall lets debuggers do dangerous things, but only under strict supervision and with proper permissions.

When you run `gdb`, you're essentially asking the kernel: "Hey, I need to attach to this process and control it completely." The kernel checks if you have permission (usually you need to own the process or be root), and if so, it grants limited god-mode powers through ptrace(). While you can't center a div, at least you can completely dominate a process's memory space.

For a deeper explanation on PTRACE and syscalls, check out [this excellent video](https://www.youtube.com/watch?v=engduNoI6DE&t)

## Getting Started with GDB

Let's compile our hello world program with debug information:
```bash
gcc -o 00 00.c -g
```
**That `-g` flag is crucial**—it tells the compiler to include debug symbols (variable names, line numbers, function info) in the executable so GDB can map machine code back to your C source.

### Basic Workflow: Compile → Debug → Profit

```bash
# 1. Compile with debug info
gcc -o 00 00.c -g

# 2. Start GDB
gdb 00
```

```bash
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

## Essential GDB Commands and Workflow

### 1. Examining Your Code
```bash
(gdb) list
1	#include <stdio.h>
2	
3	int main(){
4	   printf("Hello world!\n");
5	   int x = 0;
6	   x = 2;
7	   return 0;
8	}
```
**`list`** (alias: `l`) - Shows your source code around the current location.

### 2. Setting Breakpoints
```bash
(gdb) break 4
Breakpoint 1 at 0x555555555141: file 00.c, line 4.
```
**`break`** (alias: `b`) - Sets a breakpoint at line 4. The program will pause here when you run it.

### 3. Running Your Program
```bash
(gdb) run
Starting program: /home/djamla/Documents/code/git/learnc/ch0-tooling/00
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/usr/lib/libthread_db.so.1".

Breakpoint 1, main () at 00.c:4
4	   printf("Hello world!\n");
```
**`run`** (alias: `r`) - Starts program execution. It stops at your breakpoint on line 4.

### 4. Stepping Through Code

#### Step Into Functions
```bash
(gdb) step
0x00007ffff7c82c96 in __GI__IO_puts (
    str=0x555555556004 "Hello world!") at ioputs.c:35
```
**`step`** (alias: `s`) - Steps into the `printf` function (which becomes `puts` under the hood). This dives deep into library code.

#### Step Over Functions
```bash
(gdb) next
5	   int x = 0;
```
**`next`** (alias: `n`) - Steps over function calls. Unlike `step`, this treats function calls as single operations and doesn't dive into library code.

#### Step Out of Functions  
```bash
(gdb) finish
Run till exit from #0  0x00007ffff7c82c96 in __GI__IO_puts (
    str=0x555555556004 "Hello world!") at ioputs.c:35
Hello world!
main () at 00.c:5
5	   int x = 0;
Value returned is $1 = 13
```
**`finish`** - Executes until the current function returns, bringing you back to your code. Notice it shows the return value (13 = length of "Hello world!\n").

### 5. Managing Breakpoints
```bash
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000555555555141 in main at 00.c:4
	breakpoint already hit 1 time

(gdb) delete 1
(gdb) info breakpoints
No breakpoints, watchpoints, tracepoints, or catchpoints.
```
**`info breakpoints`** - Lists all breakpoints with their IDs and status.
**`delete 1`** - Removes breakpoint #1 using the ID from `info breakpoints`.

### 6. Watchpoints - Monitor Variable Changes
```bash
(gdb) watch x
Hardware watchpoint 2: x

(gdb) continue
Continuing.

Hardware watchpoint 2: x

Old value = 32767
New value = 0
main () at 00.c:6
6	   x = 2;
```
**`watch x`** - Creates a watchpoint that triggers whenever variable `x` changes value. The watchpoint caught the change from garbage value (32767) to 0 when `int x = 0;` executed, and now we're stopped just BEFORE `x = 2;` runs.

### 7. Examining the Call Stack
```bash
(gdb) bt
#0  main () at 00.c:6
```
**`bt`** (backtrace) - Shows the call stack, which is the chain of function calls that led you to the current point. Think of it like breadcrumbs showing how you got here. If `main` called function A, which called function B, you'd see all three stacked up. Here we only see `main` because we're at the top level.

### 8. Printing Variables and Expressions
```bash
(gdb) p x
$2 = 0

(gdb) continue
Continuing.

Hardware watchpoint 2: x

Old value = 0
New value = 2
main () at 00.c:7
7	   return 0;

(gdb) p x
$3 = 2
```
**`print`** (alias: `p`) - Evaluates and displays variables or C expressions. Notice how `x` was 0 before the assignment and 2 after. The watchpoint caught both changes.

```bash
(gdb) p x + 10
$4 = 12
```
Just like in a Python REPL, you can calculate expressions on the fly.

## Essential Aliases

**`p`** = `print`, **`n`** = `next`, **`s`** = `step`, **`c`** = `continue`, **`r`** = `run`, **`l`** = `list`, **`b`** = `break`

## Why This Matters

When your program mysteriously crashes, printf debugging only gets you so far. GDB lets you pause execution exactly where problems occur, inspect memory and variables in real-time, and see the exact sequence of function calls.

This level of control is both overwhelming and essential. Every C programmer needs basic GDB skills because it's often the only way to figure out what the hell went wrong.

Modern IDEs like VS Code and CLion provide prettier interfaces, but they're all using the same underlying debugging principles.

We will learn more about GDB as we go but for now, this is enough.

# You finished the hardest chapter, congratulations.
Now, we'll write some code, I promise.

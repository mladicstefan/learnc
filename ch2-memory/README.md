# Memory in C and Computer Architecture

Let me preface this by saying: **THIS CHAPTER IS SCARY.** I'm telling you this to prepare you for the overwhelming possibility of you not getting it on the first pass. Do not worry, this is normal. Knowing the topics of this chapter in this amount of detail is something you should be proud of. But then again, when you think about it long enough and get used to thinking in these terms, it really isn't that complex.

Today we will be talking about memory in C and wider computer architecture.

---

## Computers Do Beep Boop Beep and Programs Do Beep Boop Bam

I'm assuming you've heard of the stack and the heap. If not, reference [this cppreference guide](https://www.learncpp.com/cpp-tutorial/the-stack-and-the-heap/).

A stack data structure works similarly to an array, except that you cannot access things at random - you can only access things on the top. You can push values onto the stack or pop off the most recent ones. This is how your computer executes code. 

**The stack pointer** is a special register that keeps track of where the "top" of the stack currently is. Think of it like a bookmark that always points to the most recently added item. When you push something onto the stack, the stack pointer moves to point to the new top item. When you pop something off, it moves back to point to the previous item. The CPU uses this pointer to know exactly where to put new data or where to find the most recent data.

Let's take this code for example:

```c 
void main(){
    func1();
    func2();
    func3();
}
```

Here's how the function calls work as a tree structure:

```
FUNCTION CALL TREE:

main()
├── func1() ──┐
│             │ (executes func1 instructions)
│             │ (returns to main)
│             └── return to main
│
├── func2() ──┐  
│             │ (executes func2 instructions)
│             │ (returns to main)
│             └── return to main
│
└── func3() ──┐
              │ (executes func3 instructions)
              │ (returns to main)
              └── return to main

EXECUTION FLOW:
1. Start at main()
2. Branch to func1() ────┐
                         │ Do func1 stuff
                         └─ Return to main()
3. Branch to func2() ────┐
                         │ Do func2 stuff  
                         └─ Return to main()
4. Branch to func3() ────┐
                         │ Do func3 stuff
                         └─ Return to main()
5. main() finishes, program ends
```

Effectively, each function in assembly pushes its instructions onto the stack, plus the return address. This is called a **stack frame**. You can remember it as a "function frame" although it's not the official terminology.

**When a function is called:**
1. **Push** a new stack frame with the return address
2. **Execute** the function's instructions  
3. **Pop** the stack frame and jump back to the return address

---

## The Memory Hierarchy: From Lightning Fast to Molasses Slow

Let's examine all the memory our computer has, from fastest to slowest:

| **Memory Type** | **Speed** | **Size** | **Location** |
|---|---|---|---|
| **Registers** | Fastest | Smallest | Literally inside the CPU |
| **L1, L2, L3 Cache** | Very fast | Small | On or very close to the CPU |
| **RAM** | Fast enough | Medium | System memory |
| **Hard Drive/SSD** | Slowest | Largest | Storage device |

Registers being the fastest is because they are closest (actual distance on the chip) to the ALU (the unit responsible for processing). This is called **cache-locality**.

> **Important note about the stack:** The call stack physically lives in RAM, but the CPU constantly caches recently accessed stack data in L1/L2/L3 cache for speed. The CPU automatically caches the most recently accessed stack data in its cache levels. Since stack access follows predictable patterns (LIFO - Last In, First Out), the CPU's cache is very effective at keeping "hot" stack data close to the CPU. So while stack frames are stored in RAM, the parts you're actively using get pulled into faster cache automatically.

---

## Assembly: The Scary Monster That's Actually Just Data Movement

**PLEASE, DO NOT CLICK OFF. I SWEAR IT'S NOT SCARY. NOOOOOO!!!!!!!**

If I wasn't the one writing this, I would've probably clicked off at this point. But if you're actually reading this, congratulations.

We need to understand a slight bit of assembly to understand a core aspect of memory: **registers**.

Assembly is compiled garbage that seems impossible to understand until you frame it like this: In any other programming language, you would invoke a function by passing variables (which are just names for memory locations) as arguments. But what that really does underneath is move your data into registers (the fastest memory there is since it's directly in the CPU) and put a return address on the stack so the CPU knows where to go back to when the function is done. 

A function call in assembly is just: **put the data in the right registers, jump to the function's instructions, do the work, then jump back.** So the values need to be in the registers for the CPU to actually work with them - everything else is just organizing this data movement and keeping track of where to jump next.

### Seeing Assembly in Action

Let's look at a simple C function and then disassemble it to Assembly.

**C Code:**
```c
int add_two(int a, int b){
  return a + b;
}

void main() {
    int res = add_two(1,2);
    int x = 2 + 2; //just for making a breakpoint here :)
}
```

**Let's examine the assembly!**
```bash 
(gdb) layout asm
```

**Assembly Output (from GDB):**
```asm
0x1119 <add_two>        push   %rbp                    
0x111a <add_two+1>      mov    %rsp,%rbp               
0x111d <add_two+4>      mov    %edi,-0x4(%rbp)         
0x1120 <add_two+7>      mov    %esi,-0x8(%rbp)         
0x1123 <add_two+10>     mov    -0x4(%rbp),%edx         
0x1126 <add_two+13>     mov    -0x8(%rbp),%eax         
0x1129 <add_two+16>     add    %edx,%eax               
0x112b <add_two+18>     pop    %rbp                    
0x112c <add_two+19>     ret                            
```

### Step-by-Step Breakdown

1. **`push %rbp`** - Save the old stack frame by pushing (assembly operation that decrements stack pointer and stores value) the current base pointer to the `%rbp` register (register for maintaining the current function's stack frame base address).

2. **`mov %rsp,%rbp`** - Set up the new stack frame by doing mov (which is move/copy operation) to copy the stack pointer `%rsp` (register that points to the top of the stack) into `%rbp`, establishing this function's frame boundary.

3. **`mov %edi,-0x4(%rbp)`** - Store the first argument by doing mov to copy from `%edi` register (register for the first function argument in x86-64 calling convention) to stack memory location 4 bytes below the base pointer. 

4. **`mov %esi,-0x8(%rbp)`** - Store the second argument by doing mov to copy from `%esi` register (register for the second function argument in x86-64 calling convention) to stack memory location 8 bytes below the base pointer.

5. **`mov -0x4(%rbp),%edx`** - Load the first argument by doing mov to copy from stack memory (where we stored 'a') into `%edx` register (general-purpose register used for data manipulation).

6. **`mov -0x8(%rbp),%eax`** - Load the second argument by doing mov to copy from stack memory (where we stored 'b') into `%eax` register (accumulator register, also used for function return values).

7. **`add %edx,%eax`** - Perform the addition by doing add (arithmetic operation that adds source to destination) with `%edx` as source and `%eax` as destination, storing the result in `%eax`.

8. **`pop %rbp`** - Restore the old stack frame by doing pop (assembly operation that loads value from stack top and increments stack pointer) to restore the previous function's base pointer from `%rbp` register.

9. **`ret`** - Return to the caller by doing ret (return instruction that pops return address from stack and jumps to it), with the result already in `%eax` following x86-64 calling convention.

### But Where Are The Numbers?!

Since we're working with 32-bit ints (4 bytes), if we increment the stack pointer by 4 bytes, that's equal to 1, and by eight is... you guessed it... **TWO!!!!!**

> **Note:** You'll see `%edi/%esi` in assembly but `rdi/rsi` in GDB. They're the same physical registers - `%edi` is the 32-bit version, `rdi` is the full 64-bit version. Same register, different bit widths.

---

## Let's See This in Action! Step-by-Step GDB Guide

### 1. Compile with debug symbols
```bash
gcc -g -o addtwo add_two.c
```

### 2. Start GDB and set breakpoint
```bash
gdb ./addtwo
(gdb) break 10        # Break at line 10 (after function call)
(gdb) run
```

### 3. Inspect the registers after function execution
```bash
(gdb) info registers
```

### Register Output After `add_two(1,2)` Completes

```
rax            0x3                 3          # Return value (1+2=3)
rbx            0x0                 0
rcx            0x555555557e18      93824992247320
rdx            0x1                 1          # Used during calculation
rsi            0x2                 2          # Second argument (b)
rdi            0x1                 1          # First argument (a)
rbp            0x7fffffffdac0      0x7fffffffdac0  # Stack frame pointer
rsp            0x7fffffffdab0      0x7fffffffdab0  # Stack pointer
```

**Key observations:**
- `rdi = 1` (first argument `a`)
- `rsi = 2` (second argument `b`) 
- `rax = 3` (return value: 1 + 2 = 3)

> **Important:** Registers aren't always enough! When you have more arguments than available registers, or large local variables, or complex data structures, the CPU has to use the stack (which is in RAM). This is why you see those `mov` instructions putting values on the stack (`-0x4(%rbp)`, `-0x8(%rbp)`) - the compiler is managing limited register space.

---
## Memory Management

Okay, so we've established that the control of whether memory lives in registers, CPU cache, or RAM is abstracted away from us and not in our control (thank god). What we **do** have control of in C is how we allocate our program's memory that lives in RAM.

Memory in RAM is stored on the **Stack** (the one we just talked about) or the **Heap** (we'll get into it very soon). But before we dive into Stack vs Heap, we gotta clarify something important.

### Virtual Memory: Your Program's Safety Net

We do **not** store our memory directly on the RAM - that would be unsafe (we could overwrite or peek into another process's memory). This is one of the core functions of the **kernel**. The kernel decides how much memory a program can access by assigning it **virtual memory**. 

Think of it like that friend who is drunk but can still walk - they just need you to hold them upright and guide them so they don't crash into things or wander into someone else's house. The kernel is you, your program is the drunk friend, and virtual memory is the safe path you create to keep them from causing chaos.

**What virtual memory does:**
- **Isolation** - Your program can't accidentally access another program's memory
- **Protection** - Other programs can't mess with your memory
- **Abstraction** - Your program thinks it has access to a huge, continuous block of memory
- **Safety** - The kernel manages the actual physical memory mapping behind the scenes

Now that we've got that safety net established, let's talk about how your program actually uses this protected memory space...

## Memory Management

Okay, so we've established that the control of whether memory lives in registers, CPU cache, or RAM is abstracted away from us and not in our control (thank god). What we **do** have control of in C is how we allocate our program's memory that lives in RAM.

Memory in RAM is stored on the **Stack** (the one we just talked about) or the **Heap** (we'll get into it very soon). But before we dive into Stack vs Heap, we gotta clarify something important.

### Virtual Memory 

We do **not** store our memory directly on the RAM - that would be unsafe (we could overwrite or peek into another process's memory). This is one of the core functions of the **kernel**. The kernel decides how much memory a program can access by assigning it **virtual memory**. 

Think of it like that friend who is drunk but can still walk - they just need you to hold them upright and guide them so they don't crash into things or wander into someone else's house. The kernel is you, your program is the drunk friend, and virtual memory is the safe path you create to keep them from causing chaos.

**What virtual memory does:**
- **Isolation** - Your program can't accidentally access another program's memory
- **Protection** - Other programs can't mess with your memory
- **Abstraction** - Your program thinks it has access to a huge, continuous block of memory
- **Safety** - The kernel manages the actual physical memory mapping behind the scenes

## Stack 

We've already established that stack allocation is **faster**, **simpler**, **implicit**, and pretty much **automatic**. Stack Memory Management is directly managed by **scope**. 

### How Stack Allocation Works

Let's look at an example:

```c 
void some_func() {
    int x = 2;           // x allocated on stack
    {                    // New scope begins
        int y = 10;      // y allocated on stack
        char buffer[100]; // buffer allocated on stack
        // Stack grows...
    }                    // Scope ends - y and buffer AUTOMATICALLY deallocated!
    
    printf("%d", x);     // x still exists, y and buffer are GONE
}                        // x gets deallocated when function scope ends
``` 

**Scopes can be:**
- Functions
- Regular `{ }` blocks
- Loop bodies
- If/else statements

> **Remember:** Some parts of the stack might get optimized into registers by the compiler for even faster access. The compiler decides this automatically based on usage patterns and register availability.

### Stack Limitations

Among other things, the size of the memory that you allocate on the stack needs to be known at compile time.Therefore it cannot grow and shrink like the heap.
```c 
void stack_problems() {
    int size;
    printf("How many numbers do you want to store? ");
    scanf("%d", &size);
    
    int array[size];  // ERROR: size isn't a compile-time constant
}
```

The compiler looks at this and says "I have no fucking clue how much memory to reserve." Stack allocation happens during compilation - the compiler needs to know exactly how much space each function will need.

**Stack Overflow** - The stack has limited size (usually ~8MB on most systems):

```c
void dangerous_function() {
    char huge_array[10000000]; // This will likely cause a stack overflow!
    // Your program will crash - the stack ran out of space
}
```

When you exceed the stack limit, your program crashes with a **stack overflow** error. This is why massive arrays or deeply recursive functions can be dangerous.


> **Fun fact:** NASA's Jet Propulsion Laboratory follows "The Power of 10" coding rules, and Rule #3 explicitly states "Do not use dynamic memory allocation after initialization." This means NASA almost always uses stack allocation in their C code for spacecraft software! They avoid heap allocation because memory allocators like malloc can have unpredictable behavior that can impact performance, and memory errors are unacceptable when you're controlling a $2.5 billion rover on Mars. Read more about [NASA's 10 coding rules here](https://www.perforce.com/blog/kw/NASA-rules-for-developing-safety-critical-code).

---

## Heap

I hate when complex language features get treated like wizardry. Heap allocation is definitely one of them. Why don't people just frame it like this: "Some programmer way back had to solve a specific problem."

**HEAP ALLOCATION IS JUST AN ENGINEERING SOLUTION.** It doesn't mean C's heap allocation is the **BEST** solution (it isn't—*cough* Rust *cough*).

So instead of memorizing "oh I have to use malloc() for this," let's examine the heap problem from first principles.

## The Problem

- **I don't know how much data will be here!** (Size is determined at runtime)
- **It's too big!** (that's what she said)
- **I need it to shrink and grow** ( :( )

That's where HEAP allocation comes in. The heap lives entirely in RAM (no automatic caching into CPU like stack) and involves invoking a syscall which means there's overhead(refer to malloc section in this chapter). The big advantage though, is that you solve these three problems.

So how do we allocate stuff on the heap in C? Let's examine `pointers.c`:

## Let's Debug This Step by Step

Running this through GDB shows exactly what's happening with heap allocation and pointer passing:

### Program Output
```
Original value: 0
Pass by value: 0x2a
After pass_by_value: 0
Pass by pointer: 0x5555555592a0
After pass_by_pointer: 42
```

### GDB Analysis at Breakpoint
```bash
(gdb) p typed
$1 = (int *) 0x5555555592a0

(gdb) p *typed
$2 = 42
```

### What Actually Happened

**1. Heap allocation:** `malloc(sizeof(int))` allocated 4 bytes at address `0x5555555592a0`

**2. Pass by value:** Passed the VALUE (0) to the function, created a local copy at stack address `0x2a`. Modifying the copy doesn't affect the original.

**3. Pass by pointer:** Passed the HEAP ADDRESS to the function. `*x = 42` directly modified the heap memory, changing the original value.

Notice the addresses:
- **Stack address**: `0x2a` (local variable)
- **Heap address**: `0x5555555592a0` (malloc'd memory)

Pass-by-value creates a stack copy in completely different memory than the heap allocation.

### Understanding Malloc, Calloc, Realloc, Free

> Note: You can type in `man malloc` for the best explanation (if you're used to linux manuals) for this topic

#### Malloc
```c 
void *malloc(size_t size);
```

This is the function signature for malloc. It returns a `void*` which is a generic pointer, since malloc has no fucking clue what you want to use it for. The argument `size` is of type `size_t` which is just a standard unsigned integer type for sizes. It's important to note that malloc doesn't initialize the value in the memory (doesn't assign values) - it just reserves it so that you can write stuff in that virtual memory space via pointer dereferencing.

Underneath the hood, malloc uses `mmap()` (which maps the bytes in virtual memory) and `sbrk()` (requests more RAM from the kernel). This is the overhead we mentioned previously.

Of course, every `malloc()` must have its corresponding `free()`.

#### Unsafe Behaviour

- **Forgot free():** Can cause memory leaks (not heap overflow - that's different)
- **Use after free:** Undefined behaviour - you're accessing memory that might have been given to someone else
- **Double malloc:** Overwriting a pointer without freeing the original memory causes memory leaks
- **Accessing out of bounds:** malloc doesn't give you guard rails - you can write past your allocated space and corrupt other memory

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;
free(ptr);
*ptr = 99;  // USE AFTER FREE - undefined behaviour!

// Meanwhile, some other part of your program:
char *str = malloc(10);  // Might get the same memory address!
// Now your int write might corrupt the string data
```

> Note: Undefined behaviour doesn't strictly mean security vulnerability. It means that it is literally undefined - a lot of things can happen, including crashes, logic bugs, memory vulnerabilities, etc.

#### Fun Fact: Malloc Can Silently Fail

> By default, Linux follows an optimistic memory allocation strategy. This means that when malloc() returns non-NULL there is no guarantee that the memory really is available. In case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer. For more information, see the description of /proc/sys/vm/overcommit_memory and /proc/sys/vm/oom_adj in proc(5), and the Linux kernel source file Documentation/vm/overcommit-accounting.rst.

Wait... what do you mean there is no guarantee memory is really available??? **ISN'T THAT THE ONLY POINT?!?!??!!**

##### Virtual Memory ≠ Physical Memory

Virtual memory is just address space - it's like handing out apartment numbers before you've actually built the apartments. Physical memory (RAM) is when you actually pour the concrete.

**VIRTUAL ADDRESS SPACES (Each Process Gets Its Own "Map"):**

```
Process A's View:          Process B's View:
┌─────────────────┐        ┌─────────────────┐
│ 0x1000000       │        │ 0x1000000       │
│ 0x1000004       │        │ 0x1000004       │  
│ 0x1000008       │        │ 0x1000008       │
│      ...        │        │      ...        │
│ 0x40000000      │        │ 0x40000000      │
└─────────────────┘        └─────────────────┘
        │                          │
        │    KERNEL TRANSLATION    │
        ▼                          ▼
┌─────────────────────────────────────────┐
│        ACTUAL PHYSICAL RAM              │
│  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐   │
│  │ A's │  │ B's │  │ A's │  │ B's │   │
│  │Page │  │Page │  │Page │  │Page │   │
│  └─────┘  └─────┘  └─────┘  └─────┘   │
└─────────────────────────────────────────┘
```

Both processes can have the same virtual addresses, but the kernel maps them to completely different physical locations.

##### The Optimistic Lying Problem

Let's check what the malloc man page actually says:

*"By default, Linux follows an optimistic memory allocation strategy. This means that when malloc() returns non-NULL there is no guarantee that the memory really is available. In case it turns out that the system is out of memory, one or more processes will be killed by the OOM killer."*

**OH HELL NO!** This isn't silent failure - this is optimistic lying followed by murder.

Linux practices **memory overcommitment** - it's basically running a Ponzi scheme with memory:

```c
int *huge = malloc(1000000 * sizeof(int));  // Ask for ~4MB
                                           // Virtual: "Sure, here's 4MB of address space!"
                                           // Physical: 0 bytes actually allocated

huge[0] = 42;           // Write to first int
                       // Physical: Linux allocates 1 page (4KB) of real RAM
                       // (a page is the smallest chunk of memory a process can have)

huge[250000] = 99;      // Write to middle of array (~1MB in)  
                       // Physical: Linux allocates another page (4KB)
                       // Total physical RAM used: 8KB out of 4MB requested
```

Linux gives you a 4MB address range but only allocates 4KB pages as you actually touch memory. Most of your "allocated" memory exists only as virtual addresses with no physical RAM backing them.

##### When The Ponzi Scheme Collapses

```c
// Process A suddenly decides to use everything
memset(ptr_A, 1, 1000000 * sizeof(int));  // Linux: "Fuck, I need 4MB of real RAM"

// Process B also wants everything
memset(ptr_B, 1, 1000000 * sizeof(int));  // Linux: "I only have 2MB left... TIME TO KILL!"
```

The OOM killer murders Process B because Linux wrote checks it couldn't cash.

The functions don't fail silently - they lie to your face and then murder your process later. It's like a restaurant taking your order for 50 pizzas, saying "sure, no problem!" then calling the cops when you show up to collect.

Linux gambles that you won't actually touch all the memory you requested. When it loses this bet, your process gets a friendly message:

```bash
Killed
```

No error handling. No graceful degradation. Just death.

#### Free

```c 
void free(void* ptr);
```

Free is self explanatory (which means I am lazy so I will copy the linux man page explanation):

> The free() function frees the memory space pointed to by ptr, which must have been returned by a previous call to malloc() or related functions. Otherwise, or if ptr has already been freed, undefined behavior occurs. If ptr is NULL, no operation is performed.

#### Unsafe Behaviour

- **Double Free():** Calling `free()` twice on the same pointer causes undefined behaviour - the memory allocator gets confused about what's allocated vs what's free
- **Freeing stack memory:** Only free memory that was allocated with malloc/calloc/realloc - freeing stack variables will crash your program
- **Memory leaks:** Forgetting to call `free()` means that memory stays "reserved" even though you're not using it anymore

```c
int *ptr = malloc(sizeof(int));
free(ptr);
free(ptr);  // DOUBLE FREE - undefined behaviour!

// Or the classic leak:
void leaky_function() {
    int *data = malloc(1000 * sizeof(int));
    // Do some work...
    return;  // OOPS! Forgot to free(data) - memory leak!
}
```


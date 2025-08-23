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


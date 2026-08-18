# Day 4: The Registers and What They Hold

## Why this matters

Registers are the CPU's working state. A debugger reads every one of them (through a command
called `GETREGS`), rewinds one specific register (`rip`) every single time a breakpoint fires,
and reads others (`rbp`, `rsp`, the argument registers) to walk stacks and locate variables.
None of that is possible without first being able to name the registers and say what each one
is for.

---

## 1. What a register is

Registers are a small set of extremely fast storage slots that live *inside* the CPU itself, as
opposed to out in main memory. x86-64 has 16 general-purpose 64-bit registers, plus two special
ones covered below: `rip` and the flags register.

## 2. Sub-registers: the same slot, viewed narrower

Each general-purpose register has smaller "views" of itself, all sharing the same physical
storage:

```
rax (64-bit) -> eax (low 32 bits) -> ax (low 16 bits) -> al (low 8 bits)
```

Compilers commonly use the 32-bit names (`eax`, `edi`, `esi`, ...) when working with C `int`
values, since an `int` is 4 bytes. That's why disassembly of code passing plain integers shows
`edi`/`esi` rather than `rdi`/`rsi`, even though `rdi`/`rsi` are the "full" 64-bit registers.

## 3. The special-purpose registers

- **`rip`**, the instruction pointer: holds the address of the *next* instruction the CPU is
  about to execute. This is the single most important register to a debugger. When a breakpoint
  fires, `rip` gets rewound; to resume execution somewhere specific, `rip` gets set.
- **`rsp`**, the stack pointer: holds the address of the current top of the stack (the lowest
  in-use address on the stack, since the stack grows downward).
- **`rbp`**, the base/frame pointer: a fixed anchor point within the current function's stack
  frame, established by that function's prologue. Following the chain of saved `rbp` values is
  how a debugger walks the call stack.
- **`rax`**: holds a function's return value; otherwise general-purpose.
- **`eflags`**: not one value but a set of individual bits (zero flag, sign flag, carry flag,
  overflow flag, and more), set by comparisons and arithmetic, and read by conditional jumps to
  decide whether to branch.

## 4. Why this matters for a debugger

`GETREGS` hands back this entire set of registers at once. Every time a breakpoint (a `0xCC`
trap) fires, `rip` gets rewound by one byte to point back at the instruction that was actually
supposed to run. Walking the stack means following `rbp`/`rsp`. Reading a function's arguments
means reading `rdi`/`rsi`/etc, which Day 9 covers in detail.

---

# 

---

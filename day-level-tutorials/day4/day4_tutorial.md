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

Compilers commonly use the 32-bit names (`eax`, `edi`, `esi`, ...) when working with C `int`
values, since an `int` is 4 bytes. That's why disassembly of code passing plain integers shows
`edi`/`esi` rather than `rdi`/`rsi`, even though `rdi`/`rsi` are the "full" 64-bit registers.

## 2. All 16 general-purpose registers

| Register | Common use                     |
| -------- | ------------------------------ |
| rax      | return value / general-purpose |
| rbx      | general-purpose (callee-saved) |
| rcx      | 4th integer/pointer argument   |
| rdx      | 3rd integer/pointer argument   |
| rsi      | 2nd integer/pointer argument   |
| rdi      | 1st integer/pointer argument   |
| rbp      | base/frame pointer             |
| rsp      | stack pointer                  |
| r8       | 5th integer/pointer argument   |
| r9       | 6th integer/pointer argument   |
| r10      | general-purpose (scratch)      |
| r11      | general-purpose (scratch)      |
| r12      | general-purpose (callee-saved) |
| r13      | general-purpose (callee-saved) |
| r14      | general-purpose (callee-saved) |
| r15      | general-purpose (callee-saved) |

The argument order above (`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`) is the System V calling
convention, covered in day 6.

## 3. The two special-purpose registers, and the general-purpose ones a debugger leans on most

Only two registers sit outside the 16 general-purpose ones:

- **`rip`**, the instruction pointer: holds the address of the *next* instruction the CPU is
  about to execute. This is the single most important register to a debugger. When a breakpoint
  fires, `rip` gets rewound; to resume execution somewhere specific, `rip` gets set.

- **`eflags`**: not one value but a set of individual bits, each one a separate flag. Arithmetic
  and comparison instructions (`cmp`, `sub`, `add`, `test`, ...) set these bits as a side effect,
  and conditional jumps (`je`, `jg`, `jle`, ...) read them back to decide whether to branch. The
  ones that matter most day to day:
  
  | Flag | Bit | Name            | Set when                                                                                                                      |
  | ---- | --- | --------------- | ----------------------------------------------------------------------------------------------------------------------------- |
  | ZF   | 6   | Zero flag       | the result of the last op was exactly `0`                                                                                     |
  | SF   | 7   | Sign flag       | the result's most-significant bit is `1` (i.e. negative, as a signed value)                                                   |
  | CF   | 0   | Carry flag      | the last op carried out of (or borrowed into) the top bit — an unsigned overflow                                              |
  | OF   | 11  | Overflow flag   | the last op overflowed as a **signed** value (result's sign doesn't match what the true math result would be)                 |
  | PF   | 2   | Parity flag     | the low byte of the result has an even number of `1` bits                                                                     |
  | AF   | 4   | Auxiliary carry | a carry/borrow happened between bit 3 and bit 4 (used internally by BCD instructions)                                         |
  | TF   | 8   | Trap flag       | when set, the CPU raises a debug exception after every single instruction — this is the bit single-step debugging is built on |
  | IF   | 9   | Interrupt flag  | whether maskable hardware interrupts are currently allowed                                                                    |
  | DF   | 10  | Direction flag  | whether string instructions (`movs`, `stos`, ...) walk memory forward or backward                                             |
  
  `cmp a, b` (which is a `sub` that discards the result) is the instruction to build intuition
  from: it sets ZF/SF/CF/OF exactly as `a - b` would, and every conditional jump is really just a
  check on some combination of those four bits. For example `je`/`jz` branches when `ZF=1` (the
  values were equal), and `jg` (signed "greater than") branches when `ZF=0` **and** `SF=OF`.

3 of the 16 general-purpose registers carry a conventional role a
debugger depends on constantly:

- **`rsp`**, the stack pointer: holds the address of the current top of the stack (the lowest
  in-use address on the stack, since the stack grows downward).
- **`rbp`**, the base/frame pointer: a fixed anchor point within the current function's stack
  frame, established by that function's prologue. Following the chain of saved `rbp` values is
  how a debugger walks the call stack.
- **`rax`**: holds a function's return value; otherwise general-purpose.

## 4. Sub-registers: the same slot, a subset/piece of the register

Each general-purpose register has smaller "views" of itself, all sharing the same physical
storage:

```
rax (64-bit) -> eax (low 32 bits) -> ax (low 16 bits) -> al (low 8 bits)
```

---

## Why this matters for a debugger

`GETREGS` hands back this entire set of registers at once. Every time a breakpoint (a `0xCC`
trap) fires, `rip` gets rewound by one byte to point back at the instruction that was actually
supposed to run. Walking the stack means following `rbp`/`rsp`. Reading a function's arguments
means reading `rdi`/`rsi`/etc.

---

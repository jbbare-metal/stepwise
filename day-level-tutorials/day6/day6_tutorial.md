# Day 6: The System V Calling Convention

## Why this matters

This is the contract that says exactly *where* a function's arguments live and where its answer
comes back. It how it becomes possible
to know that the first argument is sitting in `rdi` and the second is in `rsi`.

## 1. What is a calling convention

When one function calls another, there has to be an agreed-upon place for arguments to go and for
the return value to come back, otherwise no two pieces of compiled code could ever call each
other reliably. On Linux, that agreement is the **System V AMD64 ABI(Application BInary Interface)**, and every compiler on the Linux x86-64 architecture follows it.

## 2. Where arguments go

Integer and pointer arguments  go directly into registers, in this exact order:

| Argument position | Register |
| ----------------- | -------- |
| 1st               | rdi      |
| 2nd               | rsi      |
| 3rd               | rdx      |
| 4th               | rcx      |
| 5th               | r8       |
| 6th               | r9       |

A 7th argument (and beyond) gets pushed onto the stack. Floating-point arguments follow a separate convention using `xmm0` through `xmm7`.

## 3. Where the return value goes

A function's return value comes back in **`rax`** (or, for values too large for one register,
split across `rdx:rax`).

## 4. Caller-saved vs. callee-saved

Not every register is free to just be overwritten across a call:

| Group        | Registers                       | Rule                                                                                                                             |
| ------------ | ------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| Caller-saved | rax, rcx, rdx, rsi, rdi, r8-r11 | the callee is free to overwrite these; if the caller needs one preserved across the call, the caller saves it first              |
| Callee-saved | rbx, rbp, r12-r15               | a function using one of these for its own internal purpose must save the caller's original value and restore it before returning |

This is why stack traces work: `rbp` is callee-saved, so its value chains predictably from
frame to frame, and a debugger can walk that chain to reconstruct the call stack.

## Why this matters for a debugger

Stopped at a breakpoint placed right after a function's prologue (say, inside `add(2, 3)`), the
first argument (`2`) is sitting in `rdi` (or its 32-bit view, `edi`), and the second (`3`) is in
`rsi` (`esi`), the calling convention guarantees "Print the function's
arguments," and later, full DWARF-based variable inspection.

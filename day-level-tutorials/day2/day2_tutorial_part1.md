# Day 2 Part 1: A Pointer Is Just an Address

## Why this matters

A debugger works entirely in terms of addresses: the address of a breakpoint, the address of a
variable, the address of a stack frame. A pointer is that same idea, a memory address(seen as a variable of type pointer in C) that stores a memory address. 

---

## 1. Memory as one long array

Picture memory as one enormous array of bytes. Every byte has a number: its **address**. A
regular variable, like `int x`, starts at some address and takes up some number of bytes (4, for
an `int`).

A **pointer** is a variable that doesn't hold ordinary data: it holds an address.

```c
int x = 42;
int *p = &x;
```

Here `x` holds the value `42`. `p` holds `&x`, meaning "the address where `x` starts." `p` is not
`42`; `p` stores the location of x.

## 2. Dereferencing: following the address

**Dereferencing** a pointer, written `*p`, means "go to the address stored in `p`, and read the value there.

- `*p` reads the value at that address: in the example above, `42`.
- `*p = 99` writes `99` into the memory at that address, which changes `x` itself, since `p`
  points at `x`.

## 3. Every pointer is 8 bytes on x86-64

The *type* of a pointer, `int *`, `char *`, `struct foo *`, doesn't change the pointer's own
size. On x86-64, every pointer, no matter what it points to, is **8 bytes or 64 bits**, because addresses on
this architecture are 64-bit numbers.

The type only tells the compiler two things: how many bytes to read starting at that address, and
how to interpret those bytes once read. `int *` means "read 4 bytes and treat them as an `int`."
`char *` means "read 1 byte and treat it as a `char`."

A pointer is itself stored somewhere in memory, so it has its own address too: `&p` is valid and
is, itself, just another address.

## 4. Why this matters for a debugger

Later, commands like `break 0x401136` or "show me the local variable `x`" boil down to the exact
same idea as `p` in the example above: a 64-bit number naming a location in the tracee's memory.
Reading a variable's value in a debugger is exactly the same operation as `*p`: go to an address,
read some bytes, interpret them by type.

---

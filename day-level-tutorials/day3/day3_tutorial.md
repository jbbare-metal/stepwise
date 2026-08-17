# Day 3: The Memory Regions: Text, Data, Heap, Stack

## Why this matters

Every action a debugger takes touches one specific region of memory. Breakpoints patch code.
Backtraces and local variables read the stack. Globals live somewhere else again. Knowing the
map of a running program tells us exactly *where* each debugger feature has to operate.

---

## 1. The four regions, at a glance

| Region | Holds | Notes |
|---|---|---|
| **`.text`** | machine instructions (the compiled code) | read + execute, not normally writable |
| **`.data` / `.bss`** | global and `static` variables | `.data` = initialized, `.bss` = zero-initialized |
| **heap** | dynamically allocated memory (`malloc`) | grows toward **higher** addresses |
| **stack** | local variables, saved registers, return addresses | grows toward **lower** addresses |

## 2. Walking through each one

**`.text`**: this is the actual machine instructions a compiler produced from the source code.
It's normally read-and-execute only, not writable, which is exactly why planting a breakpoint
(which needs to write one byte into `.text`) is a special operation done through `ptrace` rather
than an ordinary memory write.

**`.data` and `.bss`**: a global like `int g = 5;` is initialized, so its starting value has to
be stored somewhere in the executable file: that's `.data`. A global like `int g;` (no
initializer) is implicitly zero, so there's no need to store any bytes for it in the file at all:
the loader just zeroes out the right amount of space at load time. That's `.bss`, and it's why
`.bss` can be smaller on disk than it is in memory: the file only records its size, not its
content.

**heap**: memory requested at runtime through `malloc` (and released with `free`) comes from the
heap. As a program allocates more, the heap grows toward **higher** addresses.

**stack**: every function call gets a frame on the stack holding its local variables, its saved
registers, and the address to return to when it's done. Frames are pushed and popped through
`push`/`pop` and the `rsp` register, and the stack grows toward **lower** addresses as it fills
up.

## 3. Why this matters for a debugger

- Setting a breakpoint means writing into `.text`.
- Printing a backtrace, or a local variable, means reading the **stack**.
- Printing a global variable means reading `.data` or `.bss`.

Later in the plan (Week 3), these same regions show up live in `/proc/<pid>/maps`, the kernel's
own listing of exactly what's mapped where in a running process.

---

## Hands-on

```bash
readelf -S tiny | grep -E '\.text|\.data|\.bss'
nm tiny | grep -E 'add|main'
```

`readelf -S` lists each section along with its address and size, so it's possible to see exactly
where `.text`, `.data`, and `.bss` sit. `nm` lists symbols (names mapped to addresses); notice
that function names like `add` and `main` resolve to addresses that fall inside `.text`.

Optionally, write a small program with a global, a `static`, a `malloc`'d buffer, and a local
variable, and for each one, work out which region it lives in.

---

## Exercise

The exercise questions live in `day3_questions.md`. Work through them there, then check answers
in `day3_answers.md`.

---

## Done when

- Each kind of data (a local, a `malloc`'d buffer, a function's code, a global) can be placed in
  its correct region without hesitation.
- "Stack grows down, heap grows up" is automatic.
- `.text`, `.data`, and `.bss` have actually been located with `readelf` on `tiny`.

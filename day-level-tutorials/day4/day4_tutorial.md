# Day 4: The Toolchain: the Instruments Used Every Day

## Why this matters

The next several months are spent living inside a handful of command-line tools. `objdump`,
`readelf`, and `gdb` are ground truth: when a homemade debugger disagrees with them, they're
right and the homemade one has a bug. Knowing the standard build flags is also what keeps
addresses and source-line mapping sane while learning.

---

## 1. The tools and what each one is for

| Tool                 | Job                                                      |
| -------------------- | -------------------------------------------------------- |
| `gcc`                | compiles C source into an ELF executable                 |
| `objdump -d`         | disassembles a binary into readable assembly             |
| `readelf`            | inspects ELF file structure and DWARF debug info         |
| `nm`                 | lists symbols (names mapped to addresses)                |
| `xxd` / `hexdump -C` | shows a file's raw bytes                                 |
| `addr2line`          | maps an address to a `file:line`                         |
| `strace`             | traces the syscalls a program makes                      |
| `gdb`                | the reference debugger: the oracle to check work against |

## 2. The standard build flags, and why each exists

Throughout this project, C programs get compiled with this line:

```bash
gcc -g -O0 -no-pie -fno-omit-frame-pointer tiny.c -o tiny
```

- **`-g`**: emit DWARF debug information (the data that maps machine code back to source lines
  and variable names). Without it, there's nothing for a debugger to work from.
- **`-O0`**: disable optimization. Optimized code reorders instructions, eliminates variables,
  and generally makes the mapping between source and machine code messy. `-O0` keeps that
  mapping clean and predictable while learning.
- **`-no-pie`**: build a fixed-address (non-position-independent) executable. Normally, modern
  binaries get loaded at a randomized address every run (ASLR); `-no-pie` turns that off, so an
  address seen in the disassembly today is the same address the program actually runs at.
- **`-fno-omit-frame-pointer`**: keep the `rbp` register dedicated to tracking stack frames.
  Without this, the compiler is free to reuse `rbp` as a general-purpose register, which breaks
  the simple stack-walking technique this project relies on for backtraces.

These four flags are the standing build line for the whole project. They only get dropped later,
deliberately, to see how optimized or PIE binaries behave differently.

## 3. What each tool actually shows

- **`objdump -d tiny`** prints the disassembled machine code. `objdump -d --source tiny`
  interleaves the original C source alongside it.
- **`readelf -S`** lists a binary's sections (`.text`, `.data`, and so on).
  **`readelf --debug-dump=info,line,frames`** dumps the DWARF debug data directly; this becomes
  essential once DWARF parsing starts, further into the project.
- **`nm tiny`** lists every symbol (function and global variable name) alongside its address.
- **`xxd tiny`** or **`hexdump -C tiny`** dump the raw bytes of the file, useful for confirming
  exactly what's on disk.
- **`addr2line -e tiny <addr>`** takes a raw address and reports which source file and line it
  corresponds to. This exact capability gets rebuilt from scratch later; for now, `addr2line`
  is there to check that rebuilt version against.
- **`strace ./tiny`** traces every syscall a program makes. Later on, `strace -e trace=ptrace
  ./nyxdb ./tiny` will show, syscall by syscall, everything the homemade debugger is doing to
  its target.
- **`gdb ./tiny`** is the reference debugger. Whenever it's unclear how a debugger *should*
  behave in some situation, that's the tool to go check.



---

# 

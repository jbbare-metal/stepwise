# Day 7: The Instructions That Must Be Readable on Sight

## Why this matters

Reading disassembly is how a decision gets made about *where* to set a breakpoint (say, right
after a function's prologue), how single-stepping gets implemented, and how a homemade debugger
gets debugged when it misbehaves.

---

## 1. Moving data: `mov` and `lea`

- **`mov src, dst`**: copy a value from `src` into `dst`. Four forms show up constantly in
  disassembly:

  - `mov %eax, %ebx` (register to register): copy `eax`'s value into `ebx`.
  - `mov $0x5, %eax` (immediate to register): put the literal number `5` into `eax`.
  - `mov -0x4(%rbp), %eax` (memory to register): read the 4-byte value stored at `rbp - 4`
    (typically a local variable) into `eax`.
  - `mov %eax, -0x4(%rbp)` (register to memory): write `eax`'s value into that same stack slot.

  Note `mov` can't go memory-to-memory directly; one side always has to be a register.

- **`lea src, dst`**: "load effective address": it computes an address (similar to what `&expr`
  does in C) *without* reading the memory at that address. `src` here isn't a value, it's a
  memory-operand *expression* like `-0x8(%rbp)`; `lea` computes what that expression evaluates to
  and hands you the address itself.

  Example: `lea -0x8(%rbp), %rax` sets `rax = rbp - 8`, the address of a local variable, without
  touching whatever's actually stored there. Contrast with `mov -0x8(%rbp), %rax`, which would
  instead read the 8-byte *value* sitting at that address into `rax`.

  It's often used for plain arithmetic too, since it can compute `base + offset` (or even
  `base + index*scale + offset`) without a separate `add`. A common compiler trick:
  `lea (%rax,%rax,2), %rbx` sets `rbx = rax + rax*2 = rax * 3`, multiplying by 3 using one `lea`
  instead of a slower `imul`.

## 2. Arithmetic and the stack: `add`/`sub`, `push`, `pop`

- **`add`/`sub` a, b**: ordinary arithmetic, applied in place: `b` is the destination. `add a, b`
  computes `b += a`; `sub a, b` computes `b -= a`. (Same source-first, destination-second order as
  `mov`, per section 6's AT&T syntax note.)
- **`push r`**: subtracts 8 from `rsp`, then stores `r`'s value at the new `[rsp]`. It's 8
  specifically because a general-purpose register is 8 bytes (64 bits) in long mode; each
  `push`/`pop` moves the stack pointer by exactly one register-sized slot.
- **`pop r`**: loads the value at `[rsp]` into `r`, then adds 8 back to `rsp`, undoing that same
  8-byte slot.

## 3. Calling and returning: `call` and `ret`

- **`call target`** does two things at once: it **pushes the return address** (the address of
  whatever instruction comes right after this `call`) onto the stack, and it **sets `rip` to
  `target`**, jumping execution there.
- **`ret`** does the reverse: it **pops the return address off the stack, into `rip`**, sending
  execution back to wherever the call came from.

## 4. Comparisons and branches: `cmp` and the jump instructions

- **`cmp a, b`** behaves like a `sub` internally, but throws away the numeric result, keeping
  only the flags it sets (in `eflags`).
- **`jmp`** jumps unconditionally.
- **`jz`/`je`** (jump if zero/equal): jumps when `ZF=1`: the last `cmp`/arithmetic result was
  zero (i.e., the two compared values were equal).
- **`jne`/`jnz`** (jump if not equal/not zero): jumps when `ZF=0`: the opposite of the above.
- **`jg`** (jump if greater, signed): jumps when `ZF=0` and `SF=OF`: after `cmp a, b`, this means
  `b` (the second/destination operand) was strictly greater than `a` (the first/source operand),
  as signed numbers. E.g. `cmp $10, %eax` / `jg label` jumps if `eax > 10`.
- **`jl`** (jump if less, signed): jumps when `SF≠OF`: after `cmp a, b`, this means `b` was
  strictly less than `a`, as signed numbers.

All of these read the flags `cmp` (or the preceding arithmetic instruction) just set; they don't
do any comparing themselves.

## 5. The instruction that makes breakpoints possible: `int3`

**`int3`** is the software-interrupt (trap) instruction, and its machine-code opcode is
**`0xCC`**, the exact byte from Day 1's lesson. This single instruction *is* how a breakpoint
works: later in the project, this byte gets written directly into a program's code.

## 6. Reading AT&T syntax

Disassembly from `objdump` uses **AT&T syntax**: the source operand comes first, the destination
second (the opposite order from Intel syntax). So `mov %eax, -0x4(%rbp)` means "store the value
in `eax` into memory at address `rbp − 4`." `$0x2` means the literal number 2, not a memory
reference.

## 7. The rest of the common set

Sections 1-6 cover the instructions that show up *constantly* and need to be automatic. These
show up often enough to be worth recognizing on sight too, even if they don't need the same level
of muscle memory yet.

**Bitwise logic**

- **`and`/`or`/`xor`**: ordinary bitwise AND/OR/XOR. `xor reg, reg` is a very common idiom for
  "zero this register"; it's shorter to encode than `mov reg, 0`.
- **`not`**: flips every bit (bitwise complement).
- **`test a, b`**: like `cmp`, but ANDs `a` and `b` instead of subtracting, and (like `cmp`) throws
  away the result and keeps only the flags. `test reg, reg` is the standard idiom for "is this
  register zero?" (it sets `ZF` based on `reg & reg`, which is zero exactly when `reg` is zero).

**Shifts**

- **`shl`/`sal` count, r**: shift `r` left by `count` bits (they're the same operation for this
  purpose). Each bit shifted left multiplies the value by 2, so `shl $2, %eax` multiplies `eax`
  by 4.
- **`shr` count, r**: logical shift right, filling the vacated high bits with zeros. Used for
  unsigned values, since each bit shifted right divides by 2.
- **`sar` count, r**: arithmetic shift right, filling the vacated high bits with copies of the sign
  bit instead of zeros, so a negative number stays negative. Used for signed values, for the same
  divide-by-2-per-bit reason as `shr`.

**Multiply and divide**

The CPU stores a multiply/divide result as raw bits, but the *correct* bits differ depending on
whether those bits represent a signed (can be negative) or unsigned (positive-only) number; that's
why each operation below comes in a signed and an unsigned version.

- **`imul`**: signed multiply. Its simplest, one-operand form (`imul r`) multiplies `r` against
  `rax`, and (since two 64-bit numbers can multiply out to as many as 128 bits, too wide for one
  register) stores the full-width result across **`rdx:rax`**: the high half in `rdx`, the low
  half in `rax`. (`imul` also has two- and three-operand forms that multiply into a single
  destination register and silently discard any overflow; that's the form a compiler usually
  emits for ordinary `int * int`, since it only needs the truncated result.)
- **`mul`**: the unsigned equivalent of `imul`'s one-operand form: same `r * rax -> rdx:rax`
  layout, just interpreting all the bits as unsigned.
- **`idiv`/`div`**: signed/unsigned divide. Both take the **`rdx:rax`** pair as the dividend (the
  same pair multiply just filled), and split the answer across two registers: the quotient goes
  in `rax`, the remainder in `rdx`.
- **`cdq`/`cqo`**: sign-extend `eax` into `edx:eax` (or `rax` into `rdx:rax`). Compilers emit this
  immediately before an `idiv`, specifically to set up that `rdx:rax` dividend correctly: dividing
  a single register's worth of value requires it to first be spread correctly across the pair
  `idiv` reads from.

**Increment, decrement, and extending width**

- **`inc`/`dec`**: add or subtract 1. Functionally the same as `add r, 1` / `sub r, 1`, just a
  shorter encoding.
- **`movzx`/`movsx`**: zero-extend / sign-extend a smaller value into a larger register (covered
  in Day 5's sub-register quirks): the instructions a plain `mov` can't do across sizes.

**More conditional jumps**

`je`/`jz`, `jne`/`jnz`, `jg`, and `jl` from section 4 are the ones seen most, but the full
conditional-jump family is bigger:

| Instruction | Jumps when (signed) | Instruction | Jumps when (unsigned) |
| ----------- | ------------------- | ----------- | --------------------- |
| `jg`        | greater than        | `ja`        | above                 |
| `jge`       | greater or equal    | `jae`       | above or equal        |
| `jl`        | less than           | `jb`        | below                 |
| `jle`       | less or equal       | `jbe`       | below or equal        |

Signed comparisons (`jg`/`jl`/...) and unsigned comparisons (`ja`/`jb`/...) read the *same* flags
differently; this is why signed vs. unsigned matters even though the bits being compared never
change.

A few single-flag jumps round this out: **`js`**/**`jns`** (sign flag set/not set), **`jo`**/
**`jno`** (overflow flag set/not set).

**Miscellaneous**

- **`nop`**: does nothing for one instruction's worth of time; used as padding, or as a
  placeholder where code used to be (or will be) patched in.
- **`leave`**: shorthand for a function's epilogue: equivalent to `mov rsp, rbp` followed by
  `pop rbp`. It undoes the standard prologue in one instruction, right before `ret`.
- **`syscall`**: the instruction that actually enters the kernel to make a Linux system call (what
  `strace` is watching for).
- **`xchg`**: swaps the values of two operands.
- **`endbr64`**: a landing-pad marker modern compilers place at the start of every valid call/jump
  target, as a CPU-enforced defense (CET) against jumping into the middle of a function. It's a
  no-op for normal execution; it only matters if something tries to jump somewhere it shouldn't.

# Day 1: Bits, Bytes, Hex, and Little-Endian

## Why this matters

A debugger's whole job is reading and writing another program's memory, one byte at a
time. Setting a breakpoint literally means overwriting one byte. If hex and little-endian
aren't second nature, every memory dump starts to look like noise. This is the alphabet
everything else gets written in: worth locking down before moving on.

---

## 1. The building blocks

| Unit       | Size               | Notes                                  |
| ---------- | ------------------ | -------------------------------------- |
| **bit**    | 1                  | a single `0` or `1`                    |
| **nibble** | 4 bits             | = exactly 1 hex digit                  |
| **byte**   | 8 bits = 2 nibbles | holds values `0`–`255` (`0x00`–`0xFF`) |

A byte has 2⁸ = **256** possible values. That's why a byte always maps to exactly **two hex
digits**: one per nibble.

## 2. Hex (base 16)

Hex digits go `0 1 2 3 4 5 6 7 8 9 a b c d e f`, where `a`=10 … `f`=15. Written with a `0x`
prefix, e.g. `0x2A`.

**Why hex and not decimal?** One hex digit is exactly one nibble, and two hex digits are
exactly one byte. That means a hex dump lines up perfectly with memory, byte for byte.
Decimal doesn't divide evenly like that, which makes it far less useful for looking at raw
memory.

Quick anchors worth memorizing:

- `0xF` = `1111` = 15 (the largest value one hex digit can hold)
- `0xFF` = `1111 1111` = 255 (the largest value one byte can hold)
- `0x10` = 16: not ten. Easy trap for anyone coming from decimal.

### Converting between bases

**Binary → Hex:** split the bits into groups of 4 (nibbles) from the right, then convert
each group.

```
1101 0110  →  D        6   →  0xD6
```

**Hex → Binary:** expand each hex digit back into 4 bits.

```
0x2A  →  0010 1010
```

**Hex → Decimal:** multiply each digit by its place value (16⁰, 16¹, …) and add.

```
0x2A = (2 × 16) + (10 × 1) = 32 + 10 = 42
```

**Decimal → Hex:** repeatedly divide by 16, collecting the remainders from bottom to top.

## 3. Endianness: the ordering gotcha

Once a value takes up more than one byte, such as a 4-byte integer, there needs to be a
rule for which byte sits at the lowest memory address. That rule is called **endianness**.

x86-64 is **little-endian**: the **l**east-significant byte sits at the **l**owest address.

Take the 4-byte value `0x0A0B0C0D`. Its least-significant byte is `0x0D`. On x86-64 it lands
in memory like this:

```
address:  +0    +1    +2    +3
byte:     0D    0C    0B    0A     ← least-significant byte first
```

Dumping those four bytes shows `0D 0C 0B 0A`: the reverse of how the number is normally
written. That reversal is the entire gotcha. Once it's expected, it stops being confusing.

## 4. Why this matters for a debugger

`ptrace`, the syscall a debugger uses to read a process's memory, hands memory back **8 bytes
at a time**, packaged as one little-endian integer often called a "word."

Planting a breakpoint means overwriting the **first byte** of an instruction with `0xCC`.
Because of little-endian ordering, "the first byte at a given address" turns out to be the
**least-significant byte** of the word read from it. In code, that's simply:

```c
word & 0xff
```

That single line is the payoff for today's material. Later, when that expression shows up in
breakpoint code, it should read as "grab the byte physically sitting at this address," not as
a magic incantation.



---

## 

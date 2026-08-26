# Day 5: The Sub-Registers

A register like rax is one physical 64 bit address but portions of that address are accessed using subregisters.

## Example Diagram of RAX and its subregisters

```
63                                              32 31          16 15    8 7     0   bit position
┌────────────────────────────────────────────────────────────────────────────┐
│                                    RAX                                       │  64-bit
└────────────────────────────────────────────────────────────────────────────┘
                                    ┌───────────────────────────────────────┐
                                    │                 EAX                     │  32-bit (bits 31-0)
                                    └───────────────────────────────────────┘
                                                        ┌───────────────────┐
                                                        │        AX          │  16-bit (bits 15-0)
                                                        └───────────────────┘
                                                        ┌─────────┬─────────┐
                                                        │   AH    │   AL    │  8-bit halves (bits 15-8 / 7-0)
                                                        │ (high)  │  (low)  │
                                                        └─────────┴─────────┘
```

Key points the labels capture:
- **AH** = the **high byte** of `AX` — bits 15-8.
- **AL** = the **low byte** of `AX` — bits 7-0.
- Only `rax`/`rbx`/`rcx`/`rdx` get an `AH`-style high-byte

## Subregister Table

The columns after the column heading of 64 are the subregisters. Hi and Lo represent the high and low bits respectively.

| 64  | 32  | 16 | Hi8 | Lo8 |
|-----|-----|----|-----|-----|
| rax | eax | ax | ah  | al  |
| rdx | edx | dx | dh  | dl  |
| rcx | ecx | cx | ch  | cl  |
| rdi | edi | di | —   | dil |
| rsi | esi | si | —   | sil |
| rbp | ebp | bp | —   | bpl |
| rsp | esp | sp | —   | spl |

## Some Quirks
1. Writing a 32-bit sub-register zero-extends to the full 64 bits but not for 16-bit and 8-bit writes.

mov eax, 0x1        ; rax becomes 0x0000000000000001 (upper 32 bits CLEARED)
mov ax,  0x1        ; only the low 16 bits change;bits 16-63 of rax are UNTOUCHED, whatever garbage was there stays
mov al,  0x1        ; only the low 8 bits change;bits 8-63 UNTOUCHED
Only the 32-bit write gets special "clean slate" treatment.

2. You can't zero/sign-extend with a plain mov between different sizes unless you use movzx/movsx.
mov eax, al is not legal encoding;mov's two operands must be the same width. To take an 8-bit value and widen it into a 32/64-bit register with defined behavior, the ISA gives you dedicated instructions:

movzx %al, %eax      ; zero-extend al into eax
movsx %al, %eax      ; sign-extend al into eax

The plain mov sub-register rule (quirk #1) only covers the eax-style case, so anything narrower needs an explicit extend instruction.

3. AH/BH/CH/DH (high-byte access) only exist for rax/rbx/rcx/rdx — no other register has one. This naming is a leftover from the 8086 era where only four registers were byte-addressable.

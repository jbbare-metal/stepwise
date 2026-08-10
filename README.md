# stepwise

A Linux debugger I'm building from scratch to learn how debuggers work under the hood.

> **Status:** 🚧 Learning project / work in progress. I'm new to debuggers and building this step by step to understand how they actually work.

## About

This project is my hands-on way of learning what a debugger really does — the same core mechanics that tools like GDB and LLDB are built on. Rather than just using a debugger, I wanted to build a minimal one myself to understand the low-level pieces: how a program can pause, inspect, and control another running program.

It's written in **C** and targets **Linux (x86-64)**.

## What I'm learning

- **`ptrace`** — the Linux system call that lets one process observe and control another. It's the foundation everything else is built on.
- **Launching a process under the debugger** — starting a target program in a paused, controllable state.
- **Breakpoints** — pausing execution at a specific line or address so you can inspect what's happening.
- **Reading registers and memory** — looking at the CPU's state and the program's memory while it's stopped.
- **Single-stepping** — running the program one instruction (or line) at a time.

## Current status

- [ ] [Can launch a target process and attach to it]
- [ ] [Set a breakpoint at an address]
- [ ] [Continue / step execution]
- [ ] [Read register values]
- [ ] [Read/write memory]

*(I'll update this list as I implement each piece.)*

## Building and running

​```bash
gcc -o mydebugger main.c
./mydebugger ./path/to/target-program
​```

## How it works

Pending...

## Resources I'm learning from

- Eli Bendersky — *Writing a Linux Debugger* article series
- Sy Brand (TartanLlama) — *Writing a Linux Debugger* tutorial series

## Notes

This is a personal learning project, not a production tool — I'm building it to understand the concepts. Feedback and suggestions are welcome!

## License

[MIT — see LICENSE file]

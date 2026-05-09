# TextJedi Language Implementation

A complete lexical analyzer and interpreter for the TextJedi programming language. Developed in C as a collaborative project.

**Co-authors:** Mahdi Saghai Bolghari & Gökhan Babayiğit

## What is TextJedi?
TextJedi is a programming language specifically designed for editing text files, primarily through cutting and pasting operations. The language supports Strings (called `text`) and Integers, and handles basic arithmetic alongside specialized string manipulations.

## Project Structure & Final Output
This repository showcases the step-by-step development of the TextJedi language:

1.  **Phase 1: Lexical Analyzer (`la.c`)** – The initial standalone module designed to tokenize TextJedi source code into `.lx` files.
2.  **Phase 2: Full Interpreter (`TextJedi.c`)** – **The Final Output.** This is the complete implementation that integrates lexical analysis with an execution engine to interpret and run TextJedi programs directly.

## Features (Final Version)
* **Symbol Table & Type Checking:** Manages variables (`text` and `int`) and enforces strict type checking.
* **Arithmetic & String Operations:** Supports integer addition/subtraction and unique string manipulations like concatenation and substring removal.
* **Built-in Functions:** Full support for type-casting and length checking via `size()`, `asString()`, and `asText()`.
* **I/O Commands:** Handles terminal interactions (`input prompt` and `output`).

## How to Run

### Running the Final Implementation (Phase 2)
To execute a TextJedi script using the full interpreter:

```bash
gcc -o TextJedi TextJedi.c
./TextJedi laTest

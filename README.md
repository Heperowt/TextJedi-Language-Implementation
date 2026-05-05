# TextJedi Language Implementation

A complete lexical analyzer (and upcoming interpreter) for the TextJedi programming language. Developed in C as a collaborative project.

**Co-authors:** Mahdi Saghai Bolghari & Gökhan Babayiğit

## What is TextJedi?
TextJedi is a programming language specifically designed for editing text files, primarily through cutting and pasting operations. The language supports Strings (called `text`) and Integers, and handles basic arithmetic alongside specialized string manipulations.

## Phase 1: Lexical Analyzer
The first phase of this project is a standalone Lexical Analyzer. It reads a `.tj` script file and performs lexical analysis to tokenize the source code. The output is generated in a `.lx` file containing a suitable representation of a token on each line.

### Features
* **Keywords:** Recognizes all language-specific keywords (`new`, `text`, `int`, `output`, `insert`, etc.).
* **Identifiers:** Captures valid variables starting with an alphabetic character (max 30 characters).
* **Constants:** Handles zero/positive integer constants and dynamically allocated string constants of unlimited size.
* **Error Handling:** Detects and reports lexical errors such as open strings/comments (`A big lexeme just left open`) and identifier size limits (`Identifier size exceeded`).

### How to Run
Compile the C source code and run the executable by passing the script name (without the `.tj` extension):

```bash
gcc -o la la.c
./la laTest
```

*Note: This will read `laTest.tj` and produce `laTest.lx` in the same directory.*

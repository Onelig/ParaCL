[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17) [![Language](https://img.shields.io/badge/lang-English🇬🇧-white)](README.md) [![Язык](https://img.shields.io/badge/язык-Русский🇷🇺-white)](README-RU.md) [![GitHub](https://img.shields.io/badge/github-gray)](https://github.com/Onelig/ParaCL) [![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
# ParaCL — Parasyl Language Interpreter

ParaCL is a simple interpreter for the Parasyl programming language, whose syntax resembles C.  
The interpreter reads a program from **standard input** and outputs the results to **standard output**.

---

## 📜 Parasyl Language Overview

- All variables are of type `int`.
- A program consists of a sequence of statements.
- Supported control structures:
  - `if` and `else`
  - `while`
- Assignment and arithmetic expressions.
- Postfix increment and decrement operators: `x++`, `x--`.
- Input from keyboard: `x = ?;`.
- Output to screen: `print x;`.
- Comments:
  - Single-line: `// comment`
  - Multi-line: `/* comment */`
- Operator precedence (from highest to lowest):

  1. Postfix increment/decrement: `++`, `--`
  2. Unary minus: `-`
  3. Multiplication and division: `*`, `/`
  4. Addition and subtraction: `+`, `-`
  5. Comparisons: `>`, `<`, `>=`, `<=`, `==`, `!=`

---

## Syntax of Main Constructs

```pascal
if (expression) {
  // if body
} else
  // else body (optional)
```

```pascal
while (expression) {
  // loop body
}
```

```pascal
x = 1;
x = ?;    // input from keyboard
x++;
x--;
print x;
```

---

## Variable Scope

Variables are valid within the block delimited by `{ ... }`.
Accessing a variable outside its scope causes an error.

```pascal
{
  y = 1;
}
print y;  // Error: variable y is not defined
```

---

## Error Handling

Using undeclared variables or accessing variables out of scope results in a detailed error message indicating the line and position.

---

## Example Program
*To finish input in the terminal, press Ctrl + C*

```pascal
fst = 0;
snd = 1;
iters = ?;

while (iters-- > 0)
{
  tmp = fst;
  snd += tmp;
}

print snd;

y = 1;
x = ?;
while (((++x))++ > y)
{
  print /* comment */ (y == x);
  x -= 100;
}
```

---

## ⚙️ Build and Run
```console
mkdir build && cd build
cmake ..
cmake --build .
ctest
```

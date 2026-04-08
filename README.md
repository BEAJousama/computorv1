# computorv1

A polynomial equation solver written in C++17. Given a polynomial equation of degree ≤ 2, it reduces it to canonical form and computes all solutions, including complex ones.

---

## Features

- Parses and reduces polynomial equations of degree 0, 1, or 2
- Handles real and complex roots for quadratic equations
- Displays the discriminant case (positive / zero / negative)
- Outputs exact fractions when the result is rational
- Accepts input via command-line argument or interactive stdin
- Custom math helpers (no `<cmath>` dependency for sqrt/abs)

---

## Requirements

- C++17 compatible compiler (e.g. `g++`, `clang++`)
- `make`

---

## Build

```bash
make
```

This produces the `computor` binary. Use `make re` to do a clean rebuild.

```bash
make clean   # remove object files
make fclean  # remove object files and binary
make re      # fclean + build
```

---

## Usage

Pass the equation as a command-line argument:

```bash
./computor "5 * X^2 - 4 * X + 1 = 0"
```

Or run without arguments to enter it interactively:

```bash
./computor
Enter the equation:
5 * X^2 - 4 * X + 1 = 0
```

---

## Examples

### Degree 2 — two real solutions (Δ > 0)

```
$ ./computor "5 * X^2 - 6 * X + 1 = 0"
Reduced form: 1 - 6 * X^1 + 5 * X^2 = 0
Polynomial degree: 2
Discriminant is strictly positive, the two solutions are:
1
1/5
```

### Degree 2 — one real solution (Δ = 0)

```
$ ./computor "X^2 - 2 * X + 1 = 0"
Reduced form: 1 - 2 * X^1 + X^2 = 0
Polynomial degree: 2
Discriminant is zero, the solution is:
1
```

### Degree 2 — two complex solutions (Δ < 0)

```
$ ./computor "X^2 + X + 1 = 0"
Reduced form: 1 + X^1 + X^2 = 0
Polynomial degree: 2
Discriminant is strictly negative, the two complex solutions are:
-1/2 + (√3)/2 i
-1/2 - (√3)/2 i
```

### Degree 1

```
$ ./computor "3 * X + 6 = 0"
Reduced form: 6 + 3 * X^1 = 0
Polynomial degree: 1
The solution is:
-2
```

### Degree 0 — no solution / infinite solutions

```
$ ./computor "3 = 0"
Reduced form: 3 * X^0 = 0
Polynomial degree: 0
No solution.

$ ./computor "0 = 0"
Reduced form: 0 * X^0 = 0
Any real number is a solution.
```

---

## Architecture

```
Input string
    │
    ▼
Lexer          — tokenises the raw string into numbers, operators, 'X', '^', '='
    │
    ▼
Parser         — builds a coefficient map {degree → value} for each side, then
                 subtracts RHS from LHS to produce the reduced form
    │
    ▼
ParserAPI      — wraps the result in a ParseResult struct and formats the
                 reduced form string
    │
    ▼
Computor       — dispatches to the appropriate solver based on polynomial degree
                 and prints the result
```

---

## Documentation

See [DOCUMENTATION.md](DOCUMENTATION.md) for a line-by-line walkthrough of the entire codebase.

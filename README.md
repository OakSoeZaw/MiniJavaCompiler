# MiniJava Compiler (Bison/Flex)

A compiler for a MiniJava dialect, built as a course project in phases.

| Phase | Deliverable | Status |
|---|---|---|
| 1 | Parser with extensions and syntax error recovery | Complete (due Oct 15) |
| 2 | Symbol resolution and type checking, with unit tests | Planned (due Nov 10) |
| 3 | Code generation; run all sample programs | Planned |
| 4 | (Optional) Optimization pass | Planned |

This README covers Phase 1.

## Requirements

- GCC
- Flex
- **Bison 3.8 or newer.** macOS ships Bison 2.3, which is too old (`%empty` and `%define parse.error verbose` need a newer version). Install with `brew install bison` and make sure it is first on your `PATH`.

## Build and run

```bash
make                                   # builds build/minijava
./build/minijava < tests/base/BinarySearch.java
make test                              # runs every test and writes logs/summary.txt
make clean
```

The parser reads a program from stdin. Each rule prints a trace line when it is reduced, so inner constructs appear before the constructs that contain them (bottom-up order). The final line is the result:

| Final line | Meaning |
|---|---|
| `Parse successful` | No syntax errors |
| `Parse completed with N syntax error(s) (recovered)` | Errors were reported and the parser resynchronized |
| `Parse failed` | Unrecoverable error |

## Project layout

```
miniJava/
├── Makefile
├── run_tests.sh        (optional; same loop as `make test`)
├── src/
│   ├── lexer.l         Flex scanner
│   └── parser.y        Bison grammar
├── build/              generated files (gitignored)
├── tests/
│   ├── base/           official MiniJava samples
│   ├── extensions/     programs using the two extensions
│   └── errors/         programs with deliberate syntax errors
├── logs/summary.txt    one result line per test
└── evidence/           selected full outputs (see below)
```

## Grammar overview

The grammar follows the MiniJava BNF: `Goal`, `MainClass`, `ClassDeclaration` (with `extends`), `VarDeclaration`, `MethodDeclaration`, `Type`, `Statement`, `Expression`.

**Operator precedence** (lowest to highest), declared with Bison `%left`/`%right`:

| Level | Operators | Associativity |
|---|---|---|
| 1 | `\|\|` | left |
| 2 | `&&` | left |
| 3 | `==` `!=` | left |
| 4 | `<` `<=` `>` `>=` | left |
| 5 | `+` `-` | left |
| 6 | `*` `/` `%` | left |
| 7 | `!` | right |
| 8 | `[ ]` `.` | left |

**Conflict-free grammar.** Bison reports 0 conflicts (`build/parser.output` confirms this).

Design note: an earlier version had one shift/reduce conflict. A method body was written as `VarDeclarationList StatementList RETURN ...`, and both lists could be empty. After the last local declaration, the parser had to reduce the empty `StatementList` before it could see a token telling it what came next. With `IDENT` as the lookahead it chose to shift, treating the identifier as the start of another declaration, so `x = 1;` after a declaration failed to parse. The fix is `MethodBody`, which lists the four combinations (declarations present or absent, statements present or absent) using non-empty list rules. No empty rule has to be reduced there, and one token of lookahead is enough to decide.

## Extensions

**Extension 1: additional integer arithmetic and relational operators**
Adds `/`, `%`, `<=`, `>=`, `>`, and `||` to the base `&&`, `<`, `+`, `-`, `*`.

**Extension 2: `null`, `==`, `!=`**
Adds `null` as a constant expression and `==` / `!=` as operators.

The parser checks syntax only. Expressions such as `null == 3` or `true < false` parse successfully. Rejecting them is the job of the type checker in Phase 2.

Test programs (`tests/extensions/`):

| File | Extension | What it exercises |
|---|---|---|
| `ops_arith.java` | 1 | `/`, `%` mixed with `+ - *` and parentheses |
| `ops_relational.java` | 1 | `<= >= > == != \|\| && !` combined |
| `[ext1_ops_precedence.java]` | 1 | `[long mixed-operator expression]` |
| `null_refs.java` | 2 | `null` assignment, `==`/`!=` on object fields |
| `[ext2_null_args.java]` | 2 | `[null passed as an argument]` |
| `[ext2_null_new.java]` | 2 | `[== between new expressions]` |

## Evidence of successful parsing

`make test` runs every program in `tests/` and writes one line per test to `logs/summary.txt`.

**Base samples** (`tests/base/`): all official MiniJava samples.

| Program | Result |
|---|---|
| BinarySearch | Parse successful |
| `[BinaryTree]` | `[...]` |
| `[BubbleSort]` | `[...]` |
| `[Factorial]` | `[...]` |
| `[LinearSearch]` | `[...]` |
| `[LinkedList]` | `[...]` |
| `[QuickSort]` | `[...]` |
| `[TreeVisitor]` | `[...]` |

Full output for selected tests is in `evidence/`: `[BinarySearch]`, one file per extension, and all four error tests.

## Syntax error handling and recovery

The parser uses panic-mode recovery with `error` productions. After an error is reported, Bison pops the stack to the innermost state that can shift `error`, then discards input until a synchronizing token appears.

| Context | Production | Synchronizing token(s) | Effect |
|---|---|---|---|
| Statement | `Statement : error SEMI` | `;` | Bad statement is dropped; parsing resumes at the next statement |
| `main` body | `... LCURLY error RCURLY RCURLY` | `;` or `}` | Bad `main` is dropped; parsing resumes at the next class |
| Method | `MethodDeclaration : PUBLIC error RCURLY` | `}` | Bad method is dropped; resumes at the next method |
| Class | `ClassDeclaration : CLASS error RCURLY` | `}` | Bad class is dropped; resumes at the next class |

`yyerrok` is called after each recovery so a second nearby error is still reported. Without it, Bison suppresses messages for the next three shifted tokens. Messages use `%define parse.error verbose`, so they include the line number, the unexpected token, and what was expected. For example: `Syntax error at line 4: syntax error, unexpected RCURLY, expecting SEMI`.

Because `yyparse()` returns 0 after a successful recovery, `main` counts errors itself so a file with errors never prints `Parse successful`.

**Error tests** (`tests/errors/`):

| File | Planted error | Observed behavior |
|---|---|---|
| `err1_missing_semi.java` | Missing `;` in `main` and in a method | 2 errors reported; class `A` still parsed. The statement after the second missing `;` is dropped. |
| `err2_bad_expression.java` | `1 + * 2` | 1 error; the rest of the method and class parsed |
| `err3_bad_method_header.java` | `f(int x,)` | 1 error; the following method `g` parsed |
| `err4_multiple.java` | Bad expressions and `class C extends {` | 3 errors; class `D` still parsed |

### Debugging note: the first `err1` fix failed

My first attempt added `MainClass : CLASS error RCURLY RCURLY`. It had no effect, because Bison recovers at the *innermost* state that can shift `error`. That state was inside `main`, where `Statement : error SEMI` applies. The `;` search ran past `main`'s closing braces and swallowed class `A`. The fix was to add `}` as a second synchronizing token inside `main`, at the level where the error occurs.

## Known limitations

- **Recovery is coarse.** A syntax error inside a method header skips to the first `}`, which may belong to an inner block, so one mistake can cause a second spurious message.
- **A missing `;` loses the next statement.** The error is only detected at the following token, and `error SEMI` then discards the next statement through its `;`.
- **"Recovered" means resynchronized, not valid.** A method or class that contained an error can still print its `Parsed ...` line, because only the bad statement was dropped.
- **Keywords are reserved everywhere.** The scanner matches keywords before identifiers, so `main`, `length`, `out`, `println`, `String`, and `System` cannot be used as identifier names.
- **Comments:** only `//` line comments are supported.
- **Syntax only.** No symbol resolution or type checking until Phase 2.

## Next phases

- **Phase 2:** build symbol tables and a type checker, with unit tests for expression and invocation errors, duplicate declarations, overload/override errors, and illegal inheritance.
- **Phase 3:** generate and run code for all sample programs.
- **Phase 4 (optional):** add an optimization pass and re-run the samples.

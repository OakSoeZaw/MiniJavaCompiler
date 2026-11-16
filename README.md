# MiniJava Compiler (Bison/Flex)

A compiler for a MiniJava dialect, built as a course project in phases.

| Phase | Deliverable | Status |
|---|---|---|
| 1 | Parser with extensions and syntax error recovery | Complete (due Oct 15) |
| 2 | Symbol resolution and type checking, with unit tests | Complete (due Nov 10) |
| 3 | Code generation; run all sample programs | Planned |
| 4 | (Optional) Optimization pass | Planned |

This README covers Phases 1 and 2.

## Requirements

- GCC
- Flex
- **Bison 3.8 or newer.** macOS ships Bison 2.3, which is too old (`%empty` and `%define parse.error verbose` need a newer version). Install with `brew install bison` and make sure it is first on your `PATH`.

## Build and run

```bash
make                                   # builds build/minijava
./build/minijava < tests/base/BinarySearch.java
make test                              # parses base, extensions, errors; writes logs/summary.txt
make test-all                          # runs every suite and checks expected results
make clean
```

The program reads a MiniJava program from stdin. The parser builds an AST. If there are no syntax errors, the semantic checker runs on it. The last line of output is the result:

| Final line | Meaning |
|---|---|
| `Parse successful` | No syntax errors and no semantic errors |
| `Parse completed with N syntax error (recovered)` | Syntax errors were reported and the parser resynchronized; the semantic check is skipped |
| `Semantic check completed with N errors` | The program parsed but has N semantic errors (details on stderr) |
| `Parse failed (unrecoverable)` | The parser could not recover |

## Project layout

```
miniJava/
├── Makefile
├── RULES.md            semantic rules (Phase 2)
├── run_all_tests.sh    runs every suite and checks expected results
├── make_tests.sh       regenerates tests/semantic/*.java
├── src/
│   ├── lexer.l         Flex scanner
│   ├── parser.y        Bison grammar; builds the AST
│   ├── ast.h / ast.c   AST nodes and constructors
│   └── semantic.h / semantic.c   class table, inheritance, member and type checks
├── build/              generated files (gitignored)
├── tests/
│   ├── base/           official MiniJava samples (+ Bad.java, a syntax-error case)
│   ├── extensions/     programs using the extensions
│   ├── errors/         programs with deliberate syntax errors
│   ├── semantic/       one program per semantic error, plus a valid control
│   └── results/        saved output: base.txt, extensions.txt, semantic.txt
└── logs/summary.txt    one result line per `make test` run
```

## Grammar overview

The grammar follows the MiniJava BNF: `Goal`, `MainClass`, `ClassDeclaration` (with `extends`), `VarDeclaration`, `MethodDeclaration`, `Type`, `Statement`, `Expression`. Each rule's action builds an AST node instead of printing a trace line.

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

**Conflict-free grammar.** Bison should report 0 conflicts. Check with `bison -v` and `build/parser.output`.

Design note: a method body starts with local declarations and then statements, and both a declaration (`Foo x;`) and a statement (`x = 1;`) can begin with an identifier. Writing the body as `VarDeclList StatementList RETURN ...` with both lists possibly empty forces the parser to reduce an empty list before it can see what follows, which causes a shift/reduce conflict. The fix has two parts:

1. `MethodBody` lists the four combinations (declarations present or absent, statements present or absent) using non-empty list rules, so no empty rule has to be reduced there.
2. `VarDeclList1` is **left-recursive**. After `int x;`, an `IDENT` could start another declaration or a statement. With left recursion the parser shifts that identifier in both cases and decides from the next token: `=` or `[` means a statement, another `IDENT` means a declaration. A right-recursive version caused a conflict, and `x = 1;` after a declaration failed with `unexpected EQUAL, expecting IDENT`.

Lists that are not followed by a token that can start the list's elements (fields, methods, classes, statements before `return`) stay right-recursive, so they come out in source order with no reversal.

**AST naming.** AST types must not share names with Bison tokens, because tokens become C enum constants in `parser.tab.h`. The class node is therefore `CLASS_DECL`, not `CLASS`.

## Extensions

**Extension 1: additional integer arithmetic and relational operators**
Adds `/`, `%`, `<=`, `>=`, `>`, and `||` to the base `&&`, `<`, `+`, `-`, `*`.

**Extension 2: `null`, `==`, `!=`**
Adds `null` as a constant expression and `==` / `!=` as operators.

The parser checks syntax only, so `null == 3` or `true < false` parse successfully. The Phase 2 type checker rejects them (see `RULES.md`).

Test programs (`tests/extensions/`):

| File | Extension | What it exercises |
|---|---|---|
| `ops_arith.java` | 1 | `/`, `%` mixed with `+ - *` and parentheses |
| `ops_relational.java` | 1 | `<= >= > == != \|\| && !` combined |
| `null_refs.java` | 2 | `null` assignment, `==`/`!=` on object fields |

`ops_relational` and `null_refs` print boolean values, which is why `System.out.println` accepts `int` or `boolean` in this dialect.

## Evidence of successful parsing

`make test` runs every program in `tests/` and writes one line per test to `logs/summary.txt`. `make test-all` also checks each result against its expected last line and saves full output to `tests/results/`.

**Base samples** (`tests/base/`):

| Program | Result |
|---|---|
| BinarySearch | Parse successful |
| BinaryTree | Parse successful |
| BubbleSort | Parse successful |
| Factorial | Parse successful |
| LinearSearch | Parse successful |
| LinkedList | Parse successful |
| QuickSort | Parse successful |
| TreeVisitor | Parse successful |

`Bad.java` in the same folder is a deliberate syntax error and is expected to report a recovered syntax error.

Full output for every suite is in `tests/results/base.txt`, `tests/results/extensions.txt` and `tests/results/semantic.txt`.

## Syntax error handling and recovery

The parser uses panic-mode recovery with `error` productions. After an error is reported, Bison pops the stack to the innermost state that can shift `error`, then discards input until a synchronizing token appears.

| Context | Production | Synchronizing token(s) | Effect |
|---|---|---|---|
| Statement | `Statement : error SEMI` | `;` | Bad statement is dropped; parsing resumes at the next statement |
| `main` body | `... LCURLY error RCURLY RCURLY` | `;` or `}` | Bad `main` is dropped; parsing resumes at the next class |
| Method | `MethodDeclaration : PUBLIC error RCURLY` | `}` | Bad method is dropped; resumes at the next method |
| Class | `ClassDeclaration : CLASS error RCURLY` | `}` | Bad class is dropped; resumes at the next class |

`yyerrok` is called after each recovery so a second nearby error is still reported. Without it, Bison suppresses messages for the next three shifted tokens. Messages use `%define parse.error verbose`, so they include the line number, the unexpected token, and what was expected. For example: `Syntax error at line 4: syntax error, unexpected RCURLY, expecting SEMI`.

Because `yyparse()` returns 0 after a successful recovery, `main` counts errors itself so a file with errors never prints `Parse successful`. Each error production sets its value to `NULL`, and list rules drop `NULL` statements and methods, so the AST never contains holes. Because a recovered parse can still lose declarations, the semantic check is skipped whenever there were syntax errors.

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
- **"Recovered" means resynchronized, not valid.** A method or class that contained an error can still be kept with the bad statement dropped, so no semantic check is run on a file with any syntax error.
- **Keywords are reserved everywhere.** The scanner matches keywords before identifiers, so `main`, `length`, `out`, `println`, `String`, and `System` cannot be used as identifier names.
- **Comments:** only `//` line comments are supported.

## Phase 2: Symbol resolution and type checking

**Status:** complete. AST (`ast.h/.c`), semantic passes
(`semantic.h/.c`), rules in `RULES.md`.

**Build:** `make`   **Run:** `build/minijava < file.java`
**Tests:** `make test` (parse), `make test-all` (base, extensions,
semantic, syntax-error cases).

**Results:** `tests/results/base.txt`, `extensions.txt`, `semantic.txt`.

| Category | Tests |
|---|---|
| Expression errors | 08, 09, 13, 14, 15, 17 |
| Invocation errors | 10, 11 |
| Duplicate declarations | 01, 05, 06, 18, 19, 20 |
| Override / overload | 07 |
| Illegal inheritance | 02, 03, 04 |
| Return type | 12 |
| Valid control | 16 |

## Next phases

- **Phase 3:** generate and run code for all sample programs.
- **Phase 4 (optional):** add an optimization pass and re-run the samples.
## Known Grammar Limitations

### LALR(1) Shift/Reduce Conflict: Variable Declarations vs. Assignment Statements

* **Issue:** Bison reports 1 shift/reduce conflict between `VarDeclaration` and `Statement`.
* **Root Cause:** Both local variable declarations (`Type Identifier ;`) and assignment statements (`Identifier = Expression ;`) begin with an identifier token when `Type` is a custom class type (e.g., `MyClass x;` vs `x = 5;`). Standard LALR(1) parsing uses a single-token lookahead and cannot distinguish between a class-type identifier and a variable-name identifier at lookahead token #1.
* **Resolution Strategy:** 
  * We rely on Bison's default shift resolution behavior, which correctly shifts the lookahead token and parses valid MiniJava programs as intended.
  * *Alternative fixes considered:* Refactoring the grammar into a combined `BlockItem` production to delay resolution to lookahead token #2, or introducing a `%glr-parser` directive. We retained the original BNF grammar structure to align strictly with the standard MiniJava specification.

#miniJava Semantic Rules

## 1. Error reporting
- Every semantic error is printed to stderr as
    'Semantic error at line N: <message>'.
-Checking runs only after a syntax-error-free parese, so the AST is complete.
-One mistake produces one message. An expression with an error gets the special type '<error>', which is compatible with every other type, so it causes no follow-up errors.
the final status line is 'Semantic check completed with N erros', or 'Parse successful' when there are none.

##2. Passes
1. **Class table:** register the main class and all classes; report duplicates.
2. **Inheritance:** report undefined parents, 'extends' of itself, 'extends' of the main class, and inheritance cycles (a cycle is broken after being reported so later passes terminate).
3.**Members:** report duplicate fields, methods, parameters and locals; check that every class type use exists; check overrides.
**Bodies:** type-check every statement and expression.

## 3. Decisions

### Overloading and overriding
-There is no **no overloading**. A method with a same name as a method in an ancestor is an override and must have the identical return type and identical parameter types (same cound, same types in order).
-Two methods with the same name in one class are a duplicate-method error, even if their parameters differ.
-A local variable may not have the same name as a parameter of its method.
-Lookup order for a name: locals, then parameters, then fields of current class, then fields of its ancestors.

### Subtyping and assignability
- 'int', 'boolean' and 'int[]' are subtypes only of themselves.
- A class type 'B' is a subtype of 'A' if 'B' is 'A' or inherits from 'A' through 'extends'.
- 'x = e' and argument passing require 'type(e)' to be a subtype of the target. The returned expression must be a subtype of the declared return type.

### null
- 'null' has its own type. It is assignable to any class type and to 'int[]'.
- It is not assignable to 'int' or 'boolean'.

### == and !=
- Allowed if one operand's type is a subtype of the other's. This permits 'int == int', 'boolean == boolean', class references of related types, and any reference compared with 'null'.
- Rejected: mixing a primitive with a reference or with another primitive (e.g. '1 == true', 'x == null' for an 'int x').
- The result is always 'boolean'.

### Operators
- '+ - * / %' take two 'int's and give 'int'.
- '< <= > >=' take two 'int's and give 'boolean'.
- '&& ||' take two 'boolean's and give 'boolean'; '!' takes a 'boolean'.
- 'a[i]' needs 'int[]' and 'int'; 'a.length' nees 'int[]'; 'new int[e]' needs 'int e'

### println
- 'Sytem.out.println' accepts only 'int'.

### Statements
- 'if' and 'while' conditions must be 'boolean'.
- 'a[i] = e' needs 'a' of type 'int[]' and 'i', 'e' of type 'int'. 

### Method calls
- The receiver must have a clas type. The method is searched in that class and its ancestors. The argument count must match, and each argument must be a subtype of the corresponding parameter type. The result is the method's declared return type. 

### The main class
- 'extends Main' is an error.
- 'new Main()' is an error, and 'Main' cannot be used as a variable type. 
- 'this' insdie 'main' is an error (there is no current object).
- 'main' can only contain the single statement allowed by the grammar, and it has no local variables or fields.

## 4. Tests
'tests/semantic/' has one file per error kind (01-15) and one valid control program (16). Output of all runs is in 'tests/semantic/results.txt'.
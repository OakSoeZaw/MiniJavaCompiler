#!/bin/sh
mkdir -p tests/semantic
cd tests/semantic

MAIN='class Main { public static void main(String[] args) { System.out.println(0); } }'

# 01 duplicate class
cat > 01_dup_class.java <<EOF
$MAIN
class A { }
class A { }
EOF

# 02 undefined parent
cat > 02_undefined_parent.java <<EOF
$MAIN
class A extends Z { }
EOF

# 03 extends self
cat > 03_extends_self.java <<EOF
$MAIN
class A extends A { }
EOF

# 04 inheritance cycle
cat > 04_cycle.java <<EOF
$MAIN
class A extends B { }
class B extends A { }
EOF

# 05 duplicate field
cat > 05_dup_field.java <<EOF
$MAIN
class A {
    int x;
    boolean x;
}
EOF

# 06 duplicate method
cat > 06_dup_method.java <<EOF
$MAIN
class A {
    public int f() { return 1; }
    public int f() { return 2; }
}
EOF

# 07 bad override (return type differs)
cat > 07_bad_override.java <<EOF
$MAIN
class A { public int f() { return 1; } }
class B extends A { public boolean f() { return true; } }
EOF

# 08 undefined variable
cat > 08_undefined_var.java <<'EOF'
class Main {
    public static void main(String[] args) {
        x = 1;
    }
}
EOF

# 09 int/boolean mismatch
cat > 09_type_mismatch.java <<EOF
$MAIN
class A {
    public int f() {
        int x;
        x = true;
        return x;
    }
}
EOF

# 10 bad arity
cat > 10_bad_arity.java <<EOF
$MAIN
class A {
    public int f(int a) { return a; }
    public int g() { return this.f(1, 2); }
}
EOF

# 11 bad argument type
cat > 11_bad_arg_type.java <<EOF
$MAIN
class A {
    public int f(int a) { return a; }
    public int g() { return this.f(true); }
}
EOF

# 12 wrong return type
cat > 12_wrong_return.java <<EOF
$MAIN
class A { public int f() { return true; } }
EOF

# 13 non-boolean condition
cat > 13_non_bool_cond.java <<'EOF'
class Main {
    public static void main(String[] args) {
        if (1) System.out.println(1); else System.out.println(2);
    }
}
EOF

# 14 null misuse
cat > 14_null_misuse.java <<EOF
$MAIN
class A {
    public int f() {
        int x;
        x = null;
        return x;
    }
}
EOF

# 15 this in main
cat > 15_this_in_main.java <<'EOF'
class Main {
    public static void main(String[] args) {
        System.out.println(this.foo());
    }
}
EOF

## test for println
cat > 17_println_bad_type.java <<'EOF'
class Main {
    public static void main(String[] args) {
        System.out.println(new int[3]);
    }
}
EOF

# 16 valid control program
cat > 16_valid.java <<'EOF'
class Main {
    public static void main(String[] args) {
        System.out.println(new Zoo().run());
    }
}
class Animal {
    int legs;
    public int getLegs() { return legs; }
    public int setLegs(int n) { legs = n; return legs; }
}
class Dog extends Animal {
    public int bark() {
        int x;
        x = this.getLegs();
        return x;
    }
}
class Zoo {
    public int count(Animal a) { return a.getLegs(); }
    public int run() {
        Dog d;
        int r;
        d = new Dog();
        r = d.setLegs(4);
        r = this.count(d);
        return r;
    }
}
EOF

M='class Main { public static void main(String[] args) { System.out.println(0); } }'
cat > 18_dup_param.java <<EOF
$M
class A { public int f(int a, int a) { return a; } }
EOF
cat > 19_dup_local.java <<EOF
$M
class A { public int f() { int a; int a; return 1; } }
EOF
cat > 20_local_redeclares_param.java <<EOF
$M
class A { public int f(int a) { int a; return a; } }
EOF
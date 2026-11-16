class Main { public static void main(String[] args) { System.out.println(0); } }
class A {
    public int f(int a) { return a; }
    public int g() { return this.f(true); }
}

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

class ArithMain {
    public static void main(String[] a) {
        System.out.println(new Calc().run(17, 5));
    }
}

class Calc {
    public int run(int b, int c) {
        int x;
        int y;
        x = b / 2 + c % 3 * 4;
        y = (b - c) / 2 - b % c;
        x = x * y / 3 % 7;
        System.out.println(x + y);
        return x - y;
    }
}
class RelMain {
    public static void main(String[] a) {
        System.out.println(new Check().test(3, 7, 9));
    }
}

class Check {
    public boolean test(int p, int q, int r) {
        boolean res;
        res = p <= q || q > r && r >= p;
        if (p > q || !(q <= r)) res = true; else res = false;
        while (p < q && q != r || p == 0) {
            p = p + 1;
        }
        return res == true || p >= 0;
    }
}
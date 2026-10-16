class NullMain {
    public static void main(String[] a) {
        System.out.println(new Node().test(new Node()));
    }
}

class Node {
    Node next;
    int val;

    public boolean test(Node other) {
        Node n;
        boolean b;
        n = null;
        next = null;
        b = n == null;
        if (other != null && n == next) b = true; else b = false;
        return b || other == null;
    }
}
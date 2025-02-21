package Java;

public class MET01J {

    public static int foo(int x) { // THIS IS NON-COMPLIANT
        assert x > 0 : "x must be greater than 0";
        assert x < 100 : "x must be less than 100";
        return x * x;
    }

    public static int bar(int x) { // THIS IS COMPLIANT
        if (x < 0) {
            throw new IllegalArgumentException("x must be greater than 0");
        }
        if (x >= 100) {
            throw new IllegalArgumentException("x must be less than 100");
        }
        return x * x;
    }

    public static void main(String[] args) {
        System.out.println(foo(5));
        System.out.println(bar(5));
    }
}


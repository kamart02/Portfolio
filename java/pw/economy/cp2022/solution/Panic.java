package cp2022.solution;

public class Panic extends RuntimeException {
    public Panic() {
        super("panic: unexpected thread interruption");
    }
}

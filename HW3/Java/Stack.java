import java.util.Arrays;
import java.util.EmptyStackException;

public class Stack {
    private static final int MAXIMUM_CAPACITY = 65536;
    private static final int STARTING_CAPACITY = 16;
    private String[] values;
    private int topIndex;

    /**
     * Utility class for validating stack operations.
     */
    static class Validate {
        public static void isNotEmpty(Stack stack) {
            if (stack.isEmpty()) {
                throw new EmptyStackException();
            }
        }
        public static void isNotFull(Stack stack) {
            if (stack.isFull()) {
                throw new IllegalStateException("Stack has reached maximum capacity");
            }
        }
        public static void isValidString(String value) {
            if (value == null) {
                throw new IllegalArgumentException("Value cannot be null");
            }
            if (value.length() == 0) {
                throw new IllegalArgumentException("Value cannot be empty");
            }
            if (value.length() > 16) {
                throw new IllegalArgumentException("Value cannot be longer than 16 characters");
            }
        }
    }

    /**
     * Constructs a new Stack with an initial capacity defined by STARTING_CAPACITY.
     * Initializes the internal array to hold the stack values and sets the initial size to 0.
     */
    public Stack() {
        values = new String[STARTING_CAPACITY];
        topIndex = 0;
    }

    /**
     * Pushes a new value onto the stack. If the stack is at full capacity,
     * it will expand the capacity before adding the new value.
     *
     * @param value the string value to be pushed onto the stack
     */
    public void push(String value) {
        Validate.isValidString(value);
        if (topIndex == values.length) {
            expandCapacity();
        }
        values[topIndex++] = value;
    }

    /**
     * Removes and returns the top element of the stack.
     *
     * @return the top element of the stack
     * @throws IllegalStateException if the stack is empty
     */
    public String pop() {
        Validate.isNotEmpty(this);
        String value = values[--topIndex];
        values[topIndex] = null;
        return value;
    }

    /**
     * Returns a copy of the top element of the stack without removing it.
     *
     * @return the top string element of the stack
     * @throws EmptyStackException if the stack is empty
     */
    public String peek() {
        Validate.isNotEmpty(this);
        return values[topIndex - 1];
    }

    /**
     * Expands the capacity of the stack by doubling the current size of the array.
     * If the current size has already reached the maximum capacity, an IllegalStateException
     * is thrown.
     *
     * @throws IllegalStateException if the stack has reached its maximum capacity.
     */
    private void expandCapacity() {
        Validate.isNotFull(this);
        values = Arrays.copyOf(values, values.length * 2);
    }

    /**
     * @return true if the stack is full, false otherwise.
     */
    public boolean isFull() {
        return size() >= MAXIMUM_CAPACITY;
    }

    /**
     * @return true if the stack is empty, false otherwise.
     */
    public boolean isEmpty() {
        return topIndex == 0;
    }

    /**
     * @return the size of the stack
     */
    public int size() {
        return topIndex;
    }
}
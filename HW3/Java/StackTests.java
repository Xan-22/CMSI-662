import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.util.EmptyStackException;

public class StackTests {
    private Stack stack;

    @BeforeEach
    public void setUp() {
        stack = new Stack();
    }

    @Test
    public void testPush() {
        for (int i = 0; i <= 5; i++) {
            stack.push("Test " + i);
        }
        assertFalse(stack.isEmpty());
    }

    @Test
    public void testPop() {
        for (int i = 0; i <= 5; i++) {
            stack.push("Test " + stack.size());
        }
        while (!stack.isEmpty()) {
            String value = stack.pop();
            assertEquals("Test " + stack.size(), value);
        }
        assertTrue(stack.isEmpty());
    }

    @Test
    public void testPeek() {
        for (int i = 0; i < 3; i++) {
            stack.push("Test " + stack.size());
        }
        assertFalse(stack.isEmpty());
        String value = stack.peek();
        assertEquals(stack.size(), 3);
        assertEquals("Test " + (stack.size() - 1), value);
    }

    @Test
    public void testIsEmpty() {
        assertTrue(stack.isEmpty());
        stack.push("Test 1");
        assertFalse(stack.isEmpty());
        stack.pop();
        assertTrue(stack.isEmpty());
    }

    @Test
    public void testSize() {
        assertEquals(0, stack.size());
        stack.push("Test 1");
        assertEquals(1, stack.size());
        stack.push("Test 2");
        assertEquals(2, stack.size());
        stack.pop();
        assertEquals(1, stack.size());
    }

    @Test
    public void testPopEmptyStack() {
        assertThrows(EmptyStackException.class, () -> {
            stack.pop();
        });
    }

    @Test
    public void testPeekEmptyStack() {
        assertThrows(EmptyStackException.class, () -> {
            stack.peek();
        });
    }

    @Test
    public void testPushFullStack() {
        while (!stack.isFull()) {
            stack.push("Test " + stack.size());
        }
        assertThrows(IllegalStateException.class, () -> {
            stack.push("Test Full");
        });
        assertTrue(stack.isFull());
    }
}
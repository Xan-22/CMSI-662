package Java;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;

public class MSC06J {
    public static class Item {
        private final String name;
        public Item(String name) { this.name = name; }
        public String getName() { return name; }
    }

    public static void main(String[] args) {
        List<Item> items = Collections.synchronizedList(new ArrayList<>());

        for (int i = 0; i < 3; i++) {
            final int threadIndex = i;
            Thread thread = new Thread(() -> {
                synchronized (items) {
                    if (items.size() > 1) {
                        System.out.println("Thread " + threadIndex + ": Modifying a previous item");
                        items.set(0, new Item("Thread " + threadIndex + ": Modified Item 0"));
                    }
                    for (int j = 0; j < 5; j++) {
                        items.add(new Item("Thread " + threadIndex + ": Item " + j));
                    }
                }
            });
            thread.start();
        }
        try {
            Thread.sleep(1000); // Wait for threads to finish
            for (Item item : items) {
                System.out.println(item.getName());
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

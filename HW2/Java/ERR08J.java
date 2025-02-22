package Java;

public class ERR08J {

    static final int MAX_ITEMS = 10;

    public static void checkIndex(int index) {
        if (index < 0 || index >= MAX_ITEMS) {
            throw new IllegalArgumentException("Index out of bounds");
        }
    }

    public static class Item {
        private final String name;
        public Item(String name) { this.name = name; }
        public String getName() { return name; }
    }

    public static class Container {
        private final Item[] items;

        public Container() {
            this.items = new Item[MAX_ITEMS];
        }
        public Item getItem(int index) {
            checkIndex(index);
            return items[index];
        }
        public void setItem(int index, Item item) {
            checkIndex(index);
            items[index] = item;
        }
    }

    public static void main(String[] args) {
        Container container = new Container();
        try
        {
            Item item = container.getItem(11); // Expected to throw an exception
            // Not executed due to our hardcoded index, but null checking should be done here
            if (item != null) {
                System.out.println(item.getName());
            }
        } catch (IllegalArgumentException e) { // COMPLIANT: Only catching expected exception
            System.out.println(e.getMessage()); // Output: Index out of bounds
        }
    }
}

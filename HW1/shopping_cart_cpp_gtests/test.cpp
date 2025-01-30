#include "pch.h"
#include "../shopping_cart_cpp/cart.h"
#include <regex>
#include <random>

TEST(ShoppingCartTest, DestructorIsLinked) {
    ShoppingCart cart("ABC12345DE-A");
    // The destructor will be called automatically when 'cart' goes out of scope.
    SUCCEED();
}

TEST(ShoppingCartTest, NonEnglishID) { // Not currently passing
	ShoppingCart cart("アイウ12345エオ-A");
    ASSERT_THROW(ShoppingCart cart("ABC12345DE-ア"), std::invalid_argument);
}

TEST(ShoppingCartTest, InvalidOwnerID) {
    ASSERT_THROW(ShoppingCart cart("INVALID_ID"), std::invalid_argument);
    ASSERT_THROW(ShoppingCart cart("ABC12345DE-Z"), std::invalid_argument);
    ASSERT_THROW(ShoppingCart cart("A"), std::invalid_argument);
}

TEST(ShoppingCartTest, CartIDIsUUID4) {
    // UUID4 pattern: "xxxxxxxx-xxxx-4xxx-[89ab]xxx-xxxxxxxxxxxx"
    // Source: https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
    std::regex uuid4_pattern("^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$", std::regex::icase);
	std::string test = "c446bf02-a1bd-4a30-8da2-bf22976ec9f3"; // Random UUID4 for testing regex
    ASSERT_TRUE(std::regex_match(test, uuid4_pattern)); // This should always pass

    ShoppingCart cart("ABC12345DE-A");
    std::string cart_id = cart.getCartId();
    ASSERT_TRUE(std::regex_match(cart_id, uuid4_pattern));
}

TEST(ShoppingCartTest, AddItem) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 3);
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, AddExistingItem) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items["banana"], 5);
	cart.addItem("apple", 4);
    items = cart.getItems();
    ASSERT_EQ(items["apple"], 7);
    ASSERT_EQ(items["banana"], 5);
    ASSERT_EQ(items.size(), 2);
}

TEST(ShoppingCartTest, AddBadItem) {
    ShoppingCart cart("ABC12345DE-A");
    try {
        cart.addItem("zzz", 3);
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Item not found in catalog");
    }
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 0);
}

TEST(ShoppingCartTest, AddBadQuantity) {
    ShoppingCart cart("ABC12345DE-A");
    try {
        cart.addItem("apple", 10000);
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Quantity cannot be greater than 99");
    }
    try {
        cart.addItem("apple", -99);
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Quantity cannot be less than 1");
    }
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 0);
}

TEST(ShoppingCartTest, UpdateItem) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	cart.updateItem("apple", 5);
	auto items = cart.getItems();
	ASSERT_EQ(items["apple"], 5);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, UpdateMissingItem) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 3);
    try {
		cart.updateItem("banana", 5);
	}
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Cannot update an item not present in cart");
    }
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, UpdateBadQuantity) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	try {
		cart.updateItem("apple", 10000);
	}
	catch (const std::exception& e)
	{
		ASSERT_STREQ(e.what(), "Quantity cannot be greater than 99");
	}
	try {
		cart.updateItem("apple", -99);
	}
	catch (const std::exception& e)
	{
		ASSERT_STREQ(e.what(), "Quantity cannot be less than 1");
	}
	auto items = cart.getItems();
	ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, RemoveItem) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	cart.removeItem("apple");
	auto items = cart.getItems();
	ASSERT_EQ(items.size(), 0);
}

TEST(ShoppingCartTest, RemoveMissingItem) {
    ShoppingCart cart("ABC12345DE-A");
    try {
        cart.removeItem("apple");
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Cannot remove an item not present in cart");
    }
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 0);
}

TEST(ShoppingCartTest, RemoveItemMissingFromList) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("banana", 3);
    try {
		cart.removeItem("apple");
    }
	catch (const std::exception& e)
	{
		ASSERT_STREQ(e.what(), "Cannot remove an item not present in cart");
	}
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, TotalCost) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	cart.addItem("banana", 5);
	double total = cart.getTotalCost();
	ASSERT_EQ(total, 3 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 2);
}

TEST(ShoppingCartTest, TotalCostWithManyItems) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 99);
    cart.addItem("banana", 99);
	cart.addItem("orange", 99);
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 99 * 0.5 + 99 * 0.25 + 99 * 0.75);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 3);
}

TEST(ShoppingCartTest, TotalCostAfterUpdate) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	cart.addItem("banana", 5);
	cart.updateItem("apple", 5);
	double total = cart.getTotalCost();
	ASSERT_EQ(total, 5 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 2);
}

TEST(ShoppingCartTest, TotalCostAfterRemoval) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    cart.removeItem("apple");
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 1);
}
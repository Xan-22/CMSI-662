#include "pch.h"
#include "../shopping_cart_cpp/cart.h"
#include <regex>
#include <random>

TEST(ShoppingCartTest, DestructorIsLinked) {
    ShoppingCart cart("ABC12345DE-A");
    // The destructor will be called automatically when 'cart' goes out of scope.
    SUCCEED();
}

TEST(ShoppingCartTest, AddItem) {
    ShoppingCart cart("ABC12345DE-A");
    cart.addItem("apple", 3);
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 3);
}

TEST(ShoppingCartTest, NonEnglishID) {
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

TEST(ShoppingCartTest, UpdateItem) {
	ShoppingCart cart("ABC12345DE-A");
	cart.addItem("apple", 3);
	cart.updateItem("apple", 5);
	auto items = cart.getItems();
	ASSERT_EQ(items["apple"], 5);
}
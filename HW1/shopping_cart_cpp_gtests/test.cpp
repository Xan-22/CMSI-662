#include "pch.h"
#include "../shopping_cart_cpp/cart.h"
#include <regex>
#include <random>

TEST(ShoppingCartTest, CopyConstructor) {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(cart1);
    auto items = cart2.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, MoveConstructor) {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(std::move(cart1));
    auto items = cart2.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, CopyAssignment) {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(L"XYZ67890FG-Q");
    cart2 = cart1;
    auto items = cart2.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, MoveAssignment) {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(L"XYZ67890FG-Q");
    cart2 = std::move(cart1);
    auto items = cart2.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, NonEnglishID) {
    ShoppingCart cart(L"アイウ12345エオ-A");
    ASSERT_THROW(ShoppingCart cart(L"ABC12345DE-ア"), std::invalid_argument);
    try {
        ShoppingCart cart(L"アイウアイウ12345DE-ア");
    }
	catch (const std::exception& e)
	{
        ASSERT_STREQ(e.what(), "Owner ID must be 12 characters long");
	}
}

TEST(ShoppingCartTest, InvalidOwnerID) {
    try {
		ShoppingCart cart(L"INVALID_ID");
	}
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Invalid owner ID format");
    }
    ASSERT_THROW(ShoppingCart cart(L"ABC12345DE-Z"), std::invalid_argument);
    ASSERT_THROW(ShoppingCart cart(L"A"), std::invalid_argument);
    ASSERT_THROW(ShoppingCart cart(L"123ABCDE45-A"), std::invalid_argument);
}

TEST(ShoppingCartTest, LargeOwnerID) {
    try {
        ShoppingCart cart(L"AAAAAAAAAAAAAA12345AA-ZZ");
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Owner ID must be 12 characters long");
    }
    try {
        ShoppingCart cart(L"AAA12345AA-ZZ");
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Owner ID must be 12 characters long");
    }
    try {
        ShoppingCart cart(L"昔ちイぼ準日ミマアナ週含げ楽果へふ村浩ょッ後高しば前質63作ウクメ色海エレ属熱あうるす難郎毎画カ旅趣くまフち広室ムオエ長屋フ被界サテオ群達ワキラコ資意たぎ他張フぴまた。下牟イふ子申延みフょせ皮川い断2属カエセ膜合ヲフコ反南あ禁58見2話子ちラぐめ行誌統極飲わべゃ。除ルイツ佐利なゃ員質ず書巨ぼ話4割ほリぶ足録ざえ事攻タネモセ売口ネヲケカ署委問芸リテ昇展チラ客主岩極境れめてき。");
    }
    catch (const std::exception& e)
    {
        ASSERT_STREQ(e.what(), "Owner ID must be 12 characters long");
    }
}

TEST(ShoppingCartTest, CartIDIsUUID4) {
    // UUID4 pattern: "xxxxxxxx-xxxx-4xxx-[89ab]xxx-xxxxxxxxxxxx"
    // Source: https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
    std::regex uuid4_pattern("^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$", std::regex::icase);
    std::string test = "c446bf02-a1bd-4a30-8da2-bf22976ec9f3"; // Random UUID4 for testing regex
    ASSERT_TRUE(std::regex_match(test, uuid4_pattern)); // This should always pass

    ShoppingCart cart(L"ABC12345DE-A");
    std::string cart_id = cart.getCartId();
    ASSERT_TRUE(std::regex_match(cart_id, uuid4_pattern));
}

TEST(ShoppingCartTest, AddItem) {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 3);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, AddExistingItem) {
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.updateItem("apple", 5);
    auto items = cart.getItems();
    ASSERT_EQ(items["apple"], 5);
    ASSERT_EQ(items.size(), 1);
}

TEST(ShoppingCartTest, UpdateMissingItem) {
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.removeItem("apple");
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 0);
}

TEST(ShoppingCartTest, RemoveMissingItem) {
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
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
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 3 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 2);
}

TEST(ShoppingCartTest, TotalCostWithManyItems) {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 99);
    cart.addItem("banana", 99);
    cart.addItem("orange", 99);
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 99 * 0.5 + 99 * 0.25 + 99 * 0.75);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 3);
}

TEST(ShoppingCartTest, TotalCostAfterUpdate) {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    cart.updateItem("apple", 5);
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 5 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 2);
}

TEST(ShoppingCartTest, TotalCostAfterRemoval) {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    cart.removeItem("apple");
    double total = cart.getTotalCost();
    ASSERT_EQ(total, 5 * 0.25);
    auto items = cart.getItems();
    ASSERT_EQ(items.size(), 1);
}
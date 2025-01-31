#include "cart.h"
#include <assert.h>
#include <regex>
#include <random>

static void TEST_CopyConstructor() {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(cart1);
    auto items = cart2.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_MoveConstructor() {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(std::move(cart1));
    auto items = cart2.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_CopyAssignment() {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(L"XYZ67890FG-Q");
    cart2 = cart1;
    auto items = cart2.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_MoveAssignment() {
    ShoppingCart cart1(L"ABC12345DE-A");
    cart1.addItem("apple", 3);
    ShoppingCart cart2(L"XYZ67890FG-Q");
    cart2 = std::move(cart1);
    auto items = cart2.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_NonEnglishID() {
    ShoppingCart cart(L"アイウ12345エオ-A");
    try {
        ShoppingCart cart(L"ABC12345DE-ア");
    }
	catch (const std::exception& e)
	{
		assert(strcmp(e.what(), "Invalid owner ID format") == 0);
	}

    try
    {
        ShoppingCart cart(L"アイウアイウ12345DE-ア");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Owner ID must be 12 characters long") == 0);
    }
}

static void TEST_InvalidOwnerID() {
    try {
        ShoppingCart cart(L"INVALID_ID");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Invalid owner ID format") == 0);
    }
	try {
		ShoppingCart cart(L"ABC12345DE-Z");
	}
	catch (const std::exception& e)
	{
		assert(strcmp(e.what(), "Invalid owner ID format") == 0);
	}
	try {
		ShoppingCart cart(L"A");
	}
	catch (const std::exception& e)
	{
		assert(strcmp(e.what(), "Invalid owner ID format") == 0);
	}
	try {
		ShoppingCart cart(L"123ABCDE45-A");
	}
	catch (const std::exception& e)
	{
		assert(strcmp(e.what(), "Invalid owner ID format") == 0);
	}
}

static void TEST_LargeOwnerID() {
    try {
        ShoppingCart cart(L"AAAAAAAAAAAAAA12345AA-ZZ");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Owner ID must be 12 characters long") == 0);
    }
    try {
        ShoppingCart cart(L"AAA12345AA-ZZ");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Owner ID must be 12 characters long") == 0);
    }
    try {
        ShoppingCart cart(L"昔ちイぼ準日ミマアナ週含げ楽果へふ村浩ょッ後高しば前質63作ウクメ色海エレ属熱あうるす難郎毎画カ旅趣くまフち広室ムオエ長屋フ被界サテオ群達ワキラコ資意たぎ他張フぴまた。下牟イふ子申延みフょせ皮川い断2属カエセ膜合ヲフコ反南あ禁58見2話子ちラぐめ行誌統極飲わべゃ。除ルイツ佐利なゃ員質ず書巨ぼ話4割ほリぶ足録ざえ事攻タネモセ売口ネヲケカ署委問芸リテ昇展チラ客主岩極境れめてき。");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Owner ID must be 12 characters long") == 0);
    }
}

static void TEST_CartIDIsUUID4() {
    // UUID4 pattern: "xxxxxxxx-xxxx-4xxx-[89ab]xxx-xxxxxxxxxxxx"
    // Source: https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
    std::regex uuid4_pattern("^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$", std::regex::icase);
    std::string test = "c446bf02-a1bd-4a30-8da2-bf22976ec9f3"; // Random UUID4 for testing regex
    assert(std::regex_match(test, uuid4_pattern)); // This should always pass

    ShoppingCart cart(L"ABC12345DE-A");
    std::string cart_id = cart.getCartId();
    assert(std::regex_match(cart_id, uuid4_pattern));
}

static void TEST_AddItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    auto items = cart.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_AddExistingItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    auto items = cart.getItems();
    assert(items["apple"] == 3);
    assert(items["banana"] == 5);
    cart.addItem("apple", 4);
    items = cart.getItems();
    assert(items["apple"] == 7);
    assert(items["banana"] == 5);
    assert(items.size() == 2);
}

static void TEST_AddBadItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    try {
        cart.addItem("zzz", 3);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Item not found in catalog") == 0);
    }
    auto items = cart.getItems();
    assert(items.size() == 0);
}

static void TEST_AddBadQuantity() {
    ShoppingCart cart(L"ABC12345DE-A");
    try {
        cart.addItem("apple", 10000);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Quantity cannot be greater than 99") == 0);
    }
    try {
        cart.addItem("apple", -99);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Quantity cannot be less than 1") == 0);
    }
    auto items = cart.getItems();
    assert(items.size() == 0);
}

static void TEST_UpdateItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.updateItem("apple", 5);
    auto items = cart.getItems();
    assert(items["apple"] == 5);
    assert(items.size() == 1);
}

static void TEST_UpdateMissingItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    try {
        cart.updateItem("banana", 5);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Cannot update an item not present in cart") == 0);
    }
    auto items = cart.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_UpdateBadQuantity() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    try {
        cart.updateItem("apple", 10000);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Quantity cannot be greater than 99") == 0);
    }
    try {
        cart.updateItem("apple", -99);
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Quantity cannot be less than 1") == 0);
    }
    auto items = cart.getItems();
    assert(items["apple"] == 3);
    assert(items.size() == 1);
}

static void TEST_RemoveItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.removeItem("apple");
    auto items = cart.getItems();
    assert(items.size() == 0);
}

static void TEST_RemoveMissingItem() {
    ShoppingCart cart(L"ABC12345DE-A");
    try {
        cart.removeItem("apple");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Cannot remove an item not present in cart") == 0);
    }
    auto items = cart.getItems();
    assert(items.size() == 0);
}

static void TEST_RemoveItemMissingFromList() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("banana", 3);
    try {
        cart.removeItem("apple");
    }
    catch (const std::exception& e)
    {
        assert(strcmp(e.what(), "Cannot remove an item not present in cart") == 0);
    }
    auto items = cart.getItems();
    assert(items.size() == 1);
}

static void TEST_TotalCost() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    double total = cart.getTotalCost();
    assert(total == 3 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    assert(items.size() == 2);
}

static void TEST_TotalCostWithManyItems() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 99);
    cart.addItem("banana", 99);
    cart.addItem("orange", 99);
    double total = cart.getTotalCost();
    assert(total == 99 * 0.5 + 99 * 0.25 + 99 * 0.75);
    auto items = cart.getItems();
    assert(items.size() == 3);
}

static void TEST_TotalCostAfterUpdate() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    cart.updateItem("apple", 5);
    double total = cart.getTotalCost();
    assert(total == 5 * 0.5 + 5 * 0.25);
    auto items = cart.getItems();
    assert(items.size() == 2);
}

static void TEST_TotalCostAfterRemoval() {
    ShoppingCart cart(L"ABC12345DE-A");
    cart.addItem("apple", 3);
    cart.addItem("banana", 5);
    cart.removeItem("apple");
    double total = cart.getTotalCost();
    assert(total == 5 * 0.25);
    auto items = cart.getItems();
    assert(items.size() == 1);
}

int main(int argc, char** argv) {
    TEST_CopyConstructor();
	TEST_MoveConstructor();
	TEST_CopyAssignment();
	TEST_MoveAssignment();
	TEST_NonEnglishID();
	TEST_InvalidOwnerID();
	TEST_LargeOwnerID();
	TEST_CartIDIsUUID4();
	TEST_AddItem();
	TEST_AddExistingItem();
	TEST_AddBadItem();
	TEST_AddBadQuantity();
	TEST_UpdateItem();
	TEST_UpdateMissingItem();
	TEST_UpdateBadQuantity();
	TEST_RemoveItem();
	TEST_RemoveMissingItem();
	TEST_RemoveItemMissingFromList();
	TEST_TotalCost();
	TEST_TotalCostWithManyItems();
	TEST_TotalCostAfterUpdate();
	TEST_TotalCostAfterRemoval();
}
#include "cart.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <regex>
#include <utility>

namespace Catalog {
	static std::map<std::string, double> fetchItems() {
		// This is where items would be fetched from a database.
		return {
			{"apple", 0.5},
			{"banana", 0.25},
			{"orange", 0.75},
			{"grapes", 1.0},
			{"pineapple", 2.0}
		};
	}
	static std::pair<std::string, double> getItem(const std::string& item) {
		std::map items = fetchItems();
		std::string item_str(item);
		auto iter = items.find(item_str);
		if (iter != items.end()) {
			return std::make_pair(iter->first, iter->second);
		}
		else {
			throw std::invalid_argument("Item not found in catalog");
		}
	}
};

class ItemName {
public:
    ItemName(const std::string& name) : name(name) {}
    std::string get() const { return name; };
	bool operator<(const ItemName& other) const {
		return name < other.name;
	}
private:
    std::string name;
};

class Quantity {
public:
	Quantity() : quantity(0) {}
    Quantity(int quantity) {
		if (quantity < 1) {
			throw std::invalid_argument("Quantity cannot be less than 1");
		}
		if (quantity > 99) {
			throw std::invalid_argument("Quantity cannot be greater than 99");
		}
		this->quantity = quantity;
	}
    int get() const { return quantity; }
private:
    int quantity;
};

class CartID {
public:
	CartID() {
		this->id = generate_uuid4();
	}
	std::string get() const { return id; };
private:
	std::string generate_uuid4() {
		// Credit to happy_sisyphus on Stackoverflow: https://stackoverflow.com/a/60198074
		// The alternative to this would be to use an external library like Boost.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis(0, 15); // 0-f
		std::uniform_int_distribution<uint32_t> encoding_bits(8, 11); // 8-b

		std::stringstream ss;
		int i;
		ss << std::hex;
		for (i = 0; i < 8; i++) {
			ss << dis(gen);
		}
		ss << "-";
		for (i = 0; i < 4; i++) {
			ss << dis(gen);
		}
		ss << "-4";
		for (i = 0; i < 3; i++) {
			ss << dis(gen);
		}
		ss << "-" << encoding_bits(gen);
		for (i = 0; i < 3; i++) {
			ss << dis(gen);
		}
		ss << "-";
		for (i = 0; i < 12; i++) {
			ss << dis(gen);
		}
		return ss.str();
	}
	std::string id;
};

class OwnerID {
public:
	OwnerID() { this->id = L""; }
	OwnerID(const std::wstring& id) {
		if (id.size() > 12) {
			throw std::invalid_argument("Owner ID must be 12 characters long");
		}
		// Must be 3 letters, 5 numbers, 2 letters, a dash, and an A or a Q.
		std::wregex pattern(L"^[A-Z\u0080-\uFFFF]{3}[0-9]{5}[A-Z\u0080-\uFFFF]{2}-[AQ]$", std::regex_constants::icase);
		if (!std::regex_match(id, pattern)) {
			throw std::invalid_argument("Invalid owner ID format");
		}
		this->id = id;
	}
	std::wstring get() const { return id; };
private:
	std::wstring id;
};

struct ShoppingCart::ShoppingCartData {
	OwnerID owner_id;
	CartID cart_id;
	std::map<ItemName, Quantity> items;
};

ShoppingCart::ShoppingCart(const std::wstring& owner_id) {
	data = std::make_unique<ShoppingCartData>(OwnerID(owner_id), CartID(), std::map<ItemName, Quantity>());
}
ShoppingCart::~ShoppingCart() = default;

ShoppingCart::ShoppingCart(const ShoppingCart& other) {
	data = std::make_unique<ShoppingCartData>(other.data->owner_id, other.data->cart_id, other.data->items);
};

ShoppingCart::ShoppingCart(ShoppingCart&& other) noexcept = default;

ShoppingCart& ShoppingCart::operator=(const ShoppingCart& other) {
	data = std::make_unique<ShoppingCartData>(other.data->owner_id, other.data->cart_id, other.data->items);
	return *this;
};

ShoppingCart& ShoppingCart::operator=(ShoppingCart&& other) noexcept = default;

std::wstring ShoppingCart::getId() const { return data->owner_id.get(); }
std::string ShoppingCart::getCartId() const { return data->cart_id.get(); }
std::map <std::string, int> ShoppingCart::getItems() const {
		std::map <std::string, int> copied_items;
		for (const auto& item : data->items) {
			copied_items[item.first.get()] = item.second.get();
		}
		return copied_items; 
	}

void ShoppingCart::addItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		// Only add an item if it exists in the catalog
		std::ignore = Catalog::getItem(item_name);
		// If the item already exists, add the quantity to the existing quantity.
		if (data->items.find(item) != data->items.end()) {
			data->items[item] = Quantity(data->items[item].get() + quantity.get());
		}
		else {
			data->items[item] = quantity;
		}
    }

void ShoppingCart::updateItem(const std::string item_name, int amount) {
		ItemName item(item_name);
		Quantity quantity(amount);
		if (data->items.find(item) == data->items.end()) {
			throw std::invalid_argument("Cannot update an item not present in cart");
		}
		data->items[item] = quantity;
	}

void ShoppingCart::removeItem(const std::string item_name) {
		ItemName item(item_name);
		if (data->items.find(item) == data->items.end()) {
			throw std::invalid_argument("Cannot remove an item not present in cart");
		}
		data->items.erase(item);
	}

double ShoppingCart::getTotalCost() const {
		double total = 0.0;
		for (const auto& item : data->items) {
			total += (double)item.second.get() * Catalog::getItem(item.first.get()).second;
		}
		return total;
	}
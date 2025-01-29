#pragma once
#include <map>
#include <string>
#include <memory>

class ShoppingCart {
public:
	ShoppingCart(const std::string& owner_id);
	~ShoppingCart(); // Rule of Five
	ShoppingCart(const ShoppingCart& other);
	ShoppingCart(ShoppingCart&& other) noexcept;
	ShoppingCart& operator=(const ShoppingCart& other);
	ShoppingCart& operator=(ShoppingCart&& other) noexcept;

	std::string getId() const;
	std::string getCartId() const;
	std::map <std::string, int> getItems() const;
	void addItem(const std::string item_name, int amount);
	void updateItem(const std::string item_name, int amount);
	void removeItem(const std::string item_name);
	double getTotalCost() const;
private:
	// Using the pimpl idiom: https://herbsutter.com/gotw/_100/
	struct ShoppingCartData;
	std::unique_ptr<ShoppingCartData> data;
};
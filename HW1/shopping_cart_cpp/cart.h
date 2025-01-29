#pragma once
#include <map>

class ShoppingCart { // TODO: DOUBLE CHECK ALL. MAKE SURE THEY HAVE DEFINITIONS
public:
	ShoppingCart(const std::string& owner_id);

	std::string getId() const;
	std::string getCartId() const;

	void addItem(const std::string item_name, int amount);

	void updateItem(const std::string item_name, int amount);

	void removeItem(const std::string item_name);

	double getTotalCost() const;
};
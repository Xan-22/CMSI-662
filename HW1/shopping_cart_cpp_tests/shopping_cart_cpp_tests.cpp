#include "pch.h"
#include "CppUnitTest.h"
#include "../shopping_cart_cpp/cart.h"
#include "Assert.h"
#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace shoppingcartcpptests
{
	TEST_CLASS(shoppingcartcpptests)
	{
	public:
		
		TEST_METHOD(CartCreated)
		{
			ShoppingCart cart("ABC12345DE-A");
		}
		TEST_METHOD(ValidCustomerID)
		{
			// Must be 3 letters, 5 numbers, 2 letters, a dash, and an A or a Q

			ShoppingCart cart(std::string("ABCFEWAF"));
		}
	};
}

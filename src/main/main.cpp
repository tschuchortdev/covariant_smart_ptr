#include <iostream>
#include "shared_ptr.hpp"

class Base {};
class Derived : public Base {
	int x;
};
class NotDerived {
	float y;
};
class WithCtorArgs {
public:
	WithCtorArgs(int, float, float, int) {}
};

void foo(my_smart_ptr<Base>) {}

int main() {
	auto with_args = my_smart_ptr<WithCtorArgs>::make_shared(1, 2.0, 3.0, 4);

	auto derived = my_smart_ptr{new Derived{}};
	auto not_derived = my_smart_ptr{new NotDerived{}};
	foo(derived);
	foo(not_derived);

    return 0;
}
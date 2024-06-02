#include <gtest/gtest.h>

int main(int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);
	testing::GTEST_FLAG(color) = "yes";
	auto res {RUN_ALL_TESTS()};

	return res;
}

// TestNFA.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "gtest/gtest.h"

//#include "regex_expression.h"
//#include "nfa.h"

int Foo(int a, int b)
{
	if (a == 0 || b == 0)
	{
		throw "don't do that";
	}
	int c = a % b;
	if (c == 0)
		return b;
	return Foo(b, c);
}

TEST(FooTest, HandleNoneZeroInput)
{
	EXPECT_EQ(2, Foo(4, 10));
	EXPECT_EQ(6, Foo(30, 18));
}

int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	return 0;
}


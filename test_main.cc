//#include "stdafx.h"
#include "gtest/gtest.h"

#include "regex_expression_unit_test.h"

//#include "regex_expression.h"
#include "nfa.h"
#include "dfa.h"
#include "regex_expression.h"

#include <string>

TEST(RegexExpressionTest, GetNextSymbol)
{
	EXPECT_STREQ("a.b", RegexExpressionGenSymbol("ab").c_str());
	EXPECT_STREQ("(a.b)", RegexExpressionGenSymbol("(ab)").c_str());
	EXPECT_STREQ("(a*.b)", RegexExpressionGenSymbol("(a*b)").c_str());
	EXPECT_STREQ("a*|b", RegexExpressionGenSymbol("a*|b").c_str());
	EXPECT_STREQ("c.a.b", RegexExpressionGenSymbol("cab").c_str());
	EXPECT_STREQ("a*.(b.c|d).e", RegexExpressionGenSymbol("a*(bc|d)e").c_str());
	EXPECT_STREQ("a.(b.c)", RegexExpressionGenSymbol("a(bc)").c_str());
	EXPECT_STREQ("(a.c).b", RegexExpressionGenSymbol("(ac)b").c_str());
	EXPECT_STREQ("(a.b).(c.d)", RegexExpressionGenSymbol("(ab)(cd)").c_str());
}

TEST(RegexExpressionTest, PriorityHigherOrEqual)
{
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('|', '|'));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('(', '('));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('*', '*'));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('.', '.'));

	EXPECT_FALSE(RegexExpression::PriorityHigherOrEqual('|', '*'));
	EXPECT_FALSE(RegexExpression::PriorityHigherOrEqual('|', '.'));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('|', '('));

	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('*', '|'));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('*', '.'));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('*', '('));

	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('.', '('));
	EXPECT_TRUE(RegexExpression::PriorityHigherOrEqual('.', '|'));
	EXPECT_FALSE(RegexExpression::PriorityHigherOrEqual('.', '*'));
}

TEST(RegexExpressionTest, IsSpecialSymbol)
{
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol('.'));
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol('|'));
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol('*'));
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol('('));
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol(')'));

	EXPECT_FALSE(RegexExpression::IsSpecialSymbol('a'));
	EXPECT_FALSE(RegexExpression::IsSpecialSymbol('b'));
	EXPECT_FALSE(RegexExpression::IsSpecialSymbol('1'));
	EXPECT_FALSE(RegexExpression::IsSpecialSymbol('Z'));
	EXPECT_FALSE(RegexExpression::IsSpecialSymbol('-'));
}

/*
// 测试是否是正则表达式的特殊字符
class RegexExpressionTest : public::testing::TestWithParam<char>
{

};
TEST_P(RegexExpressionTest, IsSpecialSymbol)
{
	char ch =  GetParam();
	EXPECT_TRUE(RegexExpression::IsSpecialSymbol(ch));
}
// 定义参数
INSTANTIATE_TEST_CASE_P(TrueReturn, RegexExpressionTest, testing::Values('.', '*', '(', ')', '|'));
*/

int main(int argc, char* argv[])
{
	//testing::InitGoogleTest(&argc, argv);
	//return RUN_ALL_TESTS();
	//"(a*|b)c"通过
	//std::string re_str = "ab"; // pass
	std::string re_str = "((a*|b)c)*|b"; // pass
	//std::string re_str = "a*"; // pass
	RegexExpression re(re_str.c_str());
	NFA nfa(&re);
	nfa.PrintFA();

	DFA dfa;
	dfa.ConstructFromNFA(&nfa);
	dfa.PrintFA();
}


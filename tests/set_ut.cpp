#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "set.hpp"

TEST(Set, DefaultCtor) {
    set<int> s;
    EXPECT_EQ(0, s.size());
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.begin(), s.end());
}

TEST(Set, InitListCtor) {
    set<char> s1 = {'a', 'b', 'c'};
    EXPECT_EQ(3, s1.size());
    EXPECT_FALSE(s1.empty());
}

TEST(Set, CopyCtor) {
    set<char> s1 = {'a', 'b', 'c'};
    set<char> s2 = s1;

    EXPECT_EQ(s1.size(), s2.size());
    EXPECT_FALSE(s2.empty());

    EXPECT_TRUE(s2.contains('a'));
    EXPECT_TRUE(s2.contains('b'));
    EXPECT_TRUE(s2.contains('c'));
}

TEST(Set, IterBegin) {
    set<int> s = {16, 7, 8, 2, 5};
    EXPECT_EQ(2, s.begin()->value);

    const set<int> s1{3, 6, 1, 3};
    EXPECT_EQ(1, s1.cbegin()->value);
}

TEST(Set, IterDirections) {
    set<int> s = {5, 2, 1, 0, 9, 3};
    auto it = s.begin();
    ++it;
    EXPECT_EQ(it->value, 1);
    ++++it;
    ------it;
    EXPECT_EQ(it++, s.begin());
    EXPECT_EQ(it, ++s.begin());
}

TEST(Set, CopyAssignmentOperator) {
    set<int> s = {4, 8, 1, 2};
    set s2 = {6, 3, 1, 1};
    s = s2;

    ASSERT_THAT(s, ::testing::ElementsAre(1, 3, 6));
}

TEST(Set, MoveCtor) {
    set<double> s1 = {1.2, 3.14, 1.99};
    size_t sz = s1.size();
    set<double> s2 = std::move(s1);

    EXPECT_EQ(sz, s2.size());
    EXPECT_FALSE(s2.empty());

    EXPECT_EQ(0, s1.size());
    EXPECT_TRUE(s1.empty());

    EXPECT_TRUE(s2.contains(1.2));
    EXPECT_TRUE(s2.contains(3.14));
    EXPECT_TRUE(s2.contains(1.99));
}

TEST(Set, Equality) {
    set<int> s = {3, 2, 1};
    set<int> s1 = {1, 2, 3};
    set s2 = s;

    EXPECT_TRUE(s == s1 && s1 == s2);
    set<int> s4 = {7, 6, 3};
    EXPECT_FALSE(s == s4);
}

TEST(Set, Swap) {
    set<int> s = {3, 2, 1};
    set<int> s1 = {4, 6, 5};
    s.swap(s1);

    ASSERT_THAT(s, ::testing::ElementsAre(4, 5, 6));
    ASSERT_THAT(s1, ::testing::ElementsAre(1, 2, 3));
}

TEST(Set, Order) {
    set<int> s = {3, 2, 1};
    set<int> s1 = {4, 6, 5};
    EXPECT_TRUE(s < s1);
    EXPECT_TRUE(s1 > s);

    set<int> s2 = {9, 1, 0};
    set<int> s3 = {1, 3, 4};
    EXPECT_TRUE(s2 < s3);
    EXPECT_TRUE(s3 > s2);

    set<int> s4 = {1, 2, 3};
    set<int> s5 = {2, 1, 3};
    EXPECT_TRUE(s4 <= s5);
    EXPECT_TRUE(s5 >= s4);
}

TEST(Set, Insert) {
    set<int> s;
    s.insert(7);
    s.insert(1);
    s.insert(2);
    s.insert(3);
    ASSERT_THAT(s, ::testing::ElementsAre(1, 2, 3, 7));
}

TEST(Set, InsertWithReps) {
    set<int> s;
    s.insert(8);
    s.insert(8);
    s.insert(6);
    s.insert(4);
    s.insert(6);
    s.insert({3, 4, 6});
    ASSERT_THAT(s, ::testing::ElementsAre(3, 4, 6, 8));
    EXPECT_EQ(4, s.size());
    auto pair = s.insert(3);
    EXPECT_FALSE(pair.second);
    pair = s.insert(1);
    EXPECT_TRUE(pair.second);
}


TEST(Set, Emplace) {
    set<int> s;
    auto pair = s.emplace(10);
    EXPECT_TRUE(pair.second);
    EXPECT_EQ(s.size(), 1);

    pair = s.emplace(10);
    EXPECT_FALSE(pair.second);
    EXPECT_EQ(s.size(), 1);
    
    pair = s.emplace(20);
    EXPECT_TRUE(pair.second);
    EXPECT_EQ(s.size(), 2);
}

TEST(Set, EmplaceHint) {
    set<int> s = {8, 3, 4, 2};
    set<int>::iterator hint = s.begin();
    s.emplace_hint(hint, 1);
    hint = s.begin();
    ++++++++hint;
    s.emplace_hint(hint, 5);
    ASSERT_THAT(s, ::testing::ElementsAre(1, 2, 3, 4, 5, 8));
}

TEST(Set, Erase) {
    set<int> s;
    s.insert(3);
    s.insert(5);
    s.insert(2);
    s.insert(1);
    s.erase(1);
    ASSERT_THAT(s, ::testing::ElementsAre(2, 3, 5));
    s.erase(3);
    ASSERT_THAT(s, ::testing::ElementsAre(2, 5));
    s.erase(s.begin());
    ASSERT_THAT(s, ::testing::ElementsAre(5));
    s.erase(5);
    EXPECT_TRUE(s.empty());
}

TEST(Set, EraseWithReps) {
    set<int> s{1, 6, 3, 4};
    EXPECT_TRUE(s.erase(1));
    EXPECT_FALSE(s.erase(2));
    int expected[] = {4, 6};
    size_t ind = 0;
    for (auto it = s.erase(s.begin()); it != s.end(); ++it) {
        EXPECT_EQ(it->value, expected[ind++]);
    }
    s.erase(s.begin(), s.end());
    EXPECT_TRUE(s.empty());
}

TEST(Set, Clear) {
    set<char> s{'t', 'w', 'f', 'q'};
    s.clear();
    EXPECT_TRUE(s.empty());
}

TEST(Set, Find) {
    set<int> s = {10, 20, 30, 40, 50};
    
    auto it1 = s.find(10);
    EXPECT_EQ(it1->value, 10);
    
    auto it2 = s.find(30);
    EXPECT_EQ(it2->value, 30);

    auto it3 = s.find(50);
    EXPECT_EQ(it3->value, 50);

    EXPECT_EQ(s.find(0), s.end());
    EXPECT_EQ(s.find(4), s.end());
    EXPECT_EQ(s.find(42), s.end());

    set<char> s2;
    EXPECT_EQ(s.find('a'), s.end());


    set<int> s3 = {3, 4, 5, 6, 7, 8};
    int expected[] = {6, 7, 8};
    size_t ind = 0;
    for (auto it = s3.find(6); it != s3.end(); ++it) {
        EXPECT_EQ(it->value, expected[ind++]);
    }
}
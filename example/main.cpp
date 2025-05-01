#include <iostream>
#include "set.hpp"

int main() {
    set<int> myset;
    myset.insert(9);
    myset.insert(2);
    myset.insert(3);
    for (auto it = myset.begin(); it != myset.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
    myset.erase(myset.begin(), myset.end());
    for (auto it = myset.begin(); it != myset.end(); ++it) {
        std::cout << *it << ' ';
    }
    if (myset.empty()) std::cout << "empty\n";

    myset.insert({1,7,1,5,4});
    std::cout << myset.rbegin() << ' ';
    for (auto it = myset.rbegin(); it != myset.rend(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}
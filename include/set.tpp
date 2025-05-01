#pragma once
#include "set.hpp"

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>::set()
    : beginNode_(nullptr), fakeNode_(nullptr), sz_(0), iter(beginNode_) {}

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>::set(std::initializer_list<T> init, const Compare& comp, const Alloc& alloc): set() {
    insert(init);
}

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>::set(const set& other) {
    beginNode_ = allocateAndConstruct<NodeAlloc, Node<T>>(node_alloc);
    fakeNode_ = allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
    beginNode_->copy(other.fakeNode_->left, fakeNode_->left, other.fakeNode_->left, beginNode_, fakeNode_->right);
    sz_ = other.sz_;
    fakeNode_->right = allocateAndConstruct<NodeAlloc, Node<T>>(node_alloc);
}

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>::set(set&& other) noexcept {
    beginNode_ = other.beginNode_; other.beginNode_ = nullptr;
    fakeNode_ = other.fakeNode_; other.fakeNode_ = nullptr;
    sz_ = other.sz_; other.sz_ = 0;
    iter = other.iter; other.iter = {};
}

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>::~set() noexcept {
    if (!fakeNode_) return;
    fakeNode_->left->deleteTree(fakeNode_->left);
    destroyAndDeallocate(basenode_alloc, fakeNode_);
}

template <typename T, typename Compare, typename Alloc>
constexpr void set<T, Compare, Alloc>::swap(set& other) noexcept {
    std::swap(beginNode_, other.beginNode_);
    std::swap(fakeNode_, other.fakeNode_);
    std::swap(sz_, other.sz_);
    std::swap(node_alloc, other.node_alloc);
    std::swap(basenode_alloc, other.basenode_alloc);
}

template <typename T, typename Compare, typename Alloc>
set<T, Compare, Alloc>& set<T, Compare, Alloc>::operator=(set other) {
    swap(other);
    return *this;
}

template <typename T, typename Compare, typename Alloc>
constexpr bool set<T, Compare, Alloc>::operator==(const set& other) const noexcept {
    const_iterator iter1 = begin();
    const_iterator iter2 = other.begin();
    if (*iter1 != *iter2) return false;
    return true;
}

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
set<T, Compare, Alloc>::base_iterator<IsConst>::reference set<T, Compare, Alloc>::base_iterator<IsConst>::operator*() const 
{ return *ptr; }

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
set<T, Compare, Alloc>::base_iterator<IsConst>::pointer set<T, Compare, Alloc>::base_iterator<IsConst>::operator->() const 
{ return ptr; }

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
bool set<T, Compare, Alloc>::base_iterator<IsConst>::operator==(const base_iterator& iter) const
{ return this->ptr == iter.ptr; }

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
bool set<T, Compare, Alloc>::base_iterator<IsConst>::operator!=(const base_iterator& iter) const 
{ return !(*this == iter);}

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
constexpr typename set<T, Compare, Alloc>::template base_iterator<IsConst>&
set<T, Compare, Alloc>::base_iterator<IsConst>::operator++() noexcept {
    ptr = ptr->find_next(ptr->value, ptr);
    return *this;
}

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
constexpr typename set<T, Compare, Alloc>::template base_iterator<IsConst>
set<T, Compare, Alloc>::base_iterator<IsConst>::operator++(int) noexcept {
    pointer copy = ptr;
    ptr = ptr->find_next(ptr->value, ptr);
    return copy;
}

template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
constexpr typename set<T, Compare, Alloc>::template base_iterator<IsConst>&
set<T, Compare, Alloc>::base_iterator<IsConst>::operator--() noexcept {
    ptr = ptr->find_prev(ptr->value, ptr);
    return *this;
}


template <typename T, typename Compare, typename Alloc>
template <bool IsConst>
constexpr typename set<T, Compare, Alloc>::template base_iterator<IsConst>
set<T, Compare, Alloc>::base_iterator<IsConst>::operator--(int) noexcept {
    pointer copy = ptr;
    ptr = ptr->find_prev(ptr->value, ptr);
    return copy;
}

template <typename T, typename Compare, typename Alloc>
constexpr std::weak_ordering set<T, Compare, Alloc>::operator<=>(const set& other) const {
    return std::lexicographical_compare_three_way(this->begin(), this->end(),
        other.begin(), other.end(), [](const Node<T>& a, const Node<T>& b) {return a <=> b; });
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::iterator set<T, Compare, Alloc>::begin() noexcept { 
    return beginNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::iterator set<T, Compare, Alloc>::end() noexcept { 
    return fakeNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_iterator set<T, Compare, Alloc>::begin() const noexcept { 
    return beginNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_iterator set<T, Compare, Alloc>::end() const noexcept { 
    return fakeNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_iterator set<T, Compare, Alloc>::cbegin() const noexcept { 
    return beginNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_iterator set<T, Compare, Alloc>::cend() const noexcept { 
    return fakeNode_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::reverse_iterator set<T, Compare, Alloc>::rbegin() noexcept { 
    return reverse_iterator(fakeNode_->right); 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::reverse_iterator set<T, Compare, Alloc>::rend() noexcept { 
    return reverse_iterator(beginNode_);
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_reverse_iterator set<T, Compare, Alloc>::rbegin() const noexcept { 
    return reverse_iterator(fakeNode_->right); 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_reverse_iterator set<T, Compare, Alloc>::rend() const noexcept { 
    return reverse_iterator(beginNode_); 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_reverse_iterator set<T, Compare, Alloc>::crbegin() const noexcept { 
    return const_reverse_iterator(fakeNode_->right); 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::const_reverse_iterator set<T, Compare, Alloc>::crend() const noexcept { 
    return const_reverse_iterator(beginNode_);
}


template <typename T, typename Compare, typename Alloc>
std::pair<typename set<T, Compare, Alloc>::iterator, bool>
set<T, Compare, Alloc>::insert(const T& value) {
    if (fakeNode_ == nullptr) {
        fakeNode_ = allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
    }
    Node<T>* node = fakeNode_->left->insert(value, fakeNode_->left, beginNode_, fakeNode_->right);
    if (node == nullptr) return {iterator(find(value)), false};
    else {
        ++sz_;
        return {iterator(node), true};
    }
}

template <typename T, typename Compare, typename Alloc>
void set<T, Compare, Alloc>::insert(std::initializer_list<T> ilist) {
    for (const T& obj : ilist) this->insert(obj);
}

template <typename T, typename Compare, typename Alloc>
template <typename... Args>
std::pair<typename set<T, Compare, Alloc>::iterator, bool> 
set<T, Compare, Alloc>::emplace(Args&&...args) {
    if (fakeNode_ == nullptr) {
        fakeNode_ = allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
    }
    T value(std::forward<Args>(args)...);
    Node<T>* node = fakeNode_->left->insert(value, fakeNode_->left, beginNode_, fakeNode_->right);
    if (node == nullptr) return {iterator(find(value)), false};
    else {
        ++sz_;
        return {iterator(node), true};
    }
}

template <typename T, typename Compare, typename Alloc>
template <typename... Args>
set<T, Compare, Alloc>::iterator 
set<T, Compare, Alloc>::emplace_hint(set<T, Compare, Alloc>::iterator hint, Args&&...args) {
    auto ptr = hint.operator->(); 
    return fakeNode_->left->insert_hint(ptr, fakeNode_->left, beginNode_, fakeNode_->right, std::forward<Args>(args)...);
}

template <typename T, typename Compare, typename Alloc>
bool set<T, Compare, Alloc>::erase(const T& value) {
    bool erased = fakeNode_->left->remove(value, fakeNode_->left, beginNode_, fakeNode_->right);
    if (erased) --sz_;
    return erased;
}

template <typename T, typename Compare, typename Alloc>
typename set<T, Compare, Alloc>::iterator set<T, Compare, Alloc>::erase(iterator iter) {
    T value = iter->value;
    fakeNode_->left->remove(value, fakeNode_->left, beginNode_, fakeNode_->right);
    --sz_;
    return iter->find_next(value, fakeNode_->left);
}

template <typename T, typename Compare, typename Alloc>
typename set<T, Compare, Alloc>::iterator set<T, Compare, Alloc>::erase(iterator first, iterator last) {
    if (last == fakeNode_) last = fakeNode_->right;
    iterator begin = first;
    while (first->value != last->value) {
        ++first;
        erase(begin->value);
        begin = first;
    }
    if (last == fakeNode_->right) erase(last->value);
    return first;
}

template <typename T, typename Compare, typename Alloc>
constexpr void set<T, Compare, Alloc>::clear() noexcept {
    fakeNode_->left->deleteTree(fakeNode_->left);
    fakeNode_->left = nullptr;
    sz_ = 0;
}

template <typename T, typename Compare, typename Alloc>
[[nodiscard]] constexpr bool set<T, Compare, Alloc>::empty() const noexcept { 
    return sz_ == 0; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::size_type set<T, Compare, Alloc>::size() const noexcept { 
    return sz_; 
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::size_type set<T, Compare, Alloc>::max_size() const noexcept { 
    return std::numeric_limits<difference_type>::max();
}

template <typename T, typename Compare, typename Alloc>
constexpr typename set<T, Compare, Alloc>::allocator_type set<T, Compare, Alloc>::get_allocator() const noexcept { 
    return alloc; 
}

template <typename T, typename Compare, typename Alloc>
typename set<T, Compare, Alloc>::iterator set<T, Compare, Alloc>::find(const T& value) {
    Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
    return result == nullptr ? fakeNode_ : iterator(result);
}

template <typename T, typename Compare, typename Alloc>
typename set<T, Compare, Alloc>::const_iterator set<T, Compare, Alloc>::find(const T& value) const {
    Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
    return result == nullptr ? fakeNode_ : iterator(result);
}

template <typename T, typename Compare, typename Alloc>
bool set<T, Compare, Alloc>::contains(const T& value) const {
    Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
    return result != nullptr;
}

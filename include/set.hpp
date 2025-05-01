#pragma once
#include <iostream>
#include <initializer_list>
#include <memory>
#include <iterator>
#include <compare>
#include <limits>

#include "redblack.hpp"

template <typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
class set {
public:
    set();
    set(std::initializer_list<T> init, const Compare& comp = Compare(), const Alloc& alloc = Alloc());
    set(const set& other);
    set(set&& other) noexcept;
    ~set() noexcept;

    set& operator=(set other);
    constexpr void swap(set& other) noexcept;

    constexpr bool operator==(const set& other) const noexcept;
    constexpr std::weak_ordering operator<=>(const set& other) const;

private:
    template <bool IsConst>
    class base_iterator {
      public: 
        using pointer = std::conditional_t<IsConst, const Node<T>*, Node<T>*>;
        using reference = std::conditional_t<IsConst, const Node<T>&, Node<T>&>;
        using value_type = Node<T>;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        
      private:
        pointer ptr;

      public:
        base_iterator() : ptr(nullptr) {};
        base_iterator(pointer ptr) : ptr(ptr) {};
        base_iterator(reference node) : ptr(&node) {};
        base_iterator(BaseNode<Node<T>>* node) : ptr(nullptr) {};
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference operator*() const;
        pointer operator->() const;

        bool operator==(const base_iterator& iter) const;
        bool operator!=(const base_iterator& iter) const;
        constexpr base_iterator& operator++() noexcept;
        constexpr base_iterator operator++(int) noexcept;
        constexpr base_iterator& operator--() noexcept;
        constexpr base_iterator operator--(int) noexcept;
        friend std::ostream& operator<< (std::ostream& os, base_iterator iter) {
            os << iter->value;
            return os;
        }
        friend std::ostream& operator<< (std::ostream& os, const std::reverse_iterator<base_iterator>& riter) {
            os << riter.base();
            return os;
        }
    };
  public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = base_iterator<false>;
    using const_iterator =  base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using allocator_type = Alloc;
    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node<T>>;
    using BaseNodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<BaseNode<Node<T>>>;

    constexpr iterator begin() noexcept;
    constexpr iterator end() noexcept;
    constexpr const_iterator begin() const noexcept;
    constexpr const_iterator end() const noexcept;
    constexpr const_iterator cbegin() const noexcept;
    constexpr const_iterator cend() const noexcept;
    constexpr reverse_iterator rbegin() noexcept;
    constexpr reverse_iterator rend() noexcept;
    constexpr const_reverse_iterator rbegin() const noexcept;
    constexpr const_reverse_iterator rend() const noexcept;
    constexpr const_reverse_iterator crbegin() const noexcept;
    constexpr const_reverse_iterator crend() const noexcept;

    std::pair<iterator, bool> insert(const T& value);
    
    void insert(std::initializer_list<T> ilist);

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    template <typename... Args>
    iterator emplace_hint(iterator hint, Args&&... args);

    bool erase(const T& value);
    iterator erase(iterator iter);
    iterator erase(iterator first, iterator last);
    constexpr void clear() noexcept;

    [[nodiscard]] constexpr bool empty() const noexcept;
    constexpr size_type size() const noexcept;
    constexpr size_type max_size() const noexcept;
    constexpr Alloc get_allocator() const noexcept;

    iterator find(const T& value);
    const_iterator find(const T& value) const;
    bool contains(const T& value) const;

  private:
    Node<T>* beginNode_;
    BaseNode<Node<T>>* fakeNode_;
    size_t sz_;
    typename set<T, Compare, Alloc>::iterator iter;
    Alloc alloc;
    typename std::allocator_traits<Alloc>::template rebind_alloc<Node<T>> node_alloc;
    typename std::allocator_traits<Alloc>::template rebind_alloc<BaseNode<Node<T>>> basenode_alloc;
};

#include "set.tpp"
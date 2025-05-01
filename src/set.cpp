#include <iostream>
#include <limits>
#include <compare>

#include "..\include\redblack.hpp"

template <typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
class set {
  public:
    set() : beginNode_(nullptr), fakeNode_(nullptr), sz_(0), iter(beginNode_) {}
    
    set(std::initializer_list<T> init, const Compare& comp = Compare(), const Alloc& alloc = Alloc()) : set() {
        insert(init);
    }

    set(const set& other) {
        beginNode_ = allocateAndConstruct<NodeAlloc, Node<T>>(node_alloc);
        fakeNode_ = allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
        beginNode_->copy(other.fakeNode_->left, fakeNode_->left, other.fakeNode_->left, beginNode_, fakeNode_->right);
        fakeNode_->right = allocateAndConstruct<NodeAlloc, Node<T>>(node_alloc);
    }

    set(set&& other) noexcept {
        beginNode_ = other.beginNode_; other.beginNode_ = nullptr;
        fakeNode_ = other.fakeNode_; other.fakeNode_ = nullptr;
        sz_ = other.sz_; other.sz_ = 0;
        iter = other.iter; other.iter = {};
    }

    ~set() noexcept {
        if (!fakeNode_) return;
        fakeNode_->left->deleteTree(fakeNode_->left);
        destroyAndDeallocate(basenode_alloc, fakeNode_);
    }


    constexpr void swap(set& other) noexcept {
        std::swap(beginNode_, other.beginNode_);
        std::swap(fakeNode_, other.fakeNode_);
        std::swap(sz_, other.sz_);
        std::swap(node_alloc, other.node_alloc);
        std::swap(basenode_alloc, other.basenode_alloc);
    }
    
    set& operator=(set other) {
        swap(other);
        return *this;
    }

    constexpr bool operator== (const set& other) const noexcept {
        const_iterator iter1 = begin();
        const_iterator iter2 = other.begin();
        if (*iter1 != *iter2) return false; 
        return true;
    }

    constexpr std::weak_ordering operator<=> (const set& other) const {
        return std::lexicographical_compare_three_way(this.begin(), this.end(),
        other.begin(), other.end(), [](const Node<T>& a, const Node<T>& b) {return a <=> b; });
    }

    
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
        base_iterator(value_type node) : ptr(&node) {};
        base_iterator(BaseNode<Node<T>>* node) : ptr(nullptr) {};
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference operator*() const {return *ptr;}
        pointer operator->() const {return ptr;}

        bool operator==(const base_iterator& iter) const { return this->ptr == iter.ptr; }
        bool operator!=(const base_iterator& iter) const { return !(*this == iter);}

        constexpr base_iterator& operator++() noexcept {
            ptr = ptr->find_next(ptr->value, ptr);
            return *this;
        }
        
        constexpr base_iterator operator++(int) noexcept {
            pointer copy = ptr;
            ptr = ptr->find_next(ptr->value, ptr);
            return copy;
        }

        constexpr base_iterator& operator--() noexcept {
            ptr = ptr->find_prev(ptr->value, ptr);
            return *this;
        }

        constexpr base_iterator operator--(int) noexcept {
            pointer copy = ptr;
            ptr = ptr->find_prev(ptr->value, ptr);
            return copy;
        }

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


    constexpr iterator begin() noexcept { return beginNode_; }
    constexpr iterator end() noexcept {return fakeNode_ ; }

    constexpr const_iterator begin() const noexcept { return beginNode_; }
    constexpr const_iterator end() const noexcept { return fakeNode_; }

    constexpr const_iterator cbegin() const { return beginNode_; }
    constexpr const_iterator cend() const { return fakeNode_; }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(fakeNode_->right); }
    constexpr reverse_iterator rend() noexcept { return reverse_iterator(beginNode_); }

    constexpr const_reverse_iterator rbegin() const noexcept { return reverse_iterator(fakeNode_->right); }
    constexpr const_reverse_iterator rend() const noexcept { return reverse_iterator(beginNode_); }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(fakeNode_->right); }
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(beginNode_); }

    std::pair<iterator, bool> insert(const T& value) {
        if (fakeNode_ == nullptr) {
            fakeNode_ =  allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
        }
        Node<T>* node = fakeNode_->left->insert(value, fakeNode_->left, beginNode_, fakeNode_->right);
        if (node == nullptr) return {iterator(find(value)), false};
        else {
            ++sz_;
            return {iterator(node), true};
        }
    }

    void insert(std::initializer_list<T> ilist) {
        for (const T& obj : ilist) this->insert(obj);
    }
    
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        if (fakeNode_ == nullptr) {
            fakeNode_ = allocateAndConstruct<BaseNodeAlloc, BaseNode<Node<T>>>(basenode_alloc);
        }
        T value(std::forward<Args>(args)...);
        Node<T>* node = fakeNode_->left->insert(value, fakeNode_->left, beginNode_, fakeNode_->right);
        if (node == nullptr) return {iterator(this->find(value)), false};
        else {
            ++sz_;
            return {iterator(node), true};
        }
    }

    template <typename... Args>
    iterator emplace_hint(iterator hint, Args&&... args) {
        auto ptr = hint.operator->(); 
        return beginNode_->insert_hint(ptr, fakeNode_->left, beginNode_, fakeNode_->right, args...);
    }

    bool erase(const T& value) {
        --sz_;
        bool erased = fakeNode_->left->remove(value, fakeNode_->left, beginNode_, fakeNode_->right);
        return erased;
    }

    iterator erase(iterator iter) {
        T value = iter->value;
        fakeNode_->left->remove(value, fakeNode_->left, beginNode_, fakeNode_->right);
        --sz_;
        return iter->find_next(value, fakeNode_->left);
    }

    iterator erase(iterator first, iterator last) {
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

    constexpr void clear() noexcept {
        fakeNode_->left->deleteTree(fakeNode_->left);
        sz_ = 0;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return sz_ == 0; 
    }

    constexpr size_type size() const noexcept { return sz_; }
    constexpr size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max();}

    constexpr allocator_type get_allocator() const noexcept { return alloc; }

    iterator find(const T& value) {
        Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
        return result == nullptr ? fakeNode_ : iterator(result);
    }

    const_iterator find(const T& value) const {
        Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
        return result == nullptr ? fakeNode_ : iterator(result);
    }

    bool contains(const T& value) const {
        Node<T>* result = fakeNode_->left->find(fakeNode_->left, value);
        return result != nullptr;
    }

  private:
    Node<T>* beginNode_;
    BaseNode<Node<T>>* fakeNode_;
    size_t sz_;
    iterator iter;
    Alloc alloc;
    NodeAlloc node_alloc;
    BaseNodeAlloc basenode_alloc;
};

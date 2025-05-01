#define RED 0
#define BLACK 1

template<typename Alloc, typename T, typename... Args>
T* allocateAndConstruct(Alloc& alloc, Args&&... args) {
    T* ptr = std::allocator_traits<Alloc>::allocate(alloc, 1);
    std::allocator_traits<Alloc>::construct(alloc, ptr, args...);
    return ptr;
}

template<typename Alloc, typename T>
void destroyAndDeallocate(Alloc& alloc, T* ptr) {
    std::allocator_traits<Alloc>::destroy(alloc, ptr);
    std::allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
}

template <typename Node, typename Alloc = std::allocator<Node>>
struct BaseNode {
  private:
    uintptr_t parent_and_color;
  public:
    Node* left;
    Node* right;

    BaseNode() : left(nullptr), right(nullptr), parent_and_color(0) {}
    Node* get_parent() const {
        return reinterpret_cast<Node*>(parent_and_color & ~1);
    }
    void set_parent(Node* p) {
        uintptr_t color = parent_and_color & 1;
        parent_and_color = (reinterpret_cast<uintptr_t>(p) & ~1) | color;
    }
    bool get_color() const {
        return (parent_and_color & 1) != 0;
    }
    void set_color(bool color) {
        uintptr_t ptr = parent_and_color & ~1;
        parent_and_color = ptr | (color ? 1 : 0);
    }
};

template <typename T, typename Alloc = std::allocator<T>>
struct Node : public BaseNode<Node<T>> {
    T value;

    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node<T>>;;
    NodeAlloc node_alloc;

    Node() : value(INT_MAX), node_alloc(NodeAlloc()) {}
    Node(T value, const NodeAlloc& alloc = NodeAlloc()) : value(value), node_alloc(alloc) {}
    Node(Node*& other, const NodeAlloc& alloc = NodeAlloc()) : value(other.value), node_alloc(alloc) {
        Node* node = allocateAndConstruct<NodeAlloc, Node<T>>(other->value, node_alloc);
        this->left = node->left;
        this->right = node->right;
        this->parent = node->parent;
        this->color = node->color;
    }

    operator T() const {return value;}

    bool operator==(const Node<T>& other) const {
        return value == other.value;
    }

    bool operator!=(const Node<T>& other) const {
        return !(value == other.value);
    }

    std::weak_ordering operator<=> (const Node<T>& other) const {
        return value <=> other.value;
    }

    void rotateRight(Node*& node, Node*& root) {
        Node* child = node->left;
        node->left = child->right;
        if (node->left != nullptr)
            node->left->set_parent(node);
        child->set_parent(node->get_parent());
        if (node->get_parent() == nullptr)
            root = child;
        else if (node == node->get_parent()->left)
            node->get_parent()->left = child;
        else
            node->get_parent()->right = child;
        child->right = node;
        node->set_parent(child);
    }

    void rotateLeft(Node*& node, Node*& root) {
        Node* child = node->right;
        node->right = child->left;
        if (node->right != nullptr)
            node->right->set_parent(node);
        child->set_parent(node->get_parent());
        if (node->get_parent() == nullptr)
            root = child;
        else if (node == node->get_parent()->left)
            node->get_parent()->left = child;
        else
            node->get_parent()->right = child;
        child->left = node;
        node->set_parent(child);
    }

    void fixInsert(Node*& node, Node*& root, Node*& minNode, Node*& maxNode) {
        while (node != root && node->get_color() == RED && node->get_parent()->get_color() == RED) {
            Node* parent = node->get_parent();
            Node* grandparent = parent->get_parent();
            if (parent == grandparent->left) {
                Node* uncle = grandparent->right;
                if (uncle != nullptr && uncle->get_color() == RED) {
                    grandparent->set_color(RED);
                    parent->set_color(BLACK);
                    uncle->set_color(BLACK);
                    node = grandparent;
                } else {
                    if (node == parent->right) {
                        rotateLeft(parent, root);
                        node = parent;
                        parent = node->get_parent();
                    }
                    rotateRight(grandparent, root);
                    bool parent_color = parent->get_color();
                    parent->set_color(grandparent->get_color());
                    grandparent->set_color(parent_color);
                    node = parent;
                }
            } 
            else {
                Node* uncle = grandparent->left;
                if (uncle != nullptr && uncle->get_color() == RED) {
                    grandparent->set_color(RED);
                    parent->set_color(BLACK);
                    uncle->set_color(BLACK);
                    node = grandparent;
                } else {
                    if (node == parent->left) {
                        rotateRight(parent, root);
                        node = parent;
                        parent = node->get_parent();
                    }
                    rotateLeft(grandparent, root);
                    bool parent_color = parent->get_color();
                    parent->set_color(grandparent->get_color());
                    grandparent->set_color(parent_color);
                    node = parent;
                }
            }
        }
        root->set_color(BLACK);
    }

    void fixDelete(Node*& node, Node*& root) {
        while (node != root && node->get_color() == BLACK) {
            if (node == node->get_parent()->left) {
                Node* sibling = node->get_parent()->right;
                if (sibling->get_color() == RED) {
                    sibling->set_color(BLACK);
                    node->get_parent()->set_color(RED);
                    Node* parent = node->get_parent();
                    rotateLeft(parent, root);
                    node->set_parent(parent); 
                    sibling = node->get_parent()->right;
                }
                if ((sibling->left == nullptr || sibling->left->get_color() == BLACK) &&
                    (sibling->right == nullptr || sibling->right->get_color() == BLACK)) {
                    sibling->set_color(RED);
                    node = node->get_parent();
                } 
                else {
                    if (sibling->right == nullptr || sibling->right->get_color() == BLACK) {
                        if (sibling->left != nullptr)
                            sibling->left->set_color(BLACK);
                        sibling->set_color(RED);
                        rotateRight(sibling, root);
                        sibling = node->get_parent()->right;
                    }
                    sibling->set_color(node->get_parent()->get_color());
                    node->get_parent()->set_color(BLACK);
                    if (sibling->right != nullptr)
                        sibling->right->set_color(BLACK);
                    Node* parent = node->get_parent();
                    rotateLeft(parent, root);
                    node->set_parent(parent);
                    node = root;
                }
            } 
            else {
                Node* sibling = node->get_parent()->left;
                if (sibling->get_color() == RED) {
                    sibling->set_color(BLACK);
                    node->get_parent()->set_color(RED);
                    Node* parent = node->get_parent();
                    rotateRight(parent, root);
                    node->set_parent(parent);
                    sibling = node->get_parent()->left;
                }
                if ((sibling->left == nullptr || sibling->left->get_color() == BLACK) &&
                    (sibling->right == nullptr || sibling->right->get_color() == BLACK)) {
                    sibling->set_color(RED);
                    node = node->get_parent();
                } else {
                    if (sibling->left == nullptr || sibling->left->get_color() == BLACK) {
                        if (sibling->right != nullptr)
                            sibling->right->set_color(BLACK);
                        sibling->set_color(RED);
                        rotateLeft(sibling, root);
                        sibling = node->get_parent()->left;
                    }
                    sibling->set_color(node->get_parent()->get_color());
                    node->get_parent()->set_color(BLACK);
                    if (sibling->left != nullptr)
                        sibling->left->set_color(BLACK);
                    Node* parent = node->get_parent();
                    rotateRight(parent, root);
                    node->set_parent(parent);
                    node = root;
                }
            }
        }
        node->set_color(BLACK);
    }

    void transplant(Node*& root, Node*& u, Node*& v) {
        if (u->get_parent() == nullptr)
            root = v;
        else if (u == u->get_parent()->left)
            u->get_parent()->left = v;
        else
            u->get_parent()->right = v;
        if (v != nullptr)
            v->set_parent(u->get_parent());
    }

    void deleteTree(Node* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            destroyAndDeallocate<decltype(node_alloc), Node>(node_alloc, node);
        }
    }

    Node* insert(T key, Node*& root, Node*& minNode, Node*& maxNode) {
        Node* node = allocateAndConstruct<NodeAlloc, Node>(node_alloc, key);
        Node* parent = nullptr;
        Node* current = root;
        while (current != nullptr) {
            parent = current;
            if (node->value < current->value)
                current = current->left;
            else if (node->value > current->value)
                current = current->right;
            else {
                destroyAndDeallocate<decltype(node_alloc), Node>(node_alloc, node);
                node = nullptr; 
                break;
            }
        }
        if (node == nullptr) return node;
        node->set_parent(parent);
        if (parent == nullptr)
            root = node;
        else if (node->value < parent->value)
            parent->left = node;
        else
            parent->right = node;
        if (minNode == nullptr || key < minNode->value) minNode = node;
        if (maxNode == nullptr || key > maxNode->value) maxNode = node;
        fixInsert(node, root, minNode, maxNode);
        return node;
    }

    template <typename... Args>
    Node* insert(Node*& root, Node*& minNode, Node*& maxNode, Args&&... args) {
        Node* node = allocateAndConstruct<NodeAlloc, Node>(node_alloc, args...);
        Node* parent = nullptr;
        Node* current = root;
        while (current != nullptr) {
            parent = current;
            if (node->value < current->value)
                current = current->left;
            else if (node->value > current->value)
                current = current->right;
            else {
                destroyAndDeallocate<decltype(node_alloc), Node>(node_alloc, node);
                node = nullptr; 
                break;
            }
        }
        if (node == nullptr) return node;
        node->set_parent(parent);
        if (parent == nullptr)
            root = node;
        else if (node->value < parent->value)
            parent->left = node;
        else
            parent->right = node;
        fixInsert(node, root, minNode, maxNode);
        return node;
    }

    template <typename... Args>
    Node* insert_hint(Node*& hint_node, Node*& root, Node*& minNode, Node*& maxNode, Args&&... args) {
        Node* node = allocateAndConstruct<NodeAlloc, Node>(node_alloc, args...);
        if (find(root, node->value)) return find(root, node->value);
        if (hint_node->value > node->value && hint_node->left == nullptr) {
            node->set_parent(hint_node);
            hint_node->left = node;
            if (node->value < minNode->value) minNode = node;
            if (node->value > maxNode->value) maxNode = node;
        }
        else {
            destroyAndDeallocate(node_alloc, node);
            Node* node = insert(root, minNode, maxNode, args...);
        }
        fixInsert(node, root, minNode, maxNode);
        return node;
    }

    bool remove(T key, Node*& root, Node*& minNode, Node*& maxNode) {
        Node* node = root;
        Node* z = nullptr;
        Node* x = nullptr;
        Node* y = nullptr;
    
        while (node != nullptr) {
            if (node->value == key) {
                z = node;
            }
            if (node->value <= key) {
                node = node->right;
            } 
            else {
                node = node->left;
            }
        }
    
        if (z == nullptr) return false;
    
        y = z;
        bool yOriginalColor = y->get_color();
        if (z->left == nullptr) {
            x = z->right;
            transplant(root, z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            transplant(root, z, z->left);
        } else {
            Node* next = z->right;
            while (next->left != nullptr) next = next->left;
            y = next;
            yOriginalColor = y->get_color();
            x = y->right;
            if (y->get_parent() == z) {
                if (x != nullptr)
                    x->set_parent(y);
            } else {
                transplant(root, y, y->right);
                y->right = z->right;
                y->right->set_parent(y);
            }
            transplant(root, z, y);
            y->left = z->left;
            y->left->set_parent(y);
            y->set_color(z->get_color());
        }
    
        destroyAndDeallocate<decltype(node_alloc), Node>(node_alloc, z);
    
        if (yOriginalColor == BLACK && x != nullptr) {
            fixDelete(x, root);
        }

        Node* temp = root;
        while (temp && temp->left) temp = temp->left;
        minNode = temp;

        temp = root;
        while (temp && temp->right) temp = temp->right;
        maxNode = temp;
    
        return true;
    }

    Node* find_next(const T& key, const Node* root) const {
        if (root == nullptr) return nullptr;
        while (root->get_parent() != nullptr) root = root->get_parent();
        Node* successor = nullptr;
        while (root != nullptr) {
            if (root->value > key) {
                successor = const_cast<Node*>(root);
                root = root->left;
            }
            else root = root->right;
        }
        return successor;
    }

    Node* find_prev(const T& key, Node* root) const {
        if (root == nullptr) return nullptr;
        Node* successor = root->right;
        if (root->get_parent() == nullptr) root = root->left;
        while (root->get_parent() != nullptr) root = root->get_parent();
        while (root != nullptr) {
            if (root->value < key) {
                successor = root;
                root = root->right;
            }
            else {
                root = root->left;
            }
        }
        return successor;
    }

    Node* find(Node* root, T key) {
        if (root != nullptr) {
            if (root->value < key) return find(root->right, key);
            if (root->value > key) return find(root->left, key);
            if (root->value == key) return root;
        }
        return nullptr;
    }

    void copy(Node* root, Node*& other_root, Node* tree_root, Node*& minNode, Node*& maxNode) {
        if (root != nullptr) {
            other_root->insert(root->value, other_root, minNode, maxNode);
            copy(root->left, other_root, tree_root, minNode, maxNode);
            copy(root->right, other_root, tree_root, minNode, maxNode);
        }
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Node<T>& node) {
    os << node.value;
    return os;
}

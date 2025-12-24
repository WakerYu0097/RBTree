#include <iostream>
#include <cstdint>
#include <random>
#include <vector>
#include <algorithm>

/// @brief RBTree
/// @tparam K : must have operator> , operator< , operator==
/// @tparam V : Any
template <typename K, typename V>
class RBTree
{
private:
    enum Color : std::uint8_t
    {
        Red,
        Black,
        NColor
    };

    struct TreeNode
    {
        K key;
        V data;
        TreeNode *left;
        TreeNode *right;
        TreeNode *parent;
        Color color;

        TreeNode(K key, V value, Color col)
            : key(key), data(value), left(nullptr), right(nullptr), parent(nullptr), color(col)
        {
        }
        TreeNode(K key, V value)
            : key(key), data(value), left(nullptr), right(nullptr), parent(nullptr), color(NColor)
        {
        }
        TreeNode()
            : key(K()), data(V()), left(nullptr), right(nullptr), parent(nullptr), color(NColor)
        {
        }
        TreeNode(const TreeNode &) = delete;
        TreeNode &operator=(const TreeNode &) = delete;
        ~TreeNode()
        {
        }
    };

    // the main root of the tree
    TreeNode *root;

    void _Insert(TreeNode *&node, const K &key, const V &value)
    {
        if (node == nullptr)
        {
            node = new TreeNode(key, value, Black);
            return;
        }
        TreeNode *parent = nullptr;
        TreeNode *current = node;

        // let current=nullptr,parent=current->parent
        while (current) // current!=nullptr
        {
            if (key > current->key)
            {
                parent = current;
                current = current->right;
            }
            else if (key < current->key)
            {
                parent = current;
                current = current->left;
            }
            else // equal
            {
                current->data = value;
                return;
            }
        }

        current = new TreeNode(key, value, Red);
        if (key > parent->key)
        {
            parent->right = current;
            current->parent = parent;
        }
        else
        {
            parent->left = current;
            current->parent = parent;
        }

        // (current->color==Red && parent->color==Red) against rule
        // when parent is nullptr,current is root
        while (parent && parent->color == Red)
        {
            TreeNode *grandparent = parent->parent;
            // L
            if (parent == grandparent->left)
            {
                TreeNode *uncle = grandparent->right;

                // Situation 1
                // uncle!=nullptr&&uncle->color==Red
                if (uncle && uncle->color == Red)
                {
                    parent->color = uncle->color = Black;
                    grandparent->color = Red;

                    current = grandparent;
                    parent = current->parent;
                }
                // now uncle is nullptr or Black

                // Situation 2
                // LL
                else if (current == parent->left)
                {
                    Rotate_R(grandparent);
                    parent->color = Black;
                    grandparent->color = Red;
                    break;
                }
                // Situation 3
                // LR
                else
                {
                    Rotate_L(parent);
                    Rotate_R(grandparent);
                    current->color = Black;
                    grandparent->color = Red;
                    break;
                }
            }
            // R
            else
            {
                TreeNode *uncle = grandparent->left;

                // Situation 1
                // uncle!=nullptr&&uncle->color==Red
                if (uncle && uncle->color == Red)
                {
                    parent->color = uncle->color = Black;
                    grandparent->color = Red;

                    current = grandparent;
                    parent = current->parent;
                }
                // now uncle is nullptr or Black

                // Situation 2
                // RR
                else if (current == parent->right)
                {
                    Rotate_L(grandparent);
                    parent->color = Black;
                    grandparent->color = Red;
                    break;
                }
                // Situation 3
                // RL
                else
                {
                    Rotate_R(parent);
                    Rotate_L(grandparent);
                    current->color = Black;
                    grandparent->color = Red;
                    break;
                }
            }
        }
        node->color = Black;
    }

    // node: from where to delete
    void _Delete(TreeNode *&node, const K &key)
    {
        if (node == nullptr)
        {
            throw std::runtime_error("root is empty");
        }

        // from node
        TreeNode *current = node;
        while (current)
        {
            if (key > current->key)
            {
                current = current->right;
                if (current == nullptr)
                {
                    throw std::runtime_error(
                        "Key " + std::to_string(key) + " Not Found For Deletion");
                }
            }
            else if (key < current->key)
            {
                current = current->left;
                if (current == nullptr)
                {
                    throw std::runtime_error(
                        "Key " + std::to_string(key) + " Not Found For Deletion");
                }
            }
            else // equal
            {
                break;
            }
        }

        // now current is to delete

        // Situation 1: red nil-> delete
        if (current->color == Red &&
            current->left == nullptr && current->right == nullptr)
        {
            _DeleteRed0Child(current);
            return;
        }

        // Situation 2: red with 2 child
        // -> transform into delete (red with 0 child)or(black with 1 child(must be red))or(black with 0 child)
        if (current->color == Red &&
            current->left != nullptr && current->right != nullptr)
        {
            TreeNode *minRight = current->right;
            while (minRight->left)
            {
                minRight = minRight->left;
            }
            current->key = std::move(minRight->key);
            current->data = std::move(minRight->data);
            _Delete01Child(minRight);
            return;
        }

        // Situation 3:red with 0 child
        if (current->color == Black &&
            current->left == nullptr && current->right == nullptr)
        {
            _DeleteBlack0Child(current);
            return;
        }

        // Situation 4: red with 1 child(must be red)
        if (current->color == Black &&
            ((current->left == nullptr) != (current->right == nullptr)))
        {
            _DeleteBlack1Child(current);
            return;
        }

        // Situation 4:black with 2 child
        // -> transform into delete (red with 0 child)or(black with 1 child(must be red))or(black with 0 child)
        if (current->color == Black &&
            current->left != nullptr && current->right != nullptr)
        {
            TreeNode *minRight = current->right;
            while (minRight->left)
            {
                minRight = minRight->left;
            }
            current->key = std::move(minRight->key);
            current->data = std::move(minRight->data);
            _Delete01Child(minRight);
            return;
        }
    }

    // node that has 0-1 child
    void _Delete01Child(TreeNode *&current)
    {
        if (current->left == nullptr && current->right == nullptr)
        {
            if (current->color == Red)
            {
                _DeleteRed0Child(current);
                return;
            }
            else if (current->color == Black)
            {
                _DeleteBlack0Child(current);
                return;
            }
        }
        _DeleteBlack1Child(current);
    }

    // red node with 0 child
    void _DeleteRed0Child(TreeNode *&current)
    {
        TreeNode *parent = current->parent;
        // red must not be root,has parent
        if (current == parent->left)
        {
            parent->left = nullptr;
        }
        else
        {
            parent->right = nullptr;
        }
        delete current;
        current = nullptr;
    }

    // black node with 1 child
    void _DeleteBlack1Child(TreeNode *&current)
    {
        // current has left
        if (current->left != nullptr)
        {
            TreeNode *&child = current->left;
            TreeNode *&parent = current->parent;

            child->parent = parent;
            child->color = Black;
            // 要看current是parent的left还是right
            if (current == parent->left)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        // current has right
        else
        {
            TreeNode *&child = current->right;
            TreeNode *&parent = current->parent;

            child->parent = parent;
            child->color = Black;
            if (current == parent->left)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        delete current;
        current = nullptr;
    }

    // black with 0 child
    void _DeleteBlack0Child(TreeNode *&current)
    {
        if (current == root)
        {
            delete current;
            root = nullptr;
            return;
        }
        TreeNode *parent = current->parent;
        bool isLeft = (current == parent->left);

        if (isLeft)
        {
            parent->left = nullptr;
        }
        else
        {
            parent->right = nullptr;
        }
        delete current;
        current = nullptr;

        // now parent ->isLeftNode has 2-black
        _DeleteFixUp(parent, isLeft);
    }

    // parent ->isLeftNode has 2-black
    void _DeleteFixUp(TreeNode *parent, bool isLeft)
    {
        TreeNode *brother = isLeft ? parent->right : parent->left;
        // brother is black
        if (brother->color == Black)
        {
            // has at least one red-child
            if ((brother->left != nullptr && brother->left->color == Red) ||
                (brother->right != nullptr && brother->right->color == Red))
            {
                // Situation 1:LL
                if ((!isLeft) && (brother->left != nullptr && brother->left->color == Red))
                {
                    brother->left->color = brother->color;
                    brother->color = parent->color;
                    parent->color = Black;
                    Rotate_R(parent);
                    return;
                }
                // Situation 2:RR
                else if (isLeft &&
                         (brother->right != nullptr && brother->right->color == Red))
                {
                    brother->right->color = brother->color;
                    brother->color = parent->color;
                    parent->color = Black;
                    Rotate_L(parent);
                    return;
                }
                // Situation 3:LR
                else if ((!isLeft) &&
                         (brother->left == nullptr || brother->left->color == Black))
                {
                    brother->right->color = parent->color;
                    parent->color = Black;
                    Rotate_L(brother);
                    Rotate_R(parent);
                    return;
                }
                // Situation 4:RL
                else
                {
                    brother->left->color = parent->color;
                    parent->color = Black;
                    Rotate_R(brother);
                    Rotate_L(parent);
                    return;
                }
            }
            // brother with 0 child
            else
            {
                brother->color = Red;
                // 2-black up

                // meet red-node -> red trans into black , over
                if (parent->color == Red)
                {
                    parent->color = Black;
                    return;
                }
                else
                {
                    // meet root,over
                    if (parent == root)
                    {
                        return;
                    }
                    // 2-black up
                    _DeleteFixUp(parent->parent, parent == parent->parent->left);
                    return;
                }
            }
        }
        // brother is red
        else
        {
            brother->color = Black;
            parent->color = Red;
            // 2-black is left
            if (isLeft)
            {
                Rotate_L(parent);
                _DeleteFixUp(parent, true);
            }
            // 2-black is right
            else
            {
                Rotate_R(parent);
                _DeleteFixUp(parent, false);
            }
        }
    }

    void _Print(TreeNode *root, bool isLeft, std::string prefix) const
    {
        if (root == nullptr)
        {
            return;
        }

        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──");
        std::cout << root->key;
        switch (root->color)
        {
        case Black:
            std::cout << "(Black)" << std::endl;
            break;
        case Red:
            std::cout << "(Red)" << std::endl;
            break;
        default:
            std::cout << "(None)" << std::endl;
        }

        _Print(root->left, true, prefix + (isLeft ? "│   " : "    "));
        _Print(root->right, false, prefix + (isLeft ? "│   " : "    "));
    }

    void _PrintWithData(TreeNode *root, bool isLeft, std::string prefix) const
    {
        if (root == nullptr)
        {
            return;
        }

        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──");
        std::cout << root->key;
        switch (root->color)
        {
        case Black:
            std::cout << "(Black)";
            break;
        case Red:
            std::cout << "(Red)";
            break;
        default:
            std::cout << "(None)";
        }
        std::cout << " [" << root->data << "]" << std::endl;

        _PrintWithData(root->left, true, prefix + (isLeft ? "│   " : "    "));
        _PrintWithData(root->right, false, prefix + (isLeft ? "│   " : "    "));
    }

    void _Destroy(TreeNode *node)
    {
        if (node == nullptr)
        {
            return;
        }
        _Destroy(node->left);
        _Destroy(node->right);
        delete node;
        node = nullptr;
    }

    void _Clear()
    {
        _Destroy(root);
    }

    void Rotate_L(TreeNode *node)
    {
        TreeNode *r = node->right;
        TreeNode *rl = r->left;

        node->right = rl;
        if (rl)
        {
            rl->parent = node;
        }

        TreeNode *parent = node->parent;

        r->left = node;
        node->parent = r;

        // node is root
        if (parent == nullptr)
        {
            root = r;
            root->parent = nullptr;
        }
        else
        {
            if (parent->left == node)
            {
                parent->left = r;
            }
            else
            {
                parent->right = r;
            }
            r->parent = parent;
        }
    }

    void Rotate_R(TreeNode *node)
    {
        TreeNode *l = node->left;
        TreeNode *lr = l->right;

        node->left = lr;
        if (lr)
        {
            lr->parent = node;
        }

        TreeNode *parent = node->parent;

        l->right = node;
        node->parent = l;

        // node is root
        if (parent == nullptr)
        {
            root = l;
            root->parent = nullptr;
        }
        else
        {
            if (parent->left == node)
            {
                parent->left = l;
            }
            else
            {
                parent->right = l;
            }
            l->parent = parent;
        }
    }

public:
    RBTree() : root(nullptr)
    {
    }

    ~RBTree()
    {
        _Clear();
    }

    RBTree(const RBTree &) = delete;
    RBTree &operator=(const RBTree &) = delete;
    RBTree(RBTree &&other) noexcept : root(other.root)
    {
        other.root = nullptr;
    }
    RBTree &operator=(RBTree &&other) noexcept
    {
        if (this != &other)
        {
            Clear();
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    /// @brief is empty
    bool Empty()
    {
        return root == nullptr;
    }

    /// @brief insert key and value.when key exists,update value
    void Insert(const K &key, const V &value)
    {
        _Insert(root, key, value);
    }

    /// @brief delete,when key can't find,pass
    void Delete(const K &key)
    {
        try
        {
            _Delete(root, key);
        }
        catch (const std::exception &e)
        {
            // std::cerr << e.what() << '\n';
        }
    }

    /// @brief get value on key
    /// @exception runtime_error : can't find key
    const V &Get(const K &key) const
    {
        TreeNode *current = root;
        while (current != nullptr)
        {
            if (key > current->key)
            {
                current = current->right;
            }
            else if (key < current->key)
            {
                current = current->left;
            }
            else
            {
                return current->data;
            }
        }
        throw std::runtime_error("Key " + std::to_string(key) + " Not Found");
    }

    /// @brief whether key exists?
    bool Contain(const K &key) const
    {
        TreeNode *current = root;
        while (current != nullptr)
        {
            if (key > current->key)
            {
                current = current->right;
            }
            else if (key < current->key)
            {
                current = current->left;
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    /// @brief update value on key
    /// @exception runtime_error : can't find Key
    void Update(const K &key, const V &value)
    {
        TreeNode *current = root;
        while (current != nullptr)
        {
            if (key > current->key)
            {
                current = current->right;
            }
            else if (key < current->key)
            {
                current = current->left;
            }
            else
            {
                current->data = value;
                return;
            }
        }
        throw std::runtime_error(
            "Key " + std::to_string(key) + " Not Found For Update");
    }

    /// @brief delete the whole tree
    void Clear()
    {
        _Clear();
    }

    /// @param displayData show data? if true,Make sure value can be output
    void Print(bool displayData = false) const
    {
        if (root == nullptr)
        {
            std::cout << "Tree is empty" << std::endl;
            return;
        }
        std::cout << "Tree Structure:" << std::endl;
        if (displayData)
        {
            _PrintWithData(root, false, "");
        }
        else
        {
            _Print(root, false, "");
        }
    }
};

/// @brief SetTree
/// @tparam K : must have operator> , operator< , operator==
template <typename K>
class SetTree
{
private:
    RBTree<K, int> tree;

public:
    SetTree() = default;

    ~SetTree()
    {
        Clear();
    }

    SetTree(const SetTree &) = delete;
    SetTree &operator=(const SetTree &) = delete;
    SetTree(SetTree &&other) noexcept : tree(std::move(other.tree))
    {
    }
    SetTree &operator=(SetTree &&other) noexcept
    {
        if (this != &other)
        {
            Clear();
            tree = std::move(other.tree);
        }
        return *this;
    }

    /// @brief insert the key
    void Insert(const K &key)
    {
        try
        {
            int count = tree.Get(key);
            tree.Update(key, count + 1);
        }
        catch (const std::exception &e)
        {
            tree.Insert(key, 1);
        }
    }

    /// @brief delete the key,if can't find,pass
    void Delete(const K &key)
    {
        try
        {
            int count = tree.Get(key);
            if (count > 1)
            {
                tree.Update(key, count - 1);
            }
            else
            {
                tree.Delete(key);
            }
        }
        catch (const std::exception &e)
        {
            // std::cerr << e.what() << '\n';
        }
    }

    /// @brief whether key exists?
    bool Contain(const K &key) const
    {
        try
        {
            int count = tree.Get(key);
            return count > 0;
        }
        catch (const std::exception &e)
        {
            return false;
        }
    }

    /// @brief get the key Count. 0 if not exist
    int GetCount(const K &key) const
    {
        try
        {
            return tree.Get(key);
        }
        catch (const std::runtime_error &e)
        {
            return 0;
        }
    }

    /// @brief delete the whole key
    void RemoveAll(const K &key)
    {
        tree.Delete(key);
    }

    /// @brief delete the whole tree
    void Clear()
    {
        tree.Clear();
    }

    /// @brief is the tree empty?
    bool Empty()
    {
        return tree.Empty();
    }

    void Print()
    {
        tree.Print(true);
    }
};


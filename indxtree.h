#include <compare>
#include <memory>

template<class Key>
class indxtree {
private:

    template<class T>
    struct node {
        T key;
        node *left;
        node *right;
        node *parent;
        size_t size;
    };

    using node_t = node<Key>;

    node_t *root;


public:

    explicit indxtree() :root(nullptr){};

    void insert(const Key &key) {

        //проверяем есть ли узел с таким ключом
        if (search(key))
            return;

        auto *new_node = new node_t;
        new_node->key = key;
        new_node->right = new_node->left = new_node->parent = nullptr;
        new_node->size = 1;

        if (!root) {
            root = new_node;
            return;
        }

        node_t *prev = nullptr;
        auto *temp = root;

        while (temp) {
            temp->size += 1;
            prev = temp;
            if (temp->key > key)
                temp = temp->left;
            else
                temp = temp->right;
        }

        new_node->parent = prev;

        if (prev->key > key)
            prev->left = new_node;
        else
            prev->right = new_node;
    }

    const Key &getKey(size_t number) {
        auto node = getNode(number);
        if (!node)
            return Key();
        return node->key;
    }

    void erase(size_t number) {

        auto node = getNode(number);

        //лист
        if (!node->left && !node->right) {
            decrease_size(node);
            if (!node->parent)
                root = nullptr;
            else {
                if (node->parent->key > node->key)
                    node->parent->left = nullptr;
                else
                    node->parent->right = nullptr;
            }
            delete node;
            return;
        }

        node_t *tmp;


        //ищем замену справа - минимальный элемент справа
        if (node->right) {
            tmp = node->right;
            //если нет левого потомка
            if (!tmp->left) {
                node->right = tmp->right;
            } else {
                while (tmp->left) tmp = tmp->left;
                tmp->parent->left = tmp->right;
            }
            if (tmp->right)
                tmp->right->parent = tmp->parent;
        //ищем замену слева - максимальный элемент слева
        } else {
            tmp = node->left;
            if (!tmp->right) {
                node->left = tmp->left;
            } else {
                while (tmp->right) tmp = tmp->right;
                tmp->parent->right = tmp->left;
            }
            if (tmp->left)
                tmp->left->parent = tmp->parent;
        }
        decrease_size(tmp);
        std::swap(node->key, tmp->key);
        delete tmp;
    }


    ~indxtree(){
        recursive_delete(root);
    }

private:
    void decrease_size(node<Key> *_node) {
        auto *tmp = _node->parent;
        while (tmp) {
            tmp->size -= 1;
            tmp = tmp->parent;
        }
    }

    void recursive_delete(node<Key>* node){
        if(!node)
            return;
        recursive_delete(node->left);
        recursive_delete(node->right);
        delete node;
    }

    node<Key> *getNode(size_t number) {

        if (!root)
            return nullptr;

        if (root->size <= number)
            return nullptr;

        auto *tmp = root;

        while (tmp) {
            //если слева есть потомок и порядковый номер равен количеству элементов в левом поддереве
            // или слева нет элементов и порядковый номер равен 0, тогда возвращаем корень
            if (tmp->left && tmp->left->size == number || !tmp->left && number == 0)
                return tmp;

            //если порядковый номер меньше количества элементов в левом поддереве, то идем влево

            //иначе идем вправо вычитая из порядкового номера количество элементов в левом поддереве + 1(учитываем
            //текущий узел)

            size_t left_size = tmp->left ? tmp->left->size : 0;

            if (number < left_size)
                tmp = tmp->left;

            else {
                number -= (left_size + 1);
                tmp = tmp->right;
            }
        }
    }

    bool search(const Key &key) {
        if (!root)
            return false;

        auto *tmp = root;

        while (tmp) {
            if (tmp->key == key)
                return true;
            else if (tmp->key > key)
                tmp = tmp->left;
            else
                tmp = tmp->right;
        }
        return false;
    }
};

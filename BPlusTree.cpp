//
// Created by 刘洋 on 2021/10/17.
//

#include "BPlusTree.h"
#include "stack"
#include "queue"

int Node::_binarySearch(int item) {
    int end = pointer;
    int start = 0;
    while (start <= end) {
        int mid = (end + start) / 2;
        if (item <= key[mid])
            end = mid - 1;
        else if (item > key[mid])
            start = mid + 1;
        else
            return mid;
    }
    return start;
}

bool Node::isFull() const {
    if (pointer == B_PLUS_TREE_ORDER - 2)
        return true;
    return false;
}

bool Node::isDeletable() const {
    if (pointer > (B_PLUS_TREE_ORDER - 1) / 2 - 1)
        return true;
    return false;
}

int Node::getPointer() const {
    return pointer;
}

int Node::getKey(int index) const {
    if (index < 0 or index > pointer) {
        return -1;
    }
    return key[index];
}

bool Node::setKey(int pos, int item) {
    if (pos < 0 or pos > pointer) {
        cout << "Node::setKey error" << endl;
        return false;
    }
    key[pos] = item;
    return true;
}

int Node::getInsertPosition(int item) {
    int pos = _binarySearch(item);
    while (key[pos] == item and pos <= pointer)
        pos++;
    return pos;
}

int Node::getSearchPosition(int item) {
    int pos = _binarySearch(item);
    if (key[pos] != item or pos > pointer)
        return -1;
    return pos;
}

int Node::getRemovePosition(int item) {
    int pos = getInsertPosition(item);
    if (pos != 0)
        pos--;
    return pos;
}

void Node::print() {
    cout << "| ";
    if (pointer == -1)
        return;
    for (int i = 0; i <= pointer; ++i) {
        cout << key[i] << " ";
    }
    cout << "|  ";
}

IndexNode::IndexNode(int k, IndexNode *node1, IndexNode *node2) {
    pointer = 0;
    key[0] = k;
    sonIndexNode[0] = node1;
    sonIndexNode[1] = node2;
}

IndexNode *IndexNode::divide(bool insert_left) {
    if (!isFull()) {
        cout << "index_array node not full, divide error" << endl;
        return nullptr;
    }
    pointer = (B_PLUS_TREE_ORDER - 1) / 2 - 1;
    if (insert_left)
        pointer--;

    auto *newNode = new IndexNode;
    for (int i = pointer + 2, j = 0; i < B_PLUS_TREE_ORDER - 1; ++i, ++j) {
        newNode->key[j] = key[i];
        newNode->sonIndexNode[j] = sonIndexNode[i];
        newNode->leafNode[j] = leafNode[i];
        newNode->pointer = j;

        sonIndexNode[i] = nullptr;
        leafNode[i] = nullptr;
    }
    newNode->sonIndexNode[newNode->pointer + 1] = sonIndexNode[B_PLUS_TREE_ORDER - 1];
    newNode->leafNode[newNode->pointer + 1] = leafNode[B_PLUS_TREE_ORDER - 1];

    sonIndexNode[B_PLUS_TREE_ORDER - 1] = nullptr;
    leafNode[B_PLUS_TREE_ORDER - 1] = nullptr;

    newNode->toTopNodeKey = key[pointer + 1];
    newNode->has_leaf = has_leaf;
    newNode->leftIndexNode = this;
    return newNode;
}

IndexNode *IndexNode::insert(int item, IndexNode *indexNode1, LeafNode *leafNode1) {
    int insert_index = getInsertPosition(item);
    if (leafNode1 != nullptr)
        has_leaf = true;
    if (!isFull()) {
        for (int i = pointer; i >= insert_index; --i) {
            key[i + 1] = key[i];
            leafNode[i + 2] = leafNode[i + 1];
            sonIndexNode[i + 2] = sonIndexNode[i + 1];
        }
        key[insert_index] = item;
        if (pointer == -1) {
            // 初始化的节点
            if (has_leaf) {
                leafNode[1] = leafNode1;
                leafNode[0] = leafNode1->frontLeaf();
            } else {
                sonIndexNode[1] = indexNode1;
                sonIndexNode[0] = indexNode1->leftIndexNode;
            }
        } else {
            leafNode[insert_index + 1] = leafNode1;
            sonIndexNode[insert_index + 1] = indexNode1;
        }
        pointer++;
        return nullptr;
    } else {
        IndexNode *newNode;
        if (insert_index <= (B_PLUS_TREE_ORDER - 1) / 2 - 1) {
            newNode = divide(true);
            if (newNode == nullptr or insert(item, indexNode1, leafNode1) != nullptr)
                cout << "insert left error" << endl;
        } else {
            newNode = divide();
            if (newNode == nullptr or newNode->insert(item, indexNode1, leafNode1) != nullptr)
                cout << "insert right error" << endl;
        }
        return newNode;
    }
}

IndexNode *IndexNode::getLeftSonNode(int index) {
    if (index < 0 or index > pointer + 1) {
        cout << "IndexNode::getLeftSonNode outside range" << endl;
        return nullptr;
    }
    return sonIndexNode[index];
}

LeafNode *IndexNode::getLeftSonLeaf(int index) {
    if (index < 0 or index > pointer + 1) {
        cout << "IndexNode::getLeftSonLeaf outside range" << endl;
        return nullptr;
    }
    return leafNode[index];
}

IndexNode *IndexNode::getRightSonNode(int index) {
    if (index < 0 or index > pointer + 1) {
        cout << "IndexNode::getRightSonNode outside range" << endl;
        return nullptr;
    }
    return sonIndexNode[index + 1];
}


int IndexNode::getDivideToFatherKey() const {
    return toTopNodeKey;
}

bool IndexNode::hasLeaf() const {
    return has_leaf;
}

LeafNode *IndexNode::getRightSonLeaf(int index) {
    if (index < 0 or index > pointer + 1) {
        cout << "IndexNode::getRightSonLeaf outside range" << endl;
        return nullptr;
    }
    return leafNode[index + 1];
}

bool IndexNode::_delete(int pos) {
    if (pos < 0 or pos > pointer) {
        cout << "IndexNode::_delete error" << endl;
        return false;
    }
    for (int i = pos; i < pointer; ++i) {
        key[i] = key[i + 1];
        sonIndexNode[i + 1] = sonIndexNode[i + 2];
        if (has_leaf)
            leafNode[i + 1] = leafNode[i + 2];
    }
    pointer--;
    return true;
}

IndexNode *IndexNode::remove(int item, IndexNode *fatherNode) {
    int pos = getRemovePosition(item);

    if (isDeletable() or fatherNode == nullptr) {
        delete_status = DELETE_SUCCESS;
        _delete(pos);
        return this;
    }

    int father_pos = fatherNode->getRemovePosition(item);

    // 若兄弟结点有富余，父结点key下移，兄弟结点key上移, 返回相对右节点
    IndexNode *left = fatherNode->getLeftSonNode(father_pos);
    IndexNode *right = fatherNode->getRightSonNode(father_pos);


    if (left->isDeletable()) {
        _delete(pos);
        delete_status = BORROW_FROM_BROTHER;
        int leftKey = left->key[left->pointer];
        IndexNode *leftSonIndex = left->sonIndexNode[left->pointer + 1];
        LeafNode *leftSonLeaf = left->leafNode[left->pointer + 1];
        left->pointer--;

        for (int i = right->pointer; i >= 0; --i) {
            right->key[i + 1] = right->key[i];
            right->sonIndexNode[i + 2] = right->sonIndexNode[i + 1];

            right->leafNode[i + 2] = right->leafNode[i + 1];
        }
        right->key[0] = fatherNode->key[father_pos];
        right->sonIndexNode[0] = leftSonIndex;
        right->leafNode[0] = leftSonLeaf;
        fatherNode->key[father_pos] = leftKey;
        return right;
    }

    if (right->isDeletable()) {
        _delete(pos);
        delete_status = BORROW_FROM_BROTHER;
        int rightKey = right->key[0];
        IndexNode *rightSonIndex = right->sonIndexNode[0];
        LeafNode *rightSonLeaf = right->leafNode[0];
        right->sonIndexNode[0] = right->sonIndexNode[1];
        right->leafNode[0] = right->leafNode[1];
        right->_delete(0);

        left->pointer++;
        left->key[left->pointer] = fatherNode->key[father_pos];
        left->sonIndexNode[left->pointer + 1] = rightSonIndex;
        right->leafNode[left->pointer + 1] = rightSonLeaf;
        fatherNode->key[father_pos] = rightKey;
        return left;
    }



    // 节点的合并，父节点的key下移到子节点
    if (!left->isDeletable() or !right->isDeletable()) {
        _delete(pos);
        delete_status = MERGE_WITH_BROTHER;
        left->pointer++;
        left->key[left->pointer] = fatherNode->getKey(father_pos);
        left->sonIndexNode[left->pointer + 1] = right->sonIndexNode[0];
        left->leafNode[left->pointer + 1] = right->leafNode[0];

        for (int i = 0; i <= right->pointer; ++i) {
            left->pointer++;
            left->key[left->pointer] = right->key[i];
            left->sonIndexNode[left->pointer + 1] = right->sonIndexNode[i + 1];
            left->leafNode[left->pointer + 1] = right->leafNode[i + 1];
        }
        return left;
    }

    cout << "Delete Failed" << endl;
    return nullptr;
}

LeafNode *LeafNode::divide(bool insert_left) {
    if (!isFull()) {
        cout << "leaf node not full, divide error" << endl;
        return nullptr;
    }
    pointer = (B_PLUS_TREE_ORDER - 1) / 2 - 1;
    if (insert_left)
        pointer--;

    auto *leafNode = new LeafNode;
    for (int i = pointer + 1, j = 0; i < B_PLUS_TREE_ORDER - 1; ++i, ++j) {
        leafNode->key[j] = key[i];
        leafNode->column[j] = column[i];
        leafNode->pointer = j;
    }
    if (rear != nullptr) {
        rear->front = leafNode;
        leafNode->rear = rear;
    }
    leafNode->front = this;
    rear = leafNode;
    return leafNode;
}


LeafNode *LeafNode::insert(int item, int col) {
    if (col < 0) {
        cout << "column cannot smaller than 0" << endl;
        return nullptr;
    }
    int insert_index = getInsertPosition(item);

    if (!isFull()) {
        for (int i = pointer; i >= insert_index; --i) {
            key[i + 1] = key[i];
            column[i + 1] = column[i];
        }
        key[insert_index] = item;
        column[insert_index] = col;
        pointer++;
        return nullptr;
    } else {
        LeafNode *leafNode;
        if (insert_index <= (B_PLUS_TREE_ORDER - 1) / 2 - 1) {
            leafNode = divide(true);
            if (leafNode == nullptr or insert(item, col) != nullptr)
                cout << "insert left error" << endl;
        } else {
            leafNode = divide();
            if (leafNode == nullptr or leafNode->insert(item, col) != nullptr)
                cout << "insert right error" << endl;;
        }
        return leafNode;
    }
}

void LeafNode::_delete(int pos) {
    for (int i = pos; i < pointer; ++i) {
        key[i] = key[i + 1];
        column[i] = column[i + 1];
    }
    pointer--;
}

// 根据 delete_status 判断删除的状态
bool LeafNode::remove(int item, IndexNode *father) {
    delete_status = DEFAULT;
    if (getSearchPosition(item) == -1) {
        cout << "key Not Exits, Remove Error" << endl;
        return false;
    }
    int pos = getSearchPosition(item);

    if (father == nullptr) {
        _delete(pos);
        return true;
    }

    int father_pos = father->getRemovePosition(item);
    LeafNode *left = father->getLeftSonLeaf(father_pos);
    LeafNode *right = father->getRightSonLeaf(father_pos);

    if (left == nullptr and right == nullptr) {
        cout << "LeafNode::remove, front rear both null" << endl;
        return false;
    }

    if (isDeletable()) {
        delete_status = DELETE_SUCCESS;
        _delete(pos);
        return true;
    }

    if (left->isDeletable()) {
        delete_status = BORROW_FROM_BROTHER;
        _delete(pos);
        right->insert(left->key[left->pointer], left->column[left->pointer]);
        left->pointer--;
        father->setKey(father->getRemovePosition(item), left->key[left->pointer]);
        return true;
    }

    if (right->isDeletable()) {
        delete_status = BORROW_FROM_BROTHER;
        _delete(pos);
        left->insert(right->key[0], right->column[0]);
        right->_delete(0);
        father->setKey(father->getRemovePosition(item), right->key[0]);
        return true;
    }

// 将当前叶子合并到前一个叶子，二节点的key数量都小于 (B_PLUS_TREE_ORDER - 1) / 2
// 由于查找时，目标叶子在父节点的右侧，所以当前节点一定是父节点的右叶子
    if (!left->isDeletable() or !right->isDeletable()) {
        delete_status = MERGE_WITH_BROTHER;
        _delete(pos);
        for (int i = 0; i <= right->pointer; ++i) {
            left->key[left->pointer + 1] = right->key[i];
            left->column[left->pointer + 1] = right->column[i];
            left->pointer++;
        }
        left->rear = right->rear;
        if (right->rear)
            right->rear->front = left;
        return true;
    }
    return false;
}


int Node::getDeleteStatus() const {
    return delete_status;
}

LeafNode *LeafNode::frontLeaf() {
    return front;
}

LeafNode *LeafNode::nextLeaf() {
    return rear;
}

int LeafNode::searchColumn(int item) {
    int pos = getSearchPosition(item);
    if (pos == -1)
        return -1;
    else
        return column[pos];
}

bool LeafNode::changeColumn(int k, int c, int newCol) {
    int pos = getSearchPosition(k);
    if (pos == -1)
        return false;
    LeafNode *temp = this;
    while (temp) {
        for (int i = pos; i <= temp->pointer and temp->getKey(i) == k and temp->column[i] == c; ++i) {
            temp->column[i] = newCol;
        }
        temp = temp->frontLeaf();
        pos = 0;
    }
    return true;
}

BPlusTree::BPlusTree() {
    leafStart = new LeafNode;
    leafEnd = leafStart;
    root = nullptr;
}

void BPlusTree::insert(int item, int col) {
    if (root == nullptr) {
        LeafNode *newLeaf = leafStart->insert(item, col);
        if (newLeaf != nullptr) {
            root = new IndexNode;
            root->insert(newLeaf->getKey(0), nullptr, newLeaf);
        }
        return;
    }

    IndexNode *tempIndex = root;
    stack<IndexNode *> stack;
    while (!tempIndex->hasLeaf()) {
        stack.push(tempIndex);
        int insert_pos = tempIndex->getInsertPosition(item);
        tempIndex = tempIndex->getLeftSonNode(insert_pos);
    }
    int insert_pos = tempIndex->getInsertPosition(item);
    LeafNode *tempLeaf = tempIndex->getLeftSonLeaf(insert_pos);
    tempLeaf = tempLeaf->insert(item, col);
    if (tempLeaf == nullptr)
        return;

    // 叶子节点分裂，新叶子插入父索引节点
    if (leafEnd->nextLeaf() == tempLeaf)
        leafEnd = tempLeaf;
    tempIndex = tempIndex->insert(tempLeaf->getKey(0), nullptr, tempLeaf);
    if (tempIndex == nullptr)
        return;

    // 有叶子的索引节点分裂，若栈非空，弹出保存的父节点，并插入
    while (!stack.empty()) {
        IndexNode *top = stack.top();
        stack.pop();
        tempIndex = top->insert(tempIndex->getDivideToFatherKey(), tempIndex, nullptr);
        // 插入父节点没有分裂退出循环
        if (tempIndex == nullptr)
            break;
    }

    // 插入 root 节点后造成节点的分裂，创建新节点
    if (tempIndex != nullptr) {
        root = new IndexNode;
        root->insert(tempIndex->getDivideToFatherKey(), tempIndex, nullptr);
    }
}

void BPlusTree::remove(int key) {
    while (_remove(key));
}

bool BPlusTree::_remove(int key) {
    if (_search(key) == -1) {
        cout << "Removing Key is not Exist" << endl;
        return false;
    }
    stack<IndexNode *> stack;
    IndexNode *tempIndex = root;
    LeafNode *tempLeaf = leafStart;

    // 只有一个叶子的情况
    if (tempIndex == nullptr) {
        tempLeaf->remove(key, tempIndex);
        return true;
    }

    while (!tempIndex->hasLeaf()) {
        stack.push(tempIndex);
        tempIndex = tempIndex->getLeftSonNode(tempIndex->getInsertPosition(key));
    }
    // 叶子
    int leaf_pos = tempIndex->getInsertPosition(key);
    tempLeaf = tempIndex->getLeftSonLeaf(leaf_pos);
    int pos = tempLeaf->getSearchPosition(key);

    int small_max_than_key;
    if (tempLeaf->getPointer() == pos and tempLeaf->nextLeaf() != nullptr)
        small_max_than_key = tempLeaf->nextLeaf()->getKey(0);
    else
        small_max_than_key = tempLeaf->getKey(pos + 1);
    tempLeaf->remove(key, tempIndex);

    if (tempLeaf->getDeleteStatus() == DELETE_SUCCESS or tempLeaf->getDeleteStatus() == BORROW_FROM_BROTHER)
        return true;

    if (tempLeaf->getDeleteStatus() == MERGE_WITH_BROTHER) {
        if (stack.empty())
            tempIndex->remove(key, nullptr);
        else
            tempIndex->remove(key, stack.top());

        bool finish = false;
        while (!stack.empty() or tempIndex->hasLeaf()) {
            if (finish)
                break;
            switch (tempIndex->getDeleteStatus()) {
                case DELETE_SUCCESS:
                    finish = true;
                    break;
                case BORROW_FROM_BROTHER:
                    finish = true;
                    break;
                case MERGE_WITH_BROTHER:
                    tempIndex = stack.top();
                    stack.pop();
                    if (stack.empty())
                        tempIndex->remove(key, nullptr);
                    else
                        tempIndex->remove(key, stack.top());
                    break;
                default:
                    cout << "BPlusTree::remove, Remove Error" << endl;
                    return false;
            }
        }
        // 更改 root
        if (tempIndex->getPointer() == -1) {
            root = tempIndex->getLeftSonNode(0);
            delete tempIndex;
        }

        tempIndex = root;
        while (tempIndex) {
            if (tempIndex->getSearchPosition(key) != -1)
                tempIndex->setKey(tempIndex->getSearchPosition(key), small_max_than_key);
            tempIndex = tempIndex->getLeftSonNode(tempIndex->getInsertPosition(key));
        }
    }
    return true;
}

int BPlusTree::_search(int item) const {
    IndexNode *tempIndex = root;
    LeafNode *tempLeaf = leafStart;

    // 节点存在，只有一个叶子则在 leafstart 查找
    if (tempIndex) {
        while (!tempIndex->hasLeaf()) {
            int insert_pos = tempIndex->getInsertPosition(item);
            tempIndex = tempIndex->getLeftSonNode(insert_pos);
        }

        int insert_pos = tempIndex->getInsertPosition(item);
        tempLeaf = tempIndex->getLeftSonLeaf(insert_pos);
    }

    return tempLeaf->searchColumn(item);
}

int *BPlusTree::search(int item) const {
    return search(item, item);
}

/*
 * 区域顺序查找，给定上界和下界，在B+树中叶子结点顺序查找
 * 查找结果保存在 int 数组中
 */
int *BPlusTree::search(int upBoundary, int downBoundary) const {
    if (upBoundary > downBoundary)
        return nullptr;
    IndexNode *tempIndex = root;
    LeafNode *tempLeaf = leafStart;

    // 节点存在，只有一个叶子则在 leafstart 查找
    if (tempIndex) {
        while (!tempIndex->hasLeaf()) {
            int insert_pos = tempIndex->getInsertPosition(upBoundary);
            tempIndex = tempIndex->getLeftSonNode(insert_pos);
        }

        int insert_pos = tempIndex->getInsertPosition(upBoundary);
        tempLeaf = tempIndex->getLeftSonLeaf(insert_pos);
    }
    int *cols = new int[MAX_RECORDS_NUM];
    int pos = tempLeaf->getSearchPosition(upBoundary);
    if (pos == -1) {
        pos = tempLeaf->getInsertPosition(upBoundary);
        pos -- ;
    }
    int num = 0;

    // 若存在重复值，此时的叶子一定是最右一个叶子
    // 若上限在树中存在重复值，需要向前查找最左的叶子节点
    while (true) {
        for (int i = pos; i >= 0 and tempLeaf->getKey(i) == upBoundary; --i) {
            cols[num] = tempLeaf->column[i];
            num++;
        }

        if (!tempLeaf->frontLeaf() or
            tempLeaf->frontLeaf()->getKey(tempLeaf->frontLeaf()->getPointer()) != upBoundary) {
            break;
        }
        tempLeaf = tempLeaf->frontLeaf();
        pos = tempLeaf->getPointer();
    }
    pos ++;

    // 从下限向上限查找
    while (tempLeaf and tempLeaf->getKey(pos) <= downBoundary) {
        for (int i = pos; i <= tempLeaf->getPointer() and tempLeaf->getKey(i) <= downBoundary; ++i) {
            cols[num] = tempLeaf->column[i];
            num++;
        }
        tempLeaf = tempLeaf->nextLeaf();
        pos = 0;
    }
    if (num == 0)
        return nullptr;

    cols[num] = -1;
    return cols;
}

bool BPlusTree::changeColumn(int key, int column, int newCol) const {
    IndexNode *tempIndex = root;
    LeafNode *tempLeaf = leafStart;
    if (tempIndex) {
        while (!tempIndex->hasLeaf()) {
            int insert_pos = tempIndex->getInsertPosition(key);
            tempIndex = tempIndex->getLeftSonNode(insert_pos);
        }
        int insert_pos = tempIndex->getInsertPosition(key);
        tempLeaf = tempIndex->getLeftSonLeaf(insert_pos);
    }
    return tempLeaf->changeColumn(key, column, newCol);
}

bool BPlusTree::changeKey(int key, int newKey) {
    int *cols = search(key);
    if (cols == nullptr)
        return false;
    remove(key);
    int len = 0;
    while (cols[len] != -1) len++;
    for (int i = 0; i < len; ++i) {
        insert(newKey, cols[i]);
    }
    return true;
}

void BPlusTree::print() const {
    IndexNode *base = root;
    IndexNode *temp;
    queue<IndexNode *> queue;
    queue.push(root);
    while (base and !queue.empty()) {
        temp = queue.front();
        queue.pop();
        temp->print();

        if (base->getRightSonNode(base->getPointer()) == temp or base == temp) {
            base = temp;
            cout << endl;
        }

        if (temp->getLeftSonNode() != nullptr)
            queue.push(temp->getLeftSonNode());
        for (int i = 0; i <= temp->getPointer(); ++i) {
            if (temp->getRightSonNode(i) != nullptr)
                queue.push(temp->getRightSonNode(i));
        }
    }
    LeafNode *node = leafStart;
    while (node) {
        node->print();
        node = node->nextLeaf();
    }
    cout << endl;
}



//int main() {
//    BPlusTree bPlusTree;
//    for (int i = 1; i < MAX_RECORDS_NUM; i += 2) {
//        bPlusTree.insert(i, i);
//    }
////    bPlusTree.print();
//
////    int *ans = bPlusTree.search(40, 70);
////    for (int i = 0; i < MAX_RECORDS_NUM and ans[i]!=-1; ++i) {
////        cout << ans[i] << " " ;
////    }
//    bPlusTree.print();
//    bPlusTree.changeKey(3,2);
//    bPlusTree.remove(2);
//    bPlusTree.print();
//}
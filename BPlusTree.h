//
// Created by 刘洋 on 2021/10/17.
//

#ifndef LINUXCODE_BPLUSTREE_H
#define LINUXCODE_BPLUSTREE_H

#include "Setting.h"

class LeafNode;

class IndexNode;

class BPlusTree {
public:
    BPlusTree();
    void insert(int item, int col);

    /* 查找失败返回-1，否则返回行id */
    int _search(int) const;

    /* 支持重复值查找，查找结果保存在数组中，以-1结束 */
    int *search(int) const;

    /* 区域顺序查找，给定上界和下界，在B+树中叶子结点顺序查找,查找结果保存在数组中，以-1结束 */
    int *search(int upBoundary, int downBoundary) const;
    bool _remove(int);
    void remove(int);
    void print() const;

    // 修改键对应的 column
    bool changeColumn(int key, int col, int newCol) const;

    // 修改键值
    bool changeKey(int key, int newKey);

public:
    LeafNode *leafStart;
    LeafNode *leafEnd;
    IndexNode *root;
};

class Node {
public:
    Node() = default;;

    virtual ~Node() = default;;

    bool isFull() const;
    bool isDeletable() const;
    int getPointer() const;
    int getKey(int) const;
    bool setKey(int, int);

    // 查找到相同值，返回下一 index_array 作为插入位置
    int getInsertPosition(int);
    int getSearchPosition(int);
    int getRemovePosition(int);
    void print();
    int getDeleteStatus() const;


private:
    // 二分查找，存在相同值则返回最左边的 index
    int _binarySearch(int item);

protected:
    int pointer{DEFAULT};
    int key[B_PLUS_TREE_ORDER - 1]{0};
    int delete_status{DEFAULT};
};

class LeafNode : public Node {
    friend BPlusTree;
public:
    LeafNode *divide(bool insert_left = false);
    LeafNode *insert(int, int);
    bool remove(int, IndexNode*);

    void _delete(int);
    LeafNode *frontLeaf();
    LeafNode *nextLeaf();
    int searchColumn(int);
    bool changeColumn(int k, int c, int newCol);

private:
    // index_array of primary key
    int column[B_PLUS_TREE_ORDER - 1]{DEFAULT};
    LeafNode *front{nullptr};
    LeafNode *rear{nullptr};
};

class IndexNode : public Node {
public:
    IndexNode() : Node() {};;
    IndexNode(int, IndexNode *, IndexNode *);
    IndexNode *divide(bool insert_left = false);
    IndexNode *insert(int, IndexNode *, LeafNode *leaf = nullptr);
    IndexNode *getLeftSonNode(int index = 0);
    LeafNode *getLeftSonLeaf(int index = 0);
    IndexNode *getRightSonNode(int index = 0);
    LeafNode *getRightSonLeaf(int index = 0);

    // 删除节点和右指针
    bool _delete(int pos);
    IndexNode* remove(int, IndexNode*);
    int getDivideToFatherKey() const;
    bool hasLeaf() const;

private:
    bool has_leaf = false;
    IndexNode *sonIndexNode[B_PLUS_TREE_ORDER]{nullptr};
    LeafNode *leafNode[B_PLUS_TREE_ORDER]{nullptr};
    int toTopNodeKey{DEFAULT};
    IndexNode *leftIndexNode{nullptr};
};


#endif //LINUXCODE_BPLUSTREE_H

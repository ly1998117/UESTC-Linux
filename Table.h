//
// Created by 刘洋 on 2021/10/16.
//

#ifndef LINUXCODE_TABLE_H
#define LINUXCODE_TABLE_H

#include "Setting.h"
#include "BPlusTree.h"

/*
 * Table 的基本数据结构和方法
 * 保证进程内创建一个table实例，防止多线程操作出错
 */
class Table {
public:
    static Table *getTableInstance();
    ~Table();

    // col 为记录的插入位置
    bool insert(int col, Record *);

    // 插入表的末尾
    bool append(Record *);

    // 给定属性和关键字，范围修改为新的键
    bool change(int row, int key, int newKey);

    // 删除关键字，支持重复值删除. -1 代表主键
    bool remove(int row, int key);

    // row：属性id，upBoundary：上界， downBoundary下界
    RecordBlock *search(int row, int upBoundary, int downBoundary);

    void print();

    static void record_print(Record *);

    static void record_print(RecordBlock *);

    bool copy_table(Record *, int, int);

    Record *get_table_struct();

    void print_tree(int row);

    // 读取索引数据，若 index=null 说明文件不存在 则从表中初始化
    bool readRow(int row, int *index);

    bool readFilled(int *);

    int *getFilled() { return filled; };

    int *getRow(int row);

    // id = -1 为主键树，其他为属性树,为空则返回空
    BPlusTree *getIndexTree(int row);

    BPlusTree *getIndexTreeAndInit(int row);

    // 初始化索引树，若指针不空则返回
    void initIndexTree(int row);

    // 删除索引树，指针置为 null
    void deleteIndexTree(int row);

    // 置为空树
    void emptyCorrectTree(int row);


private:
    explicit Table();

    BPlusTree *primaryBPlusTree;
    BPlusTree *attributeBPlusTreeList[MAX_ATTRIBUTES_NUM] = {};
    Record *table;
    int top;
    int bottom;
    int *filled;
    static Table *table_instance;
};

#endif //LINUXCODE_TABLE_H

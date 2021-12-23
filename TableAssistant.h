//
// Created by 刘洋 on 2021/10/16.
//

#ifndef LINUXCODE_TABLEASSISTANT_H
#define LINUXCODE_TABLEASSISTANT_H

#include "Table.h"
#include "CacheManage.h"

class TableAssistant {
public:
    explicit TableAssistant();
    ~TableAssistant();

    /*****************************表的增删改查********************************/
    bool append(int, const int *, int num = MAX_ATTRIBUTES_NUM);
    bool insert(int col, int key, const int *, int num = MAX_ATTRIBUTES_NUM);

    // 批量删除
    bool remove(int row, int key);

    // 将指定属性的给定所有 key 更改为 newKey，同时更改索引树的节点
    bool change(int row, int key, int newKey);

    RecordBlock *search(int row, int up, int down);

    /****************************存储**************************************/
    bool save_table();
    // 从文件读入指定的行范围的表
    bool read_table(int start, int end);


    void printTable();
    void printIndexTree(int row);
    static void record_print(Record *);
    static void record_print(RecordBlock *);

private:
    static Record *toRecord(int, const int *, int);
    bool read_row_file(int row);

private:
    CacheManager *cacheManager;
    Table *table;
};


#endif //LINUXCODE_TABLEASSISTANT_H

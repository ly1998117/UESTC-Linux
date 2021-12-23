//
// Created by 刘洋 on 2021/10/16.
//

#ifndef LINUXCODE_SETTING_H
#define LINUXCODE_SETTING_H

#include <cstdint>
#include <iostream>
#include <cstring>
#include <vector>


#define TABLE_NAME "table"
#define MAX_ATTRIBUTES_NUM 100
#define MAX_RECORDS_NUM 10000
#define B_PLUS_TREE_ORDER 5
#define DEFAULT -1
#define DELETE_SUCCESS 0
#define BORROW_FROM_BROTHER 1
#define MERGE_WITH_BROTHER 2

using namespace std;
typedef struct {
    int primary_key;
    int attributes[MAX_ATTRIBUTES_NUM];
} Record;

typedef struct {
    int len;
    Record *record;
} RecordBlock;


#endif //LINUXCODE_SETTING_H

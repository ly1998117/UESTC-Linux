//
// Created by 刘洋 on 2021/10/16.
//

#include "TableAssistant.h"

TableAssistant::TableAssistant() {
    cacheManager = CacheManager::initialize(TABLE_NAME);
    table = Table::getTableInstance();
}

bool TableAssistant::append(int key, const int *attribute, int len) {
    auto *record = toRecord(key, attribute, len);
    return table->append(record);;
}

Record *TableAssistant::toRecord(int key, const int *attribute, int len) {
    auto *record = new Record;
    record->primary_key = key;
    for (int i = 0; i < len; ++i) {
        record->attributes[i] = attribute[i];
    }
    return record;
}

// 保存table到文件
bool TableAssistant::save_table() {
    bool flag = cacheManager->table_write(table->get_table_struct(), MAX_RECORDS_NUM) and
                cacheManager->filled_write(table->getFilled()) and
                cacheManager->row_write(-1, table->getRow(-1));
    if (flag) {
        for (int i = 0; i < MAX_ATTRIBUTES_NUM; ++i) {
            if (table->getIndexTree(i))
                cacheManager->row_write(i, table->getRow(i));
        }
    }
    return flag;
}

bool TableAssistant::insert(int col, int key, const int *attribute, int len) {
    return table->insert(col, toRecord(key, attribute, len));
}

bool TableAssistant::remove(int row, int key) {
    read_row_file(row);
    return table->remove(row, key);
}

bool TableAssistant::change(int row, int key, int newKey) {
    return table->change(row, key, newKey);
}

void TableAssistant::printTable() {
    table->print();
}

bool TableAssistant::read_table(int start, int end) {
    RecordBlock *table_struct = cacheManager->table_read(start, end);
    if (table_struct == nullptr)
        return false;
    int *filled = cacheManager->filled_read();
    table->readFilled(filled);
    // 树读入
    table->readRow(-1, cacheManager->row_read(-1));
    for (int i = 0; i < MAX_ATTRIBUTES_NUM; ++i) {
        int *temp = cacheManager->row_read(i);
        if (temp)
            table->readRow(i, temp);
    }
    table->copy_table(table_struct->record, start, end);
    return true;
}

bool TableAssistant::read_row_file(int row) {
    if (row == -1 or table->getIndexTree(row))
        return false;
    cout << "IndexTree: [" << row << "] Not Loaded, start Loading from file" << endl;
    int *row_array = cacheManager->row_read(row);
    if (row_array == nullptr) {
        cout << "IndexTree file Not Exist, start Loading from table" << endl;
        table->initIndexTree(row);
    }

    // 索引在文件中保存则读入
    table->readRow(row, row_array);
    cout << "IndexTree: [" << row << "] Loads successfully" << endl;
    return true;
}

RecordBlock *TableAssistant::search(int row, int up, int down) {
    read_row_file(row);
    RecordBlock *recordBlock = table->search(row, up, down);
    return recordBlock;
}

void TableAssistant::printIndexTree(int row) {
    table->print_tree(row);
}

void TableAssistant::record_print(Record *record) {
    Table::record_print(record);
}

void TableAssistant::record_print(RecordBlock *recordBlock) {
    Table::record_print(recordBlock);
}

TableAssistant::~TableAssistant(){
    delete table;
};

//
// Created by 刘洋 on 2021/10/16.
//

#include "Table.h"
#include "iomanip"

Table *Table::table_instance = nullptr;

Table::Table() {
    table = new Record[MAX_RECORDS_NUM];
    top = -1;
    bottom = -1;
    filled = new int[MAX_RECORDS_NUM]{0};
    primaryBPlusTree = new BPlusTree;
}

bool Table::append(Record *record) {
    if (primaryBPlusTree->search(record->primary_key) != nullptr) {
        cout << "Primary Key EXIST, Duplicate Entry: [" << record->primary_key << "]" << endl;
        return false;
    }

    if (++bottom >= MAX_RECORDS_NUM) {
        return false;
    }

    if (top == -1)
        top = 0;

    table[bottom] = *record;
    filled[bottom] = 1;
    primaryBPlusTree->insert(record->primary_key, bottom);
    for (int i = 0; i < MAX_ATTRIBUTES_NUM and attributeBPlusTreeList[i] != nullptr; ++i)
        attributeBPlusTreeList[i]->insert(record->attributes[i], bottom);

    return true;
}

bool Table::change(int row, int key, int new_key) {
    int *cols = getIndexTreeAndInit(row)->search(key);
    if (cols == nullptr) {
        cout << "Search key: " << key << " NOT EXIST" << endl;
        return false;
    }
    int len = 0;
    while (cols[len] != -1) len++;
    if (row != -1 and attributeBPlusTreeList[row])
        attributeBPlusTreeList[row]->changeKey(key, new_key);
    else
        primaryBPlusTree->changeKey(key, new_key);

    for (int i = 0; i < len; ++i) {
        if (filled[cols[i]] == 0)
            return false;
        if (row == -1)
            table[cols[i]].primary_key = new_key;
        else
            table[cols[i]].attributes[row] = new_key;
    }
    return true;
}

bool Table::insert(int col, Record *record) {
    if (primaryBPlusTree->search(record->primary_key) != nullptr) {
        cout << "Primary Key EXIST, Duplicate Entry: [" << record->primary_key << "]" << endl;
        return false;
    }
    if (filled[col] == 0) {
        table[col] = *record;
        filled[col] = 1;
        primaryBPlusTree->insert(record->primary_key, col);
        for (int i = 0; i < MAX_ATTRIBUTES_NUM and attributeBPlusTreeList[i] != nullptr; ++i)
            attributeBPlusTreeList[i]->insert(record->attributes[i], col);


        if (col > bottom)
            bottom = col;
    } else {
        if (col == bottom)
            append(record);
        else {
            bottom++;
            for (int j = bottom; j > col; --j) {
                table[j] = table[j - 1];
                filled[j] = filled[j - 1];
                primaryBPlusTree->changeColumn(table[j - 1].primary_key, j - 1, j);
                for (int i = 0; i < MAX_ATTRIBUTES_NUM and attributeBPlusTreeList[i] != nullptr; ++i)
                    attributeBPlusTreeList[i]->changeColumn(table[j - 1].attributes[i], j - 1, j);
            }
            table[col] = *record;
            filled[col] = 1;
            primaryBPlusTree->insert(record->primary_key, col);
            for (int i = 0; i < MAX_ATTRIBUTES_NUM and attributeBPlusTreeList[i] != nullptr; ++i)
                attributeBPlusTreeList[i]->insert(record->attributes[i], col);
        }
    }
    if (col < top or top == -1)
        top = col;

    return true;
}


bool Table::remove(int attr, int key) {
    if (getIndexTreeAndInit(attr)->search(key) == nullptr) {
        cout << "KEY " << key << " NOT EXIST: remove failed" << endl;
        return false;
    }
    int *col = getIndexTreeAndInit(attr)->search(key);
    int len = 0;
    while (col[len] != -1)len++;
    // 删除重复值
    getIndexTreeAndInit(attr)->remove(key);
    for (int i = 0; i < len; ++i) {
        filled[i] = 0;
        table[i].primary_key = 0;
        memset(table[i].attributes, 0, MAX_ATTRIBUTES_NUM);
    }
    delete[] col;
    while (!filled[top])top++;
    while (!filled[bottom])bottom--;
    return true;
}

void Table::print() {
    cout << "[Print: 共 " << bottom - top + 1 << " 行, 仅显示前 10 行]" << endl;
    std::cout << "p_key\t";
    for (int i = 0; i < 2; ++i) {
        std::cout
                << "-------------------------------------------attributes--------------------------------------------";
    }
    std::cout << std::endl;

    for (int i = top; i <= bottom and i < top + 10; ++i) {
        record_print(&table[i]);
    }
    std::cout << "p_key\t";
    for (int i = 0; i < 2; ++i) {
        std::cout
                << "-------------------------------------------attributes--------------------------------------------";
    }
    std::cout << std::endl;
}

void Table::record_print(Record *record) {
    if (record == nullptr)
        return;
    cout << "| " << setiosflags(ios::left) << std::setw(3) << record->primary_key << "| ";
    for (auto attribute: record->attributes) {
        cout << std::setw(4) << attribute;
    }
    cout << endl;
}

void Table::record_print(RecordBlock *record) {
    if (record == nullptr)
        return;
    cout << "[Print: 共 " << record->len << " 行, 仅显示前 10 行]" << endl;
    for (int i = 0; i < record->len and i < 10; ++i) {
        record_print(&record->record[i]);
    }
}

Record *Table::get_table_struct() {
    return table;
}

bool Table::copy_table(Record *record, int start, int end) {
    top = start;
    while (!filled[top]) top++;
    for (int i = start; i <= end; ++i)
        if (filled[i]) {
            table[i] = record[i - start];
            bottom = i;
        }
    return true;
}

/*
 *范围查找，返回所查找的记录块
 */
RecordBlock *Table::search(int id, int upBoundary, int downBoundary) {
    int *cols = getIndexTreeAndInit(id)->search(upBoundary, downBoundary);
    if (cols == nullptr) {
        cout << "Search key: ( " << upBoundary << " , " << downBoundary << " ) NOT EXIST" << endl;
        return nullptr;
    }
    int size = 0;
    for (int i = 0; cols[i] != -1; ++i) {
        if (cols[i] >= top and cols[i] <= bottom) {
            cols[size] = cols[i];
            size++;
        }
    }

    auto *recordBlock = new RecordBlock{size, new Record[size]};
    for (int i = 0; i < size; ++i) {
        recordBlock->record[i] = table[cols[i]];
    }
    delete[] cols;
    return recordBlock;
}

void Table::print_tree(int id) {
    if (!getIndexTree(id))
        cout << "IndexTree Not Loaded, Please Init first" << endl;
    else
        getIndexTree(id)->print();
}

bool Table::readRow(int id, int *items) {
    if (items == nullptr) {
        deleteIndexTree(id);
        initIndexTree(id);
    } else {
        emptyCorrectTree(id);
        for (int i = 0; i < MAX_RECORDS_NUM; ++i) {
            if (filled[i] != 0)
                getIndexTreeAndInit(id)->insert(items[i], i);
        }
        delete[] items;
    }
    return true;
}

bool Table::readFilled(int *fill) {
    delete[] filled;
    filled = fill;
    return true;
}

Table *Table::getTableInstance() {
    if (table_instance != nullptr)
        return table_instance;
    table_instance = new Table;
    return table_instance;
}

void Table::initIndexTree(int row) {
    if (row == -1) {
        if (primaryBPlusTree != nullptr)
            return;
        primaryBPlusTree = new BPlusTree;
        for (int i = 0; i < MAX_RECORDS_NUM; ++i) {
            if (filled[i]) {
                primaryBPlusTree->insert(table[i].primary_key, i);
            }
        }
    } else {
        if (attributeBPlusTreeList[row] != nullptr)
            return;

        attributeBPlusTreeList[row] = new BPlusTree;
        for (int i = 0; i < MAX_RECORDS_NUM; ++i) {
            if (filled[i]) {
                attributeBPlusTreeList[row]->insert(table[i].attributes[row], i);
            }
        }
    }
}


void Table::deleteIndexTree(int row) {
    if (row == -1) {
        delete[] primaryBPlusTree;
        primaryBPlusTree = nullptr;
        return;
    }
    if (attributeBPlusTreeList[row] == nullptr)
        return;
    delete[] attributeBPlusTreeList[row];
    attributeBPlusTreeList[row] = nullptr;
}

BPlusTree *Table::getIndexTree(int row) {
    if (row == -1) {
        return primaryBPlusTree;
    } else {
        return attributeBPlusTreeList[row];
    }
}

BPlusTree *Table::getIndexTreeAndInit(int row) {
    if (!getIndexTree(row))
        initIndexTree(row);
    return getIndexTree(row);
}

void Table::emptyCorrectTree(int id) {
    if (id == -1) {
        delete[] primaryBPlusTree;
        primaryBPlusTree = new BPlusTree;
    } else {
        deleteIndexTree(id);
        attributeBPlusTreeList[id] = new BPlusTree;
    }
}

int *Table::getRow(int row) {
    int *array = new int[MAX_RECORDS_NUM];
    if (row == -1) {
        for (int i = 0; i < MAX_RECORDS_NUM; ++i) {
            array[i] = table[i].primary_key;
        }
    } else {
        for (int i = 0; i < MAX_RECORDS_NUM; ++i) {
            array[i] = table[i].attributes[row];
        }
    }
    return array;
}

Table::~Table(){
    table_instance = nullptr;
};

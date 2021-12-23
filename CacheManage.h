//
// Created by 刘洋 on 2021/10/16.
//

#ifndef LINUXCODE_CACHEMANAGE_H
#define LINUXCODE_CACHEMANAGE_H

#include "vector"
#include "iostream"
#include "Setting.h"
#include "unistd.h"


using namespace std;

class CacheManager {
public:
    static CacheManager *initialize(const char *);

    bool _write(const char *pstrMsg);
    // 表结构写入文件 table.tab
    bool table_write(Record *record, int num);
    // 表文件读入内存
    RecordBlock *table_read(int begin, int end);
    // 指定的缩引树数据写入文件 table[row].row
    bool row_write(int row, int*);
    int *row_read(int row);

    bool filled_write(int*);
    int *filled_read();

    /**********************日志函数************************/
    bool cache_write(const char *pstrMsg);
    static void writeLog(const char *);
    bool flush();

private:
    explicit CacheManager(const char *);

    ~CacheManager();

    static void onProcessExit();

    static int pathExist(const char *);

private:
    int Fd;
    char *cache;
    unsigned int cacheUsedBytes;
    bool flag_exit;
    static vector<CacheManager *> cacheManagerVector;
    const char *filepath;
    unsigned int currentManagerId;
};

#endif //LINUXCODE_CACHEMANAGE_H

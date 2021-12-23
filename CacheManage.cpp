//
// Created by 刘洋 on 2021/10/15.
//
#include "CacheManage.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#define CACHE_SIZE 4096

vector<CacheManager *> CacheManager::cacheManagerVector;

CacheManager::CacheManager(const char *path) {
    filepath = path;
    Fd = -1;
    cache = new char[CACHE_SIZE];
    cacheUsedBytes = 0;
    flag_exit = false;
    currentManagerId = -1;
};


CacheManager *CacheManager::initialize(const char *path) {
    int id = pathExist(path);
    CacheManager *manager;
    if (cacheManagerVector.empty() or id == -1) {
        manager = new CacheManager(path);
        manager->Fd = open(manager->filepath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        cacheManagerVector.push_back(manager);
        manager->currentManagerId = cacheManagerVector.size() - 1;
    } else {
        manager = cacheManagerVector[id];
    }
    if (atexit(CacheManager::onProcessExit) != 0) {
        if (manager != nullptr) {
            manager->flag_exit = true;
            CacheManager::writeLog("In CacheManager::initialize(), atexit error");
        }
    }
    return manager;
}

int CacheManager::pathExist(const char *path) {
    for (int i = 0; i < cacheManagerVector.size(); i++) {
        if (cacheManagerVector.at(i)->filepath == path)
            return i;
    }
    return -1;
}

CacheManager::~CacheManager() {
    flush();
}


bool CacheManager::_write(const char *pstrMsg) {
    if (pstrMsg == nullptr or strlen(pstrMsg) == 0)
        return false;
    unsigned int left_room = CACHE_SIZE - cacheUsedBytes;
    unsigned int len_strmsg = strlen(pstrMsg);


    if ((len_strmsg > CACHE_SIZE) || (flag_exit)) {
        if (Fd == -1)
            return false;
        ssize_t r = write(Fd, pstrMsg, len_strmsg);
        if (r == -1)
            return false;
        return true;
    }

    if (len_strmsg > left_room) {
        bool s = flush();
        if (!s)
            return false;
    }
    memcpy(cache + cacheUsedBytes, pstrMsg, len_strmsg);
    cacheUsedBytes += len_strmsg;
    return true;
}


bool CacheManager::flush() {
    if (cacheUsedBytes != 0) {
        ssize_t r = write(Fd, cache, cacheUsedBytes);
        if (r == -1) {
            cout << "CacheManager::flush, flush error\n" << endl;
            return false;
        }
    }
    cacheUsedBytes = 0;
    memset(cache, 0, CACHE_SIZE);
    return true;
}

void CacheManager::onProcessExit() {
    if (!CacheManager::cacheManagerVector.empty()) {
        for (auto &i: CacheManager::cacheManagerVector) {
            i->flush();
            i->flag_exit = true;
        }
        cout << "OnProcessExit Flush done" << endl;
    }
}


bool CacheManager::cache_write(const char *pstrMsg) {
    if (currentManagerId < 0)
        return false;
    if (!_write(pstrMsg)) {
        return false;
    }
    return true;
}

bool CacheManager::table_write(Record *record, int num) {
    close(Fd);
    string string1(filepath);
    string1.append(".tab");
    remove((char*) string1.data());

    Fd = open(string1.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (Fd == -1) {
        CacheManager::writeLog("table open error\n");
        cout << "table open error on " << filepath << endl;
        return false;
    }
    ssize_t s = write(Fd, record, sizeof(Record) * num);
    if (s != sizeof(Record) * num) {
        CacheManager::writeLog("Record write error\n");
        cout << "write error on " << filepath << endl;
        return false;
    }
    return true;
}

bool CacheManager::row_write(int row, int *keys) {
    close(Fd);
    string string1(filepath);
    string1.append(to_string(row));
    string1.append(".row");
    remove((char*) string1.data());

    Fd = open(string1.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (Fd == -1) {
        CacheManager::writeLog("index_array file open error\n");
        cout << "index_array file open error on " << string1 << endl;
        return false;
    }
    ssize_t s = write(Fd, keys, sizeof(int) * MAX_RECORDS_NUM);
    if (s != sizeof(int) * MAX_RECORDS_NUM) {
        CacheManager::writeLog("index_array write error\n");
        cout << "write error on " << string1 << endl;
        return false;
    }
    return true;
}

bool CacheManager::filled_write(int *filled) {
    close(Fd);
    string string1(filepath);
    string1.append(".fil");
    remove((char*) string1.data());

    Fd = open(string1.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (Fd == -1) {
        CacheManager::writeLog("filled file open error\n");
        cout << "filled file open error on " << string1 << endl;
        return false;
    }
    ssize_t s = write(Fd, filled, sizeof(int) * MAX_RECORDS_NUM);
    if (s != sizeof(int) * MAX_RECORDS_NUM) {
        CacheManager::writeLog("filled write error\n");
        cout << "write error on " << string1 << endl;
        return false;
    }
    return true;
}

int *CacheManager::row_read(int row) {
    close(Fd);
    string string1(filepath);
    string1.append(to_string(row));
    string1.append(".row");
    Fd = open(string1.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
    if (Fd == -1)
        return nullptr;
    auto *index = new int[MAX_RECORDS_NUM];
    ssize_t s = read(Fd, index, sizeof(int) * MAX_RECORDS_NUM);
    if (s == -1) {
        CacheManager::writeLog("index_array read error\n");
        return nullptr;
    }
    if (s != sizeof(int) * MAX_RECORDS_NUM) {
        cout << "read error on " << string1 << ", actual index_array getPointer: " << s / sizeof(Record) << endl;
        return nullptr;
    }
    return index;
}

int *CacheManager::filled_read() {
    close(Fd);
    string string1(filepath);
    string1.append(".fil");
    Fd = open(string1.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
    auto *filled = new int[MAX_RECORDS_NUM];
    ssize_t s = read(Fd, filled, sizeof(int) * MAX_RECORDS_NUM);
    if (s == -1) {
        CacheManager::writeLog("filled read error\n");
        return nullptr;
    }
    if (s != sizeof(int) * MAX_RECORDS_NUM) {
        cout << "read error on " << string1 << ", actual filled getPointer: " << s / sizeof(Record) << endl;
        return nullptr;
    }
    return filled;
}

void CacheManager::writeLog(const char *msg) {
    time_t now = time(nullptr);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';
    strcat(dt, "---");
    strcat(dt, msg);
    msg = dt;
    CacheManager *cacheManager = CacheManager::initialize("log.txt");
    cacheManager->cache_write(msg);
//    delete cacheManager;
}

RecordBlock *CacheManager::table_read(int begin, int end) {
    if (begin < 0 or end > MAX_RECORDS_NUM or end < begin) {
        CacheManager::writeLog("CacheManager:table_read: begin and end error\n");
        cout << "CacheManager:table_read: begin and end error\n" << endl;
        return nullptr;
    }
    close(Fd);
    string string1(filepath);
    string1.append(".tab");
    Fd = open(string1.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
    int size = end - begin + 1;
    auto *recordBlock = new RecordBlock{size, new Record[size]};
    lseek(Fd, (unsigned) sizeof(Record) * (begin), SEEK_SET);
    ssize_t s = read(Fd, recordBlock->record, sizeof(Record) * size);
    if (s == -1) {
        CacheManager::writeLog("Record read error\n");
        return nullptr;
    }
    if (s != sizeof(Record) * size) {
        cout << "read error on " << string1 << ", actual table getPointer: " << s / sizeof(Record) << endl;
        return nullptr;
    }
    lseek(Fd, 0, SEEK_SET);
    return recordBlock;
}


//
// Created by 刘洋 on 2021/10/15.
//

#include "TableAssistant.h"
#include "ConditionVariable.h"
#include "CLThread.h"
#include "Event.h"
#include "random"

class MultiThreadTest {
public:
    explicit MultiThreadTest(int i) {
        event = new Event[i];
        mutex = new Mutex;
    };

    static void multiThreadRun();

    static void mainFunction();

    static void threadFunction1();
    static void threadFunction2();


    Event *event;
    Mutex *mutex;
};

class Thread1 : public CLExecutiveFunctionProvider {
public:
    explicit Thread1() = default;

    ~Thread1() {};

    bool RunExecutiveFunction(void *pContext) override {
        try {
            auto *multiThreadTest = (MultiThreadTest *) pContext;
            multiThreadTest->event[0].wait();
            {
                CriticalSection criticalSection(multiThreadTest->mutex);
                MultiThreadTest::threadFunction1();
            }
        } catch (const char *e) {
            cout << e << endl;
        }
        return true;
    }
};

class Thread2 : public CLExecutiveFunctionProvider {
public:
    explicit Thread2() = default;

    ~Thread2() {};

    bool RunExecutiveFunction(void *pContext) override {
        try {
            auto *multiThreadTest = (MultiThreadTest *) pContext;
            multiThreadTest->event[1].wait();
            {
                CriticalSection criticalSection(multiThreadTest->mutex);
                MultiThreadTest::threadFunction2();
            }
        } catch (const char *e) {
            cout << e << endl;
        }
        return true;
    }
};


void MultiThreadTest::multiThreadRun() {
    CLExecutiveFunctionProvider *thread1 = new Thread1();
    CLExecutiveFunctionProvider *thread2 = new Thread2();

    CLExecutive *pThread1 = new CLThread(thread1);
    CLExecutive *pThread2 = new CLThread(thread2);
    try {
        auto *multiThreadTest = new MultiThreadTest(2);
        pThread1->Run((void *) multiThreadTest);
        pThread2->Run((void *) multiThreadTest);
        {
            CriticalSection section(multiThreadTest->mutex);
            MultiThreadTest::mainFunction();
        }
        multiThreadTest->event[0].wakeup();
        multiThreadTest->event[1].wakeup();

        delete multiThreadTest;
        delete pThread1;
        delete pThread2;
        delete thread1;
        delete thread2;

    } catch (const char *error) {
        cout << error << endl;
    }
}


/*********************************************************************************/
class Test {
public:
    // 产生随机数
    static int randint(int a, int b) {
        random_device rd;
        default_random_engine generator = default_random_engine(rd());
        uniform_int_distribution<int> distribute(a, b);
        return distribute(generator);
    };

    void table_create(int num = MAX_RECORDS_NUM) {
        int *attributes = new int[MAX_ATTRIBUTES_NUM];
        for (int i = 0; i < num; ++i) {
            for (int j = 0; j < MAX_ATTRIBUTES_NUM; ++j) {
                attributes[j] = randint(0, 100);
            }
            tableAssistant.append(i, attributes);
        }
    }

    void table_save() {
        tableAssistant.save_table();
    }

    void table_read(int start = 0, int end = MAX_RECORDS_NUM-1) {
        tableAssistant.read_table(start, end);
    }

    void search(int row, int up, int down) {
        TableAssistant::record_print(tableAssistant.search(row, up, down));
    }

    ~Test()= default;


public:
    TableAssistant tableAssistant;
};

void MultiThreadTest::mainFunction() {
    cout << "This is mainFunction" << endl;
    Test *test = new Test;
    test->table_create(1000);
    test->tableAssistant.printTable();
    test->table_save();
    // 销毁内存表
    delete test;
}

void MultiThreadTest::threadFunction1() {
    cout << "This is threadFunction1" << endl;
    Test *test = new Test;
    test->table_read(200, 500);
    test->tableAssistant.printTable();
    test->search(5, 30,100);
}

void MultiThreadTest::threadFunction2() {
    cout << "This is threadFunction2" << endl;
    Test *test = new Test;
    test->table_read(0,20);
    test->tableAssistant.printTable();

    int * att = new int[MAX_ATTRIBUTES_NUM];
    for (int i = 0; i < MAX_ATTRIBUTES_NUM; ++i) {
        att[i] = Test::randint(-10,0);
    }
    test->tableAssistant.insert(3,-1,att);
    test->tableAssistant.printTable();
}

int main() {
    MultiThreadTest multiThreadTest(2);
    multiThreadTest.multiThreadRun();

    return 0;
}

#ifndef LINUXCODE_CLTHREAD_H
#define LINUXCODE_CLTHREAD_H

#include <pthread.h>
#include "CLExecutive.h"

class CLThread : public CLExecutive {
public:
    explicit CLThread(CLExecutiveFunctionProvider *pExecutiveFunctionProvider) : CLExecutive(
            pExecutiveFunctionProvider) {};

    ~CLThread() override;

    bool Run(void *pContext) override;

    bool WaitForDeath() override;

private:
    static void *StartFunctionOfThread(void *pContext);

private:
    void *m_pContext{nullptr};
    pthread_t m_ThreadID;
};

#endif

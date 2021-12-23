#ifndef LINUXCODE_CLEXECUTIVE_H
#define LINUXCODE_CLEXECUTIVE_H
#include "CLExecutiveFunctionProvider.h"


class CLExecutive {
public:
    explicit CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider) {
        if (pExecutiveFunctionProvider == nullptr) {
            CacheManager::writeLog("In CLExecutive::CLExecutive(), pExecutiveFunctionProvider error");
            throw "In CLExecutive::CLExecutive(), pExecutiveFunctionProvider error";
        }

        m_pExecutiveFunctionProvider = pExecutiveFunctionProvider;
    };

    virtual ~CLExecutive(){};

    virtual bool Run(void *pContext) = 0;

    virtual bool WaitForDeath() = 0;

protected:
    CLExecutiveFunctionProvider *m_pExecutiveFunctionProvider;

private:
    CLExecutive(const CLExecutive &);

    CLExecutive &operator=(const CLExecutive &);
};

#endif
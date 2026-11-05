#pragma once
#include "Backend.h"

struct LattHM;

class TCO
{
private:
    llvm::Module& m_Module;

public:
    TCO(Backend& be) : m_Module(be.getModule()) {}

    void run(void);

private:
    void handleFun(llvm::Function& fun);
    void handleTerminator(llvm::ReturnInst* inst);
};

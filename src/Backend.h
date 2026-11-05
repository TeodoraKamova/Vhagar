#pragma once

#include "Ast.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#pragma GCC diagnostic pop

void inlineFunctions(llvm::Module &module);

class Backend {
public:
  using Context = llvm::LLVMContext;
  using Builder = llvm::IRBuilder<>;
  using Module = llvm::Module;
  using Buffer = llvm::SmallVector<llvm::Value *>;

private:
  Buffer m_Buffer;

  Context m_Context;
  Builder m_Builder;
  Module m_Module;

public:
  Backend(const std::string &moduleName)
      : m_Buffer(), m_Context(), m_Builder(m_Context),
        m_Module(moduleName, m_Context) {}

  inline Buffer &getBuffer() { return m_Buffer; }
  inline Context &getContext() { return m_Context; }
  inline Builder &getBuilder() { return m_Builder; }
  inline Module &getModule() { return m_Module; }

  void codegen(const Ast &ast, const Locator &locator);

  void verifyModule();
  void emitModule(const std::string &outfile) const;

  void removeUnreachableBlocks(llvm::Function &function);

  void mem2reg(void);
  void sccp(void);
  void licm(void);

  void simplifyCFG(void);
  void inliner(void);

  void constDiv(void);
  void tco(void);
  void dedupGlobalStringData(void);
};

#include <memory>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/CFG.h"
#include "clang/Basic/Version.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"

using namespace clang;
using namespace llvm;
using namespace clang::ast_matchers;

class Callback : public MatchFinder::MatchCallback {
  public:
  virtual void run(const MatchFinder::MatchResult &Result)
  {
    const FunctionDecl *F = Result.Nodes.getNodeAs<FunctionDecl>("func");
    if (F && F->hasBody()) {
      CFG::BuildOptions Opts;
      Opts.AddImplicitDtors = true;
      Opts.AddEHEdges = true;
      Opts.PruneTriviallyFalseEdges = true;
      Opts.AddEHEdges = true;
      Opts.AddInitializers = true;
      Opts.AddImplicitDtors = true;
      Opts.AddTemporaryDtors = true;
      Opts.AddStaticInitBranches = true;
      Opts.AddCXXNewAllocator = true;
      Opts.AddCXXDefaultInitExprInCtors = true;

      errs() << "Generating CFG for function: ";
      F->getNameForDiagnostic(errs(), PrintingPolicy(LangOptions()), false);
      errs() << "\n";

      // XXX: Is this API supported in all versions of clang we support?
      std::unique_ptr<CFG> Cfg = CFG::buildCFG(F, F->getBody(), &F->getASTContext(), Opts);
    }
  }
};

class CheckAction : public PluginASTAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef fileName) override {
    Callback *cb = new Callback;
    MatchFinder *matcher = new MatchFinder;
    matcher->addMatcher(functionDecl().bind("func"), cb);
    return matcher->newASTConsumer();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<CheckAction> X("moz-check", "check test action");

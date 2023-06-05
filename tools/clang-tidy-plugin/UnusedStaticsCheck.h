#ifndef CATA_TOOLS_CLANG_TIDY_PLUGIN_UNUSEDSTATICSCHECK_H
#define CATA_TOOLS_CLANG_TIDY_PLUGIN_UNUSEDSTATICSCHECK_H

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <unordered_set>

#include "ClangTidy.h"
#include "ClangTidyCheck.h"

namespace clang
{

namespace tidy
{
class ClangTidyContext;

namespace cata
{

class DeclarationWithRange
{
    public:
        DeclarationWithRange( const VarDecl *decl, const SourceRange range )
            : decl( decl ), range( range ) {}

        const VarDecl *decl;
        const SourceRange range;
};

class UnusedStaticsCheck : public ClangTidyCheck
{
    public:
        UnusedStaticsCheck( StringRef Name, ClangTidyContext *Context )
            : ClangTidyCheck( Name, Context ) {}
        void registerMatchers( ast_matchers::MatchFinder *Finder ) override;
        void check( const ast_matchers::MatchFinder::MatchResult &Result ) override;
        void onEndOfTranslationUnit() override;
    private:
        std::unordered_set<const VarDecl *> used_decls_;
        std::vector<DeclarationWithRange> decls_;
};

} // namespace cata
} // namespace tidy
} // namespace clang

#endif // CATA_TOOLS_CLANG_TIDY_PLUGIN_UNUSEDSTATICSCHECK_H

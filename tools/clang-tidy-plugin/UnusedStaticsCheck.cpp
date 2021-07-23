#include "UnusedStaticsCheck.h"
#include "Utils.h"
#include <unordered_map>

using namespace clang::ast_matchers;

namespace clang
{
namespace tidy
{
namespace cata
{

void UnusedStaticsCheck::registerMatchers( MatchFinder *Finder )
{
    Finder->addMatcher(
        varDecl(
            anyOf( hasParent( namespaceDecl() ), hasParent( translationUnitDecl() ) ),
            hasStaticStorageDuration()
        ).bind( "decl" ),
        this
    );
    Finder->addMatcher(
        declRefExpr( to( varDecl().bind( "decl" ) ) ).bind( "ref" ),
        this
    );
}

void UnusedStaticsCheck::check( const MatchFinder::MatchResult &Result )
{
    const VarDecl *ThisDecl = Result.Nodes.getNodeAs<VarDecl>( "decl" );
    if( !ThisDecl ) {
        return;
    }

    const DeclRefExpr *Ref = Result.Nodes.getNodeAs<DeclRefExpr>( "ref" );
    if( Ref ) {
        used_decls_.insert( ThisDecl );
    }

    const SourceManager &SM = *Result.SourceManager;

    // Ignore cases in header files
    if( !SM.isInMainFile( ThisDecl->getBeginLoc() ) ) {
        return;
    }

    // Ignore cases that are not the first declaration
    if( ThisDecl->getPreviousDecl() ) {
        return;
    }

    // Ignore cases that are not static linkage
    Linkage Lnk = ThisDecl->getFormalLinkage();
    if( Lnk != InternalLinkage && Lnk != UniqueExternalLinkage ) {
        return;
    }

    SourceLocation DeclLoc = ThisDecl->getBeginLoc();
    SourceLocation ExpansionLoc = SM.getFileLoc( DeclLoc );
    if( DeclLoc != ExpansionLoc ) {
        // We are inside a macro expansion
        return;
    }

    clang::SourceRange Range{ThisDecl->getSourceRange()};
    int SemiIndex = 1;
    bool foundSemi = false;
    while( !foundSemi ) {
        clang::SourceLocation PastEndLoc{
            ThisDecl->getSourceRange().getEnd().getLocWithOffset( SemiIndex )};
        clang::SourceRange RangeForString{PastEndLoc};
        CharSourceRange CSR = Lexer::makeFileCharRange(
                                  CharSourceRange::getTokenRange( RangeForString ), *Result.SourceManager,
                                  Result.Context->getLangOpts() );
        std::string possibleSemi =
            Lexer::getSourceText( CSR, *Result.SourceManager,
                                  Result.Context->getLangOpts() )
            .str();
        if( possibleSemi == ";" ) {
            // Found a ";" so expand the range for our fixit below.
            Range.setEnd( PastEndLoc );
            foundSemi = true;
        } else {
            SemiIndex++;
        }
    }

    decls_.push_back( DeclarationWithRange( ThisDecl, Range ) );
}

void UnusedStaticsCheck::onEndOfTranslationUnit()
{
    for( const DeclarationWithRange &V : decls_ ) {
        if( used_decls_.count( V.decl ) ) {
            continue;
        }

        diag( V.range.getBegin(), "Variable %0 declared but not used." ) << V.decl
                << FixItHint::CreateRemoval( V.range );
    }
}

} // namespace cata
} // namespace tidy
} // namespace clang

#pragma once
#ifndef CATA_SRC_MAPSHARING_H
#define CATA_SRC_MAPSHARING_H

#include <set>
#include <string>

namespace MAP_SHARING
{
extern bool sharing;
extern std::string username;

extern bool competitive;
extern bool worldmenu;

void setSharing( bool mode );
void setUsername( const std::string &name );
auto isSharing() -> bool;
auto getUsername() -> std::string;

void setCompetitive( bool mode );
auto isCompetitive() -> bool;

void setWorldmenu( bool mode );
auto isWorldmenu() -> bool;

extern std::set<std::string> admins;
auto isAdmin() -> bool;

void setAdmins( const std::set<std::string> &names );
void addAdmin( const std::string &name );

extern std::set<std::string> debuggers;
auto isDebugger() -> bool;

void setDebuggers( const std::set<std::string> &names );
void addDebugger( const std::string &name );

void setDefaults();
} // namespace MAP_SHARING

#endif // CATA_SRC_MAPSHARING_H

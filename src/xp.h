#pragma once
#ifndef CATA_SRC_XP_H
#define CATA_SRC_XP_H

#include "skill.h"
#include "type_id.h"

class avatar;
class Character;

namespace xp
{

int total( const Character &c );
int available( const Character &c );
int spent( const Character &c );
int for_skill_set( const std::map<skill_id, SkillLevel> skills );
int to_raise_skill( const Character &c, const skill_id &skill );

} // namespace xp

#endif // CATA_SRC_XP_H

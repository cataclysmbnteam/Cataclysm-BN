#pragma once
#ifndef CATA_SRC_GAMEMODE_TUTORIAL_H
#define CATA_SRC_GAMEMODE_TUTORIAL_H

#include <functional>
#include <iosfwd>
#include <map>
#include <unordered_set>

#include "enums.h"
#include "gamemode.h"
#include "type_id.h"

enum action_id : int;

struct tutorial_game : public special_game {
        special_game_type id() override {
            return special_game_type::TUTORIAL;
        }
        bool init() override;
        void per_turn() override;
        void pre_action( action_id &act ) override;
        void post_action( action_id act ) override;
        void game_over() override { }

    private:
        void update_tutorial_msg();
        void add_message( const snippet_id &lesson_id );

        std::unordered_set<snippet_id> tutorials_seen_new;
};

#endif // CATA_SRC_GAMEMODE_TUTORIAL_H

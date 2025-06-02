#include <vector>
#include <string>
#include <ranges>
#include <algorithm>
#include "catalua_bindings.h"

#include "catalua.h"
#include "catalua_bindings_utils.h"
#include "catalua_impl.h"
#include "catalua_log.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "recipe.h"
#include "recipe_dictionary.h"

#include "itype.h"
#include "skill.h"

void cata::detail::reg_recipe( sol::state &lua )
{
#define UT_CLASS recipe
    {
        sol::usertype<UT_CLASS> ut =
        luna::new_usertype<UT_CLASS>(
            lua,
            luna::no_bases,
            luna::no_constructor
        );
        SET_MEMB( category );
        SET_MEMB( subcategory );
        luna::set( ut, "time", &recipe::time );
        SET_MEMB( skill_used );
        SET_MEMB( difficulty );
        SET_MEMB( required_skills );
        SET_MEMB( learn_by_disassembly );
        SET_MEMB( booksets );

        SET_FX_T( ident, const recipe_id & () const );
        SET_FX_T( result, const itype_id & () const );
        SET_FX_T( result_name, std::string() const );
        SET_FX_T( has_flag, bool ( const std::string & flag_name ) const );

        luna::set_fx( ut, "get_from_skill_used", []( const skill_id & sk ) -> std::vector<recipe> {
            auto filtered = recipe_dict
            | std::views::values
            | std::views::filter( [&]( const recipe & r )
            {
                return r.skill_used == sk;
            } );
            return { filtered.begin(), filtered.end() };
        } );
        luna::set_fx( ut, "get_from_flag", []( const std::string & flag_name ) -> std::vector<recipe> {
            auto filtered = recipe_dict
            | std::views::values
            | std::views::filter( [&]( const recipe & r )
            {
                return r.has_flag( flag_name );
            } );
            return { filtered.begin(), filtered.end() };
        } );
        luna::set_fx( ut, "get_all", []() -> std::vector<recipe> {
            auto all = recipe_dict | std::views::values;
            return { all.begin(), all.end() };
        } );
    }
}
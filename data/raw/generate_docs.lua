local fmt_arg_list = function(arg_list)
    local ret = ""
    if #arg_list == 0 then
        return ret
    end
    local is_first = true
    for _, arg in pairs(arg_list) do
        if not is_first then
            ret=ret..","
        end
        ret=ret.." "..arg
        is_first = false
    end
    return ret.." "
end

local fmt_one_constructor = function(typename, ctor)
    return typename..".new("..fmt_arg_list(ctor)..")"
end

local fmt_constructors = function(typename, ctors)
    if #ctors == 0 then
        return "No constructors.\n"
    else
        local ret = ""
        for k,v in pairs(ctors) do
            ret=ret..tostring(k).." : "..fmt_one_constructor(typename, v).."\n"
        end
        return ret
    end
end

local fmt_one_member = function(typename, member)
    local ret = tostring(member.name).."\n  ";
    
    if member.type == "var" then
        ret=ret.."Variable of type "..member.vartype
    elseif member.type == "func" then
        ret=ret.."Function ("..fmt_arg_list(member.args)..") -> "..member.retval
    elseif member.type == "const_func" then
        ret=ret.."Const function ("..fmt_arg_list(member.args)..") -> "..member.retval
    else
        error("Unknown member type "..tostring(member.type))
    end

    return ret.."\n"
end

local fmt_members = function(typename, members)
    if #members == 0 then
        return "No members.\n"
    else
        local ret = ""
        for k,v in pairs(members) do
            ret=ret..tostring(k).." : "..fmt_one_member(typename, v).."\n"
        end
        return ret
    end
end

local fmt_bases = function(typename, bases)
    if #bases == 0 then
        return "No base classes.\n"
    else
        local ret = ""
        for k,v in pairs(bases) do
            ret=ret..tostring(k).." : "..v.."\n"
        end
        return ret
    end
end

doc_gen_func.impl = function()
    local ret = "# Lua documentation\n\n"

    local dt = catadoc

    ret = ret.."## Types\n\n"

    local types_table = dt["#types"]

    for typename,dt_type in pairs(types_table) do
        ret = ret.."### "..typename.."\n"

        local bases = dt_type["#bases"]
        local ctors = dt_type["#construct"]
        local members = dt_type["#member"]

        ret = ret
        .."#### Bases\n"
        ..fmt_bases( typename, bases )
        .."\n"
        .."#### Constructors\n"
        ..fmt_constructors( typename, ctors )
        .."\n"
        .."#### Members\n"
        ..fmt_members( typename, members )
        .."\n"
    end

    return ret
end

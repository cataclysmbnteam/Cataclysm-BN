local fmt_one_constructor = function(typename, ctor)
    local ret = typename.."("
    if #ctor == 0 then
        ret=ret..")"
    else
        local is_first = true
        for _,arg in pairs(ctor) do
            if not is_first then
                ret=ret..","
            end
            ret=ret.." "..arg
            is_first = false
        end
        ret=ret.." )"
    end
    return ret
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

doc_gen_func.impl = function()
    local ret = "# Lua documentation\n\n"

    local dt = catadoc

    ret = ret.."## Types\n\n"

    local types_table = dt["#types"]

    for typename,dt_type in pairs(types_table) do
        ret = ret.."### "..typename.."\n"

        local ctors = dt_type["#construct"]

        ret = ret.."#### Constructors\n"..fmt_constructors( typename, ctors ).."\n"
    end

    return ret
end

--[[                                                                        ]]--
--[[                       ebiff configuration parser                       ]]--
--[[                                                                        ]]--

-- -------------------------------------------------------------------------- --
-- generate C parser
-- 
function generate_parser(name)
	local s = ""
	
	s=s..generate_preamble()
	s=s..generate_separe("bags")
	s=s..generate_bags(name)
	s=s..generate_separe("hooks")
	s=s..generate_parser_aux(name)

	return s
end
-- -------------------------------------------------------------------------- --
-- done
-- 
function generate_separe(name)
	return [[
	
/*================--==================--==================--==================
]] ..
"\t" .. string.rep(string.upper(name).." ",10) .. "\n" ..
[[
  ================--==================--==================--==================*/

]]
end
-- -------------------------------------------------------------------------- --
-- done
-- 
function generate_preamble()	
	local s = [[
/******************************************************************************/
/* Authomatically generated table importer from config.lua                    */
/*                                                                            */
/* Please do not modify by hand, since you will loose your changes.           */
/*                                                                            */
/******************************************************************************/

]]
	s = s .. "#include <string.h>\n"
	s = s .. "#include <stdio.h>\n"
	s = s .. "#include <lua.h>\n"
	s = s .. "\n"
	s = s .. "#define HIDDEN static\n"
	s = s .. "#define ERR_PRINT(s...) fprintf(stderr,s)\n"
	s = s .. "#define MALLOC_CHECK(p) {\\\n"
	s = s .. "\tif((p)==NULL) {\\\n"
	s = s .. "\t\tERR_PRINT(\"%s:%s: NULL assertion failed\\n\",\\\n"
	s = s .. "\t\t\t__FILE__,__LINE__);\\\n"
	s = s .. "\t\treturn 1;\\\n"
	s = s .. "\t}\\\n"
	s = s .. "}\n"
	s = s .. "\n"
	s = s .. "HIDDEN int _lua_istable(lua_State*L,int index){\n"
	s = s .. "\treturn lua_istable(L,index);\n"
	s = s .. "}\n"
	s = s .. "\n"
	s = s .. "HIDDEN int _lua_isboolean(lua_State*L,int index){\n"
	s = s .. "\treturn lua_isboolean(L,index);\n"
	s = s .. "}\n"
	s = s .. "\n"
	s = s .. "struct elements_t{\n"
	s = s .. "\tchar* name;\n"
	s = s .. "\tint (*check_type)(lua_State* L,int index);\n"
	s = s .. "\tint (*hook)(lua_State* L,void *data);\n"
	s = s .. "};\n"
	
	return s
end
-- -------------------------------------------------------------------------- --
-- done
-- 
function generate_bags(name,rootcast,container)
	local s = ""
	rootcast = rootcast or "struct "..name.."_bag_t"

	-- all the elements are listed here
	local t_typ = _G[name.."_types"] or 
		error("Unable to find '"..name.."_types' defs\n")
	local t_all = _G[name.."_allowed"] or 
		error("Unable to find '"..name.."_allowed' defs\n")
	
	-- generate sons bags
	for n,t in pairs(t_typ) do
		if t == "table" then
			local _,tmp = t_all[n](nil)
			for w in string.gfind(tmp,"(%w+)") do
				if container then
					s = s..generate_bags(w,rootcast,
						container.."."..name..".".. n)
				else
					s = s..generate_bags(w,rootcast,n)
				end
			end
		end
	end

	-- generate bag fields
	s=s.."\n// this is the bag for table "..name.."\n"
	for n,v in pairs(t_typ) do
		if v == "table" then
			local _,tmp = t_all[n](nil)
			s=s.."enum "..name.."_"..n.."_content_e {\n"
			for w in string.gfind(tmp,"(%w+)") do
				s = s.."\t"..string.upper(name.."_"..n..
					"_CONTENT_"..w)..",\n"
			end
			s=s.."};\n"
		end
	end
	s=s.."struct "..name.."_bag_t {\n"
	for n,v in pairs(t_typ) do
		if v == "string" then
			s=s.."\tchar* "..n..";\n"
		elseif v == "boolean" then
			s=s.."\tint "..n..";\n"
		elseif v == "number" then
			s=s.."\tdouble "..n..";\n"
		elseif v == "table" then
			s=s.."\t// space to store "..n.." son info\n"
			local _,tmp = t_all[n](nil)
			s=s.."\tenum "..name.."_"..n.."_content_e "..
				n.."_content;\n"
			s=s.."\tunion "..n.."_u {\n"			
			for w in string.gfind(tmp,"(%w+)") do
				s = s.."\t\tstruct "..w.."_bag_t "
					..w..";\n"
			end
			s=s.."\t} "..n..";\n"
		end
	end
	s=s.."};\n"
	if container then
	s=s.."#define "..string.upper(name)..
		"_CAST(p) ((*("..rootcast.."*)p)."..container.."."..name..")\n"
	else
	s=s.."#define "..string.upper(name)..
		"_CAST(p) ((*("..rootcast.."*)p))\n"
	end
	
	return string.gsub(s,",\n\t}","\n\t}")
end
-- -------------------------------------------------------------------------- --
-- done
--
function generate_parser_aux(name)
	local s = ""
	local t_typ = _G[name.."_types"] or 
		error("Unable to find '"..name.."_types' defs\n")
	local t_all = _G[name.."_allowed"] or 
		error("Unable to find '"..name.."_allowed' defs\n")


	-- sort the table for pretty printing
	table.sort(t_typ,function(a,b)
		if a ~= "table" then
			return true
		end
	end)
	
	-- generate the parser for subtables of name
	for n,v in pairs(t_typ) do
		if v == "table" then
			local _,tmp = t_all[n](nil)
			for w in string.gfind(tmp,"(%w+)") do
				s = s..generate_parser_aux(w)
			end
			s=s..generate_table_case_hook(name,n,tmp)
		end
	end

	local n = 0
	table.foreach(t_typ,function() n = n + 1 end)
	
	-- generate the parser for name
	s = s .. generate_table_elements(t_typ,name,n)
	s = s .. generate_table_hook(name,n)
	
	return s
end

function generate_table_case_hook(name,n,all)
	local s = "\n"
	
	s = s .."HIDDEN int "..name.."_"..n.."_hook(lua_State* L,void *data){\n"
	s = s .. "\tconst char* type;\n"
	s = s .. "\tint (*hook)(lua_State*,void *) = NULL;\n"
	s = s .. "\t\n"
	s = s .. "\t// get the type \n"
	s = s .. "\tlua_pushstring(L,\"type\");\n"
	s = s .. "\tlua_gettable(L,-2);\n"
	s = s .. "\ttype = lua_tostring(L,-1);\n"
	s = s .. "\tif(type == NULL){\n"
	s = s .. "\t\tERR_PRINT(\"Internal error\\n\");\n"
	s = s .. "\t\treturn 1; //failure\n"
	s = s .. "\t}\n"
	s = s .. "\t\n"
	s = s .. "\t// choose the right hook\n"
	for w in string.gfind(all,"(%w+)") do
		s = s .. "\tif(!strcmp(type,\""..w.."\")){\n"
		s = s .. "\t\thook = "..w.."_hook;\n"
		s = s .. "\t\t"..string.upper(name).."_CAST(data)."..n..
			"_content = \n"
		s = s .. "\t\t\t".. 
			string.upper(name.."_"..n.."_CONTENT_"..w)..";\n"
		s = s .. "\t\tgoto end_choose;\n"
		s = s .. "\t}\n"
	end
	s = s .. "\tERR_PRINT(\"Not valid hook for '%s'\\n\",type);\n"
	s = s .. "\treturn 1; //failure\n"
	s = s .. "\t\n"
	s = s .. "\tend_choose:\n"
	s = s .. "\t\n"
	s = s .. "\t// pop type\n"
	s = s .. "\tlua_pop(L,1);\n"
	s = s .. "\t\n"
	s = s .. "\t// call the hook\n"
	s = s .. "\treturn hook(L,data);\n"
	s = s .. "}\n"
	
	return s
end


function generate_table_elements(t_typ,name,n)
	local s = "\n"

	for n,v in pairs(t_typ) do
		if v ~= "table" then 
		s=s.."HIDDEN int "..name.."_"..n..
			"_hook(lua_State* L,void *data){\n"
		if v == "string" then
			s=s.."\tconst char * tmp;\n"
			s=s.."\ttmp = lua_tostring(L,-1);\n"
			s=s.."\tMALLOC_CHECK(tmp);\n"
			s=s.."\t"..string.upper(name).."_CAST(data)."..n..
				" = strdup(tmp);\n"
			s=s.."\tMALLOC_CHECK("..string.upper(name)..
				"_CAST(data)."..n..");\n"
		elseif v == "boolean" then
			s=s.."\t"..string.upper(name).."_CAST(data)."..n..
				" = lua_toboolean(L,-1);\n"
		elseif v == "number" then
			s=s.."\t"..string.upper(name).."_CAST(data)."..n..
				" = lua_tonumber(L,-1);\n"
		end
		s=s.."\treturn 0; //success\n"
		s=s.."}\n\n"
		end
	end

	s =s.."HIDDEN struct elements_t "..name.."_elements["..(n+2).."] = {\n"
	for n,v in pairs(t_typ) do
		s=s.."\t{"
		s=s.."\""..n.."\""
		s=s..","
		if v == "string" then
			s=s.."lua_isstring,"..name.."_"..n.."_hook"
		elseif v == "boolean" then
			s=s.."_lua_isboolean,"..name.."_"..n.."_hook"
		elseif v == "number" then
			s=s.."lua_isnumber,"..name.."_"..n.."_hook"
		elseif v == "table" then
			s=s.."_lua_istable,"..name.."_"..n.."_hook"
		end
		s=s.."},\n"
	end
	
	s=s.."};\n\n"
	
	
	
	return (string.gsub(s,"},\n};","}\n};"))
end

function generate_table_hook(name,n)
	local s = "\n"

	s=s.."HIDDEN int "..name.."_hook(lua_State* L,void *data){\n"
	s=s.."\tint i;\n\n"
	s=s.."\t// we expect the "..name.." table on top of the stack\n"
	s=s.."\tfor( i = 0 ; i < "..n.." ; i++) {\n"
	s=s.."\t\t\n"
	s=s.."\t\t// get it\n"
	s=s.."\t\tlua_pushstring(L,"..name.."_elements[i].name);\n"
	s=s.."\t\tlua_gettable(L,-2);\n\n"
	s=s.."\t\t// check type\n"
	s=s.."\t\tif(!"..name.."_elements[i].check_type(L,-1)){\n"
	s=s.."\t\t\tERR_PRINT(\"Wrong type of "..name..".%s\\n\",\n"
	s=s.."\t\t\t\t"..name.."_elements[i].name);\n"
	s=s.."\t\t\treturn 1; //failure\n"
	s=s.."\t\t}\n\n"
	s=s.."\t\t// call hook\n"
	s=s.."\t\tif("..name.."_elements[i].hook(L,data)){\n"
	s=s.."\t\t\tERR_PRINT(\"Hook "..name.."_%s_hook failed\\n\",\n"
	s=s.."\t\t\t\t"..name.."_elements[i].name);\n"
	s=s.."\t\t\treturn 1; //failure\n"
	s=s.."\t\t}\n\n"
	s=s.."\t\t// pop it\n"
	s=s.."\t\tlua_pop(L,1);\n"
	s=s.."\t}\n"
	s=s.."\t\n"
	s=s.."\treturn 0; //success\n"
	s=s.."}\n"

	return s
end



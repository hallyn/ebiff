-- -------------------------------------------------------------------------- --
-- generates human readable documentation for name prefixing each line with p
--
function generate_doc(name)
	local s = "<?xml version=\"1.0\"?>"
	
	s = s .. generate_doc_aux(name,"")
	
	return s
end

function generate_doc_aux(name,p)
p = p or ""

local s = ""

local t_typ = _G[name.."_types"] or 
	error("Unable to find '"..name.."_types' defs\n")
local t_mst = _G[name.."_must"] or 
	error("Unable to find '"..name.."_must' defs\n")
local t_def = _G[name.."_default"] or 
	error("Unable to find '"..name.."_default' defs\n")
local t_all = _G[name.."_allowed"] or 
	error("Unable to find '"..name.."_allowed' defs\n")
local t_cmm = _G[name.."_comment"] or 
	error("Unable to find '"..name.."_comment' defs\n")

local t = table.fieldsof(t_typ)
table.sort(t,function(a,b)	
	 return t_typ[a] < t_typ[b]
end)

s = s .. p .."<"..name..">\n"

for _,n in ipairs(t) do
s = s .. p .. "\t<field>\n"

-- optional
s = s .. p .. "\t\t<optional>"
if not table.valuesof(t_mst)[n] then
	s = s .. "yes"
else
	s = s .. "no"
end
s = s .. "</optional>\n"

-- name
s = s .. p .. "\t\t<name>"..n.."</name>\n"

-- type
s = s .. p .. "\t\t<type>"..t_typ[n].."</type>\n"

-- default
if t_def[n] ~= nil then
	s = s .. p .. "\t\t<default>"..tostring(t_def[n]).."</default>\n"
end

--comment
if t_cmm[n] and n ~= "type" then
	s = s .. p .. "\t\t<comment>"..t_cmm[n].."</comment>\n"
end

-- allowed
if type(t_all[n]) == "function" then
	local _,tmp = t_all[n](nil)
	local _,_,x = string.find(tmp,"(%.%.)")
	if x ~= nil then
		s = s .. p .. "\t\t<allowed type=\"range\">\n"
	else
		s = s .. p .. "\t\t<allowed type=\"list\">\n"
	end
	if t_typ[n] == "table" then
		for w in string.gfind(tmp,"(%w+)") do
			s = s .. p .. "\t\t\t<subitem>\n"
			s = s .. p .. generate_doc_aux(w,p.."\t\t\t\t")
			s = s .. p .. "\t\t\t</subitem>\n"
		end	
	else
		for w in string.gfind(tmp,"(%w+)") do
			s = s .. p .. "\t\t\t<item>"..w.."</item>\n"
		end
	end
	s = s .. p .. "\t\t</allowed>\n"
end
s = s .. p .. "\t</field>\n"
end
if t_cmm["type"] then
	s = s .. p .. "\t\t<plcomment>"..t_cmm["type"].."</plcomment>\n"
end

	return s .. p .. "</"..name..">\n"
end


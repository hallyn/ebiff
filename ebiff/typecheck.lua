-- -------------------------------------------------------------------------- --
-- checks if table t conforms to name definitions
--
function check_table(t,name)
	-- if not called with a defined type, take it from the table
	name = name or t.type

	if not name then
		return false, "Type error: check_table called with no type"..
			" and no field type in the table"
	end

	-- load the defs
	local t_typ = _G[name.."_types"] or 
		error("Unable to find '"..name.."_types' defs\n")
	local t_mst = _G[name.."_must"] or 
		error("Unable to find '"..name.."_must' defs\n")
	local t_def = _G[name.."_default"] or 
		error("Unable to find '"..name.."_default' defs\n")
	local t_all = _G[name.."_allowed"] or 
		error("Unable to find '"..name.."_allowed' defs\n")
		
	-- check for allowed fields
	for n,v in pairs(t) do
		if not t_typ[n] then
			return false,"Table '"..name.."' can't have a '"..
				n.."' field. Allowed are ["..
				table.concat(table.fieldsof(t_typ),",").."]"
		end
	end
	-- check for must
	for _,v in pairs(t_mst) do
		if not t[v] then
			return false,"Table '"..name.."' must have a '"..
				v.."' field"
		end
	end
	-- add default values
	for n,v in pairs(t_def) do
		if t[n] == nil then
			t[n] = v
		end
	end
	-- type check
	for n,v in pairs(t) do
		if type(v) ~= t_typ[n] then
			return false,"Field '"..n.."' of a '"..name..
				"' must".." have type '"..t_typ[n]..
				"' and not '"..type(v).."'"
		end
	end
	-- validity check
	for n,v in pairs(t_all) do
		if type(v) == "function" then
			local rc,err = v(t[n])
			if not rc then
				return false,err
			end
			if t_typ[n] == "table" then
				local rc,err = check_table(t[n])
				if not rc then
					return false,err
				end
			end
		else
			return false,"Malformed field "..n.." of "..
				name.."_allowed"
		end
	end
	-- finally
	return true
end


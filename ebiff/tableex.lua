-- -------------------------------------------------------------------------- --
-- returns a table that has t fields as values
--
function table.fieldsof(t)
	local x = {}
	table.foreach(t,function(n,v) 
		table.insert(x,n) 
	end)
	return x
end
-- -------------------------------------------------------------------------- --
-- returns a table that has t values as fields
--
function table.valuesof(t)
	local x = {}
	table.foreach(t,function(n,v) 
		x[v] = 1 
	end)
	return x
end


for l in io.lines() do
	_,_,m,t = string.find(l,'(%w+)%s*=%s*%"(%w+)%"')
	if t == "string" then t1 = "String"
	elseif t == "number" then t1 = "Number"
	elseif t == "boolean" then t1 = "Bool"
	end
	
	cast = "XOSD"
	
	print("n->Set"..t1.."(\""..m.."\","..cast.."_CAST(&b)."..m..");")
end

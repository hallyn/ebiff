-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --
-- This file contains some helper functions you can use in your
-- configuration file and some examples of plugins configuration.
-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --



-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --
--          SOME NOTIONS ABOUT LUA, THE LANGUAGE USED IN THIS FILE            --
-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --

--
-- [*] notational equivalences [*]
--
-- The following sentences are the same for LUA
-- 
-- 1) a = { b="3" , c=3 , d=true }
-- 2) a = { b='3' , c=3 , d=true , }
-- 3) a = { b="3" ; c=3 ; d=true ; }
-- 4) a = { b='3' ; c=3 ; d=true  }
-- 5) a = {}
--    a.b = "3"
--    a.c = 3
--    a.d = true
-- 
-- 1) a.b = 3
-- 2) a["b"] = 3
--
-- 1) a = { b = "3" }
-- 2) a = { ["b"] = "3" }
-- 
-- 
-- [*] notation validity [*]
-- 
-- Some of them are useful if the name contains spaces or non alphanumeric
-- characters
--
-- 1-invalid) a.lp-forum = "3"
-- 1-valid)   a["lp-forum"] = "3"
-- 2-invalid) c = { hi man = true }
-- 2-valid)   c = { ["hi man"] = true }
--



-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --
--        SOME USEFUL FUNCTIONS YOU CAN COPY IN YOUR CONFIGURATION FILE
-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --

-- -------------------------------------------------------------------------- --
-- Maildir helper function
-- 
-- This function uses the posix lua libs to traverse the filesystem.
-- You can find some documentation about posix for lua here
--    http://www.soho-one.com.br/software/poslib/pos_posix.html
--
-- Gets the maildirs that are subdirectories of dir and returns a table
-- containing them
-- 
-- Example: find_maildir(os.getenv("HOME").."/Mail")
-- 
function find_maildir(dir)
	return find_maildir_real(dir,true)
end

function find_maildir_r(dir)
	return find_maildir_real(dir,false)
end	

function find_maildir_real(dir,notdeep)
	
	local function find_maildir_aux(dir,rc)
		--print("Examining "..dir)
		local t = posix.dir(dir)
	
		if t == nil then
			return
		end
		for _,d in ipairs(t) do
			if d ~= "." and d ~= ".." then
				-- chek if it is a maildir
				local t1 = posix.dir(dir.."/"..d)
				local t2 = table.valuesof(t1 or {})
				
				if t2["cur"] ~= nil and
				   t2["new"] ~= nil and
				   t2["tmp"] ~= nil then
				   	--print("Got "..dir.."/"..d)
					table.insert(rc,dir.."/"..d)
				elseif not notdeep then
					find_maildir_aux(dir.."/"..d,rc)
				end
			end
		end
		return rc
	end

	return find_maildir_aux(dir,{})
end

-- -------------------------------------------------------------------------- --
-- Mboxr helper function
-- 
-- This function uses the posix lua libs to traverse the filesystem.
-- You can find some documentation about posix for lua here
--    http://www.soho-one.com.br/software/poslib/pos_posix.html
--
-- Gets the files (it is up to you to specify a directory containing only 
-- directoryes or mboxmailboxes) and returns a table containing them
-- 
-- Example: find_mbox(os.getenv("HOME").."/Mail")
-- 
function find_mbox(dir)
	return find_mbox_real(dir,true)
end

function find_mbox_r(dir)
	return find_mbox_real(dir,false)
end

function find_mbox_real(dir,notdeep)
	
	local function find_maildir_aux(dir,rc)
		--print("Examining "..dir)
		local t = posix.dir(dir)
	
		if t == nil then
			return
		end
		
		for _,d in ipairs(t) do
			local _,_,x = string.find(d,"^(%.)")
			if x == nil and posix.dir(dir.."/"..d) == nil then
				table.insert(rc,dir.."/"..d)
			elseif not notdeep then
				find_maildir_aux(dir.."/"..d,rc)
			end
		end
		return rc
	end

	return find_maildir_aux(dir,{})
end

-- -------------------------------------------------------------------------- --
-- Maildir helper function
-- 
--
-- Gets maildir path and creates a mailbox.
-- 
-- s : is the maildir path
-- transtbl : a mab between names, for example 
-- 	{ ["really long mailbox name"] = "short name" }
-- 	I use {["securityfocus"] = "secfoc", ["sent-mail"] = "sent" }
-- cmdgen : is a command generator function, I use
-- 	function(s) return "xterm -e mutt -f "..s.."&" end
-- interval: the interval in seconds
-- 	
-- transtbl and cmdgen can be omitted, they default value is the empty map
-- function(s) return "xterm -e mutt -f "..s end
-- 	
-- Example: 
-- 	maildir2mailbox("/home/me/Mail/inbox"),
-- 		{["securityfocus"] = "secfoc", ["sent-mail"] = "sent" },
-- 		function(s) return "xterm -e mutt -f "..s.."&" end)
-- 
--
function maildir2mailbox(s,transtbl,cmdgen,interval)

	if s == nil then return nil end

	transtbl = transtbl or {}
	cmdgen = cmdgen or function(g) return "xterm -e mutt -f "..g end

	local function strip_path(s)
		local _,_,x = string.find(s,".*/([%w%-%_]+)")
		return x or "--failed--"
	end

	
	local name = strip_path(s)
		
	if transtbl[name] ~= nil then
		name = transtbl[name]
	end
		
	local f = new {
		type = "mailbox";
		command = cmdgen(s);
		name = name;
		driver={
			type="maildir";
			path=s;
		};
		interval=interval;
		
	}

	return f
end

-- -------------------------------------------------------------------------- --
-- Maildir helper function
-- 
--
-- Gets a table of maildir path and creates a table of mailboxes.
-- 
-- t : is the table containing maildirs path
-- transtbl : a mab between names, for example 
-- 	{ ["really long mailbox name"] = "short name" }
-- 	I use {["securityfocus"] = "secfoc", ["sent-mail"] = "sent" }
-- cmdgen : is a command generator function, I use
-- 	function(s) return "xterm -e mutt -f "..s.."&" end
-- interval: the interval in seconds (the same for all)
-- 	
-- transtbl and cmdgen can be omitted, they default value is the empty map
-- function(s) return "xterm -e mutt -f "..s end
-- 	
-- Example: 
-- 	maildirs2mailboxes(find_maildir(os.getenv("HOME").."/Mail"),
-- 		{["securityfocus"] = "secfoc", ["sent-mail"] = "sent" },
-- 		function(s) return "xterm -e mutt -f "..s.."&" end)
-- 
--
function maildirs2mailboxes(t,transtbl,cmdgen,interval)
	local rc = {}

	if t == nil then
		return nil
	end
		
	for _,d in ipairs(t) do
		table.insert(rc,maildir2mailbox(d,transtbl,cmdgen,interval))
	end

	return rc
end
-- -------------------------------------------------------------------------- --
-- Mbox helper function
-- 
--
-- Gets a mbox path and creates a mailbox.
-- 
-- s : is the mbox path
-- newonly: list only new messages?
-- transtbl : a mab between names, for example 
-- 	{ ["really long mailbox name"] = "short name" }
-- 	I use {["securityfocus"] = "secfoc", ["sent-mail"] = "sent" }
-- cmdgen : is a command generator function, I use
-- 	function(s) return "xterm -e mutt -f "..s.."&" end
-- interval: the interval in seconds
-- 	
-- transtbl and cmdgen can be omitted, they default value is the empty map
-- function(s) return "xterm -e mutt -f "..s end
-- 	
-- Example: 
-- 	mbox2mailbox("/home/me/mbox"),
-- 		{["mbox"] = "mailbox"},
-- 		function(s) return "xterm -e mutt -f "..s.."&" end)
-- 
--
function mbox2mailbox(s,newonly,transtbl,cmdgen,interval)

	if s == nil or newonly == nil then return nil end

	transtbl = transtbl or {}
	cmdgen = cmdgen or function(g) return "xterm -e mutt -f "..g end

	local function strip_path(s)
		local _,_,x = string.find(s,".*/([%w%-%_]+)")
		return x or "--failed--"
	end

	
	local name = strip_path(s)
		
	if transtbl[name] ~= nil then
		name = transtbl[name]
	end
		
	local f = new {
		type = "mailbox";
		command = cmdgen(s);
		name = name;
		driver={
			type="mbox";
			path=s;
			newonly=newonly;
		};	
		interval=interval;		
	}

	return f
end

-- -------------------------------------------------------------------------- --
-- Mbox helper function
-- 
--
-- Gets a table of maildir path and creates a table of mailboxes.
-- 
-- t : is the table containing maildirs path
-- newonly: list only new messages?
-- transtbl : a mab between names, for example 
-- 	{ ["really long mailbox name"] = "short name" }
-- 	I use {["securityfocus"] = "secfoc", ["sent-mail"] = "sent" }
-- cmdgen : is a command generator function, I use
-- 	function(s) return "xterm -e mutt -f "..s.."&" end
-- interval: the interval in seconds (the same for all)
-- 	
-- transtbl and cmdgen can be omitted, they default value is the empty map
-- function(s) return "xterm -e mutt -f "..s end
-- 	
-- Example: 
-- 	maildirs2mailboxes(find_maildir(os.getenv("HOME").."/Mail"),
-- 		{["securityfocus"] = "secfoc", ["sent-mail"] = "sent" },
-- 		function(s) return "xterm -e mutt -f "..s.."&" end)
-- 
--
function mboxes2mailboxes(t,transtbl,cmdgen,newonly,interval)
	local rc = {}

	if t == nil then
		return nil
	end
		
	for _,d in ipairs(t) do
		table.insert(rc,mbox2mailbox(d,newonly,
			transtbl,cmdgen,interval))
	end

	return rc
end


-- -------------------------------------------------------------------------- --
-- Mailbox helper function
-- 
-- Returns the mailbox that has the specifyed name
--
-- If you create a table of mailboxes, for example with maildirs2mailboxes,
-- you may bind a notifier to only one of them.
-- 
-- You could even create 2 mailbox pointing to the same path, but this would
-- use more memory... so this function is not so bad...
-- 
function selectmailbox(t,name)

	for _,m in ipairs(t) do
		if m.name == name then
			return m
		end
		--print("failed "..name.." with "..m.name)
	end

	error("Unable to find mailbox "..name)
	return nil
end

-- -------------------------------------------------------------------------- --
-- debug helper function
function print_table(t)
	table.foreach(t,print)
end
-- -------------------------------------------------------------------------- --
-- debug helper function
function print_mailbox(t)
	local function print_box(b)
		table.foreach(b,print)
		print("---------------------------")
	end

	table.foreach(t,print_box)
end

-- eof

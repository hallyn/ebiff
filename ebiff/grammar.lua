local grammar = {}
grammar.__id = 0

-- -------------------------------------------------------------------------- --
-- creates the map readed by ebiff
--
function bind(b,n)--,i)
	if b == nil or n == nil then
		error("Calling bind on an nil value")
	end

	if b.type then return bind({b},n)end--,i) end
	if n.type then return bind(b,{n})end--,i) end
		
	local rc,err

	-- sanity check
	for _,m in ipairs(b) do
		rc,err = check_table(m,"mailbox")
		if not rc then error(err) end
	end
	for _,m in ipairs(n) do
		rc,err = check_table(m,"notifier")
		if not rc then error(err) end
	end

	-- create the relation table
	for _,m in ipairs(b) do
		for _,v in ipairs(n) do
			local tmp = {mailbox=m,
				notifier=v,
				type="relation"
				--,interval=i
				}
			local rc,err = check_table(tmp,relation)
			if not rc then error(err) end
			table.insert(Relation_Table,tmp);
		end
	end
	
end

-- -------------------------------------------------------------------------- --
-- creates a new object
--
function new(t)
	t.id = grammar.__id
	grammar.__id = grammar.__id + 1 
	return t
end



Relation_Table = {}

-- -------------------------------------------------------------------------- --
-- creates a check function for being in a set of elements
--
function onestrof(...)
	return function(s)
		if not s then
			return false,"["..table.concat(arg,",").."]"
		end
		for i,v in pairs(arg) do
			if v == s then
				return true
			end
		end
		return false,"Value error: '"..s.."' is not one of ["..
			table.concat(arg,",").."]"
	end
end

function onetblof(...)
	return function(t)
		if not t then
			return false,"<"..table.concat(arg,",")..">"
		end
		for i,v in pairs(arg) do
			if v == t.type then
				return true
			end
		end
		return false,"Table error: '"..(t.type or "no-type")..
			"' is not one of these types <"..
			table.concat(arg,",")..">"
	end
end

-- -------------------------------------------------------------------------- --
-- creates a check function for a nember being in a range, edges included
--
function onenumof(a,b)
	return function(i)
		if not i then
			return false,"["..a..".."..b.."]"
		end
		if i >= a and i <= b then
			return true
		else
			return false,"Range error: '"..i.."' is not in ["..
				a..".."..b.."]"
		end
	end
end


--<==========================================================================>--

-- -------------------------------------------------------------------------- --
-- mailbox definitions
--
mailbox_types   = {
	type="string",
	driver="table",
	name="string",
	id="number",
	command="string",
	interval="number"}
mailbox_must	= {"type","driver","id"}
mailbox_default = {name="-no-name-",command="/bin/true",interval=10}
mailbox_allowed = {
	type=onestrof("mailbox"),
	driver=onetblof("maildir","mbox","netin"),
	interval=onenumof(1,3600)}
mailbox_comment={
	id="Forget this, it is automatically added by new()",
	type="A way to access a mailbox",
	interval="Delay between two mailbox inspection, in seconds",
	command="The command to execute (may be requested by the notifier plugin to show the content of the mailbox, for example 'mutt -f mbox')",
	name="The name that will be used by the notifier as the mailbox identifyer"
	}	
	-- ------------------------------------------------------------------ --
	-- mailbox::maildir definitions
	--
	maildir_types = {type="string",path="string"}
	maildir_must = {"type","path"}
	maildir_default = {}
	maildir_allowed = {type=onestrof("maildir")}
	maildir_comment={
		path="You should know this :)",
		type="Maildir type mailbox, usually ~/Mail/ is a maildir"}
	-- ------------------------------------------------------------------ --
	-- mailbox::maildir definitions
	--
	netin_types = {type="string",host="string",port="number",pass="string"}
	netin_must = {"type","host","pass"}
	netin_default = {port=6758}
	netin_allowed = {type=onestrof("netin"),port=onenumof(1,65535)}
	netin_comment={
		type="Network forward plugin",
		host="The host to connect to",
		pass="The netout plugin pass",
		port="The port to connect to"
		}
	
	-- ------------------------------------------------------------------ --
	-- mailbox::mbox definitions
	--
	mbox_types = {type="string",path="string",newonly="boolean"}
	mbox_must = {"type","path"}
	mbox_default = {newonly=true}
	mbox_allowed = {type=onestrof("mbox")}
	mbox_comment={
		path="You should know this :)",
		newonly="List only new messages",
		type="Mbox type mailbox, usually ~/mbox is an mbox"}
-- -------------------------------------------------------------------------- --
-- notifier definitions
--
notifier_types = { type="string",driver="table",id="number"}
notifier_must = {"type","driver","id"}
notifier_default = {}
notifier_allowed = {
	type=onestrof("notifier"),
	driver=onetblof("stdout","gtk2","xosd","flite","sox","utmp","netout","wm")}
notifier_comment={
	id="Forget this, it is automatically added by new()",
	type="A way to notify the user"
	}	
	-- ------------------------------------------------------------------ --
	-- notifier::stdout definitions
	--
	stdout_types = { type="string",printall="boolean"}
	stdout_must = {"type"}
	stdout_default = {printall=false}
	stdout_allowed = {type=onestrof("stdout")}
	stdout_comment={
		printall="Print From and Subject info of mails",
		type="Simple print on stdout"
		}
	-- ------------------------------------------------------------------ --
	-- notifier::stdout definitions
	--
	netout_types = { type="string",port="number",pass="string"}
	netout_must = {"type","pass"}
	netout_default = {port=6758}
	netout_allowed = {type=onestrof("netout"),port=onenumof(1,65535)}
	netout_comment={
		type="Network forward plugin",
		port="The port to listen on",
		pass="The password"
		}
		
	-- ------------------------------------------------------------------ --
	-- notifier::utmp definitions
	--
	utmp_types = { type="string",printall="boolean"}
	utmp_must = {"type"}
	utmp_default = {printall=false}
	utmp_allowed = {type=onestrof("utmp")}
	utmp_comment={
		printall="Print From and Subject info of mails",
		type="Simple broadcast-print on all users utmp-registered terminals (normal tty or pts (xterm) are ok)"
		}
	
	-- ------------------------------------------------------------------ --
	-- notifier::sox definitions
	--
	sox_types = { type="string",file="string",file_empty="string"}
	sox_must = {"type","file"}
	sox_default = {file_empty=""}
	sox_allowed = {type=onestrof("sox")}
	sox_comment={
		file="The wav file to play",
		file_empty="The sound to play whe the mailbox became empty, if omitted no soud will be played",
		type="Plays a sound to notify"
		}
	
	-- ------------------------------------------------------------------ --
	-- notifier::flite definitions
	--
	flite_types = { type="string",one="string",zero="string",more="string"}
	flite_must = {"type"}
	flite_default = {
		one="There is $n$ new mail in box $b$",
		zero="No new mail in box $b$",
		more="There are $n$ mails in box $b$"}
	flite_allowed = {type=onestrof("flite")}
	flite_comment={	
		one="The phrase for 1 mail, $b$ is expanded in the mailbox name, $n$ in the number of mails",
		zero="The phrase for no mail, same special characters of one",
		more="The phrase for 2+ mails, same special characters of one",
		type="Voice-synthesize plugin"}
	
	-- ------------------------------------------------------------------ --
	-- notifier::gtk2 definitions
	--
	gtk2_types = { type="string",
		position="string",
		showall="boolean",
		preview="boolean"}
	gtk2_must = {"type"}
	gtk2_default = {position="topleft",showall=true,preview=true}
	gtk2_allowed = {type=onestrof("gtk2"),
		position=onestrof("managed","top","bottom","left","right",
			"topleft","topright","bottomleft","bottomright")}
	gtk2_comment={
		position="Where to put the window",
		showall="Not hide the empty boxes",
		preview="Show a button to show the mailbox preview",
		type="Gbuffy like plugin"}
	-- ------------------------------------------------------------------ --
	-- notifier::xosd definitions
	--
	xosd_types = { type="string",
		pos="string",
		align="string",
		voffset="number",
		hoffset="number",
		color="string",
		shoadowoffset="number",
		outlineoffset="number",
		outlinecolor="string",
		font="string",
		timeout="number"}
	xosd_must = {"type"}
	xosd_default = {
		pos="middle",
		align="center",
		voffset=0,
		hoffset=0,
		color="#FFFF00",
		shoadowoffset=0,
		outlineoffset=1,
		outlinecolor="MediumBlue",
		font="-*-freesans-*-r-*-*-32-*-*-*-*-*-*-*",
		timeout=5}
	xosd_allowed = {type=onestrof("xosd"),
		pos=onestrof("top","middle","bottom"),
		align=onestrof("left","center","right"),
		voffset=onenumof(0,9999),
		hoffset=onenumof(0,9999),
		shoadowoffset=onenumof(0,10),
		outlineoffset=onenumof(0,3),
		timeout=onenumof(0,60),
		}
	xosd_comment = {
	pos="Vertical position",
	align="Horizontal position",
	voffset="Move some pixels from position",
	hoffset="Move some pixels from position",
	color="Font color, see http://www.yo.rim.or.jp/~kageyama/xrgb.html a list",
	shoadowoffset="Shadow dimension",
	outlineoffset="Border dimension",
	outlinecolor="Font border color",
	font="The font in X format (es: -*-freesans-*-r-*-*-32-*-*-*-*-*-*-*)",
	timeout="Seconds before hiding notice",
	type="Xosd is X on screen display"}

	-- ------------------------------------------------------------------ --
	-- notifier::wm definitions
	--
	wm_types = { type="string",command="string" }
	wm_must = {"type"}
	wm_default = {command=""}
	wm_allowed = {type=onestrof("wm")}
	wm_comment={
		command="The shell command to be executed when clicked"
		}
	
--<==========================================================================>--

relation_types = {mailbox="table",
		notifier="table",
		type="string"}
relation_must = {"mailbox","notifier","type"}
relation_default = {}
relation_allowed = {mailbox=onetblof("mailbox"),notifier=onetblof("notifier"),
	type=onestrof("relation")}
relation_comment={}

-- eof

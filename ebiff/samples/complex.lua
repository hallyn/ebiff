-- this file requires the functions.lua file that defines useful functions
dofile("samples/functions.lua")

-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --
--                  CONFIGURATION FILE EXAMPLE                                --
--
-- Note that this example needs some of the previously defined functions.
-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --

-- -------------------------------------------------------------------------- --
-- create a gtk2 notifier
--

gtk2 = new {
	type="notifier";
	driver={
		type="gtk2";
		preview=true;
		showall=true;
		position="left";
		};
}

gtk2_man = new {
	type="notifier";
	driver={
		type="gtk2";
		preview=false;
		showall=false;
		position="managed";
		};
}

gtk2_top = new {
	type="notifier";
	driver={
		type="gtk2";
		preview=true;
		showall=false;
		position="top";
		};
}

-- -------------------------------------------------------------------------- --
-- create a xosd notifier
--

xosd = new {
	type="notifier";
	driver={
		type="xosd";
		outlineoffset=0;
		shoadowoffset=3;
		color="#00FF00";
		timeout=60;
		align="right";
		pos="bottom";
		voffset=100;
		};
}

-- -------------------------------------------------------------------------- --
-- create a sox notifier
--

sox = new {
	type="notifier";
	driver={
		type="sox";
		file="/usr/share/licq/sounds/fun/Online.wav";
		};
}


-- -------------------------------------------------------------------------- --
-- create the list of all the ~/Mail maildirs
--

mailboxes = maildirs2mailboxes(find_maildir( os.getenv("HOME") .. "/Mail" ),
	{	["securityfocus"] = "secfoc", 
		["sent-mail"] = "sent",
		["sourceforge"] = "sf.net",
		["lp-forum"] = "lp-frm"},
	function(s) return "xterm -e mutt -f "..s.."&" end, 10)

-- -------------------------------------------------------------------------- --
-- create the list of all the ~/Mail mboxes 
--
--
--mailboxes = mboxs2mailboxes(find_mbox( os.getenv("HOME") .. "/Mail" ),
--	{}, function(s) return "xterm -e mutt -f "..s.."&" end, true, 10)

-- -------------------------------------------------------------------------- --
-- create a mailbox for ~/mbox
-- 
mbox = new {
	type= "mailbox";
	driver={
		type="mbox";
		path=os.getenv("HOME").."/mbox";
		newonly=false;
	};
	name="mbox";
	command="xterm -e mutt -f "..os.getenv("HOME").."/mbox";
	interval=60;
}

-- this is equivalent
-- 
-- mbox = mbox2mailbox(os.getenv("HOME").."/mbox",false,{},function f(s) 
--	"xterm -e mutt -f "..os.getenv("HOME").."/mbox" 
--	end)

-- -------------------------------------------------------------------------- --
-- bind all the mailboxes with the gtk2 notifier.
--

bind(mailboxes , gtk2)
bind(mailboxes , gtk2_man)
bind(mailboxes , gtk2_top)
bind(mbox , gtk2) 
bind(mbox , sox) 

-- -------------------------------------------------------------------------- --
-- bind "amici" maildir with the xosd notifier.
--

bind(selectmailbox(mailboxes,"inbox"),xosd)

-- eof

-- this file requires the functions.lua file that defines useful functions
dofile("samples/functions.lua")

-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --
--                  CONFIGURATION FILE EXAMPLE                                --
--
-- Note that this example needs some of the previously defined functions.
-- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == --

-- -------------------------------------------------------------------------- --
-- create a gtk2 notifyer
--

gtk2 = new {
	type="notifyer";
	driver={
		type="gtk2";
		preview=true;
		showall=true;
		position="left";
		};
}

-- -------------------------------------------------------------------------- --
-- create a xosd notifyer
--

xosd = new {
	type="notifyer";
	driver={
		type="xosd";
		outlineoffset=0;
		shoadowoffset=3;
		timeout=7;
		};
}

-- -------------------------------------------------------------------------- --
-- create a sox notifyer
--

sox = new {
	type="notifyer";
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
-- bind all the mailboxes with the gtk2 notifyer.
--

bind(mailboxes , gtk2)
bind(mbox , gtk2) 
bind(mbox , sox) 

-- -------------------------------------------------------------------------- --
-- bind "amici" maildir with the xosd notifyer.
--

bind(selectmailbox(mailboxes,"inbox"),xosd)

-- eof

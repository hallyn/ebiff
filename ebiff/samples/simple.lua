-- get the home of the user
home = os.getenv("HOME")

-- create a mbox for ~/mbox
what_a_mailbox = new {
	type= "mailbox";
        driver={
                type="mbox";
                path=home.."/mbox";
                newonly=false;
        };
        name="mbox";
        command="xterm -e mutt -f "..home.."/mbox&";
        interval=5;
}

-- create the gtk2 notifyer
notifyer_1 = new {
	type="notifyer";
        driver={
                type="gtk2";
                position="managed";
                };
}

-- create the xosd notifyer
notifyer_2 = new {
	type="notifyer";
        driver={
                type="xosd";
                };
}


-- bind them together
bind(what_a_mailbox,notifyer_1)
bind(what_a_mailbox,notifyer_2)

-- this line is the same as the 2 before
--bind(what_a_mailbox,{notifyer_1,notifyer_2})


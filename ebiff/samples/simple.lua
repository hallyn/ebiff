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

-- create the gtk2 notifier
notifier_1 = new {
	type="notifier";
        driver={
                type="gtk2";
                position="managed";
                };
}

-- create the xosd notifier
notifier_2 = new {
	type="notifier";
        driver={
                type="xosd";
                };
}


-- bind them together
bind(what_a_mailbox,notifier_1)
bind(what_a_mailbox,notifier_2)

-- this line is the same as the 2 before
--bind(what_a_mailbox,{notifier_1,notifier_2})


-- create a mbox for ~/mbox
what_a_mailbox = new {
	type= "mailbox";
        driver={
                type="mbox";
                path="/home/me/mbox";
                newonly=false;
        };
        name="mbox";
        command="xterm -e mutt -f /home/me/mbox&";
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

-- bind them together
bind(what_a_mailbox,notifyer_1)

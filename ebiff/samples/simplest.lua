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

-- create the gtk2 notifier
notifier_1 = new {
	type="notifier";
        driver={
                type="gtk2";
                position="managed";
                };
}

-- bind them together
bind(what_a_mailbox,notifier_1)

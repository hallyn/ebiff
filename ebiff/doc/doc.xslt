<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="name">
field <FONT color="blue"><xsl:value-of select="."/></FONT>
</xsl:template>

<xsl:template match="type">
of type <FONT color="green"><xsl:value-of select="."/></FONT>
</xsl:template>

<xsl:template match="comment">
<BR/><I style="margin-left:20px;"><xsl:value-of select="."/></I>
</xsl:template>

<xsl:template match="optional">
<xsl:if test="text() = 'yes'">
is optional</xsl:if>
<xsl:if test="text() = 'no'">
is required</xsl:if></xsl:template>

<xsl:template match="default">, default value is <FONT color="red"><xsl:value-of select="."/></FONT></xsl:template>

<xsl:template match="field">
  <li>
     <xsl:apply-templates select="name"/>
     <xsl:apply-templates select="type"/>
     <xsl:apply-templates select="optional"/>
     <xsl:apply-templates select="default"/>
     <xsl:apply-templates select="allowed"/>
     <xsl:apply-templates select="comment"/>
  </li>
</xsl:template>

<xsl:template match="plcomment">
<I> ( <xsl:value-of select="."/> ) </I>
</xsl:template>

<xsl:template match="item">
<FONT color="red"><xsl:value-of select="."/></FONT>
<xsl:if test="position() &lt; last() ">, </xsl:if>
</xsl:template>

<xsl:template match="subitem">
<li>
  <DIV>
    <xsl:attribute name="ID">
      <xsl:value-of select="name(*)"/>
    </xsl:attribute>
    Table <B><xsl:value-of select="name(*)"/></B>
    <xsl:apply-templates select="*/plcomment"/>
    has these fields:</DIV>
  <ul>
  <xsl:apply-templates select="*/field[optional/text()='no']"/>
  <xsl:apply-templates select="*/field[optional/text()='yes']"/>

  </ul>
</li>
</xsl:template>

<xsl:template match="allowed">
 <xsl:if test="count(*) &gt; 1">, allowed values are </xsl:if>
 <xsl:if test="count(*) &lt; 2">, allowed value is </xsl:if>
 <xsl:if test="@type = 'range'">in range </xsl:if>
  <xsl:apply-templates select="item"/>
  <xsl:for-each select="subitem/*">
    <FONT color="DarkViolet"><xsl:value-of select="name(.)"/></FONT>
    <xsl:if test="position() &lt; last() ">, </xsl:if> 
  </xsl:for-each>
  <xsl:if test="name(*[1]) = 'subitem'">
    <ul>
      <xsl:apply-templates select="subitem"/>
    </ul>
  </xsl:if>
</xsl:template>

<xsl:template match="relation">
  <ul>
  <xsl:apply-templates select="field[optional/text()='no']"/>
  <xsl:apply-templates select="field[optional/text()='yes']"/>
  </ul>
</xsl:template>

<xsl:template match="/">
  <html>
  <head>
  <title>ebiff configuration file documentation</title>
  <style type="text/css">
  TT {font-size:12pt;font-family:courier;}
  I {font-size:12pt;font-family:helvetica;}
  PRE {font-size:12pt;font-family:courier;
  	border-style:solid;border-size:1;border-color:grey;
	margin:10 10 10 10;padding:10 10 10 10;}
  BODY {font-size:14pt;}
  UL {list-style-type:disc;}
  UL UL {list-style-type:square;}
  UL UL UL {list-style-type:disc;}
  UL UL UL UL {list-style-type:square;}
  UL UL UL UL UL {list-style-type:disc;}
  H1 {text-align: center;}
  A  {color:DarkViolet;text-decoration: none}
  A:hover {color: Violet;background-color: DarkViolet;text-decoration: none;}
  A.nohover:hover {background-color: transparent;text-decoration: none;
  	color: transparent;}
  A.normal  {color:Blue;text-decoration: none}
  A.normal:hover {color: LightBlue;background-color: Blue;text-decoration: none;}
  A.normal.nohover:hover {background-color: transparent;text-decoration: none;
  	color: transparent;}
  </style>
  </head>
  <body>
    <h1>Configuration file documentation</h1>
    The only purpose of the configuration file is to say ebiff to
    check mailbox x for new mail and notify the user with y.<BR/>
    Supported mailbox drivers are: 
    <xsl:for-each select="/relation/field[name/text()='mailbox']/allowed/subitem/mailbox/field/allowed/subitem/*">
      <A>
        <xsl:attribute name="HREF">
          #<xsl:value-of select="name(.)"/>
        </xsl:attribute>
        <xsl:value-of select="name(.)"/>
      </A>
      <xsl:if test="position() &lt; last() ">, </xsl:if>
    </xsl:for-each>.
    <BR/>
    Supported notifyer drivers are: 
    <xsl:for-each select="/relation/field[name/text()='notifyer']/allowed/subitem/notifyer/field/allowed/subitem/*">
      <A>
        <xsl:attribute name="HREF">
          #<xsl:value-of select="name(.)"/>
        </xsl:attribute>
        <xsl:value-of select="name(.)"/>
      </A>
      <xsl:if test="position() &lt; last() ">, </xsl:if>
    </xsl:for-each>.
    <BR/>
    The function call <TT>bind(mailbox,notifyer)</TT>
    creates a new <B>relation</B> , and adds it to the relation 
    list used by ebiff.<BR/>
    An alternative way of calling <B>bind</B> is to pass a mailbox list or
    a notifyer list instead of a single element. The right syntax to do this 
    is <TT>bind({box1,box2,..,boxn},{not1,not2,...,notm})</TT>.<BR/>
    This is a short example of a configuration file (a longer and more complex
    example is in <TT>/usr/share/doc/ebiff/</TT> in the debian system):
<pre>
--
-- This is a sample configuration file for ebiff
--

-- create a mailbox
b_inbox = new{
	type = "mailbox";
	name = "Inbox";
	command = "mutt.sh -f =inbox &amp;";
	driver = {
		type="maildir";
		path = "/home/me/Mail/inbox";
		};
	interval=10;
	}
	
-- create a gtk2 notifyer
n_gtk2 = new{
	type = "notifyer";
	driver= {
		type="gtk2";
		position="right";
		showall=true;
		}
	}

-- create a xosd notifyer
n_osd = new{
	type = "notifyer";
	driver= {
		type="xosd";
		}
	}

-- bind them
bind(b_inbox,{n_gtk2,n_osd})
</pre>
    In the example the mailbox <TT>b_inbox</TT> will be inspected every 
    10 seconds and eventually both <TT>n_gtk2</TT> and <TT>n_osd</TT>
    will be used to notify the user.<BR/>
    Rememeber that comments starts with <TT>--</TT>, strings can be queted with
    single <TT>'</TT> or double <TT>"</TT> quotes, boolean values are 
    <TT>true</TT> and <TT>false</TT>. The syntax and 
    functionalities you can use are really more than these, since 
    the configuration file is a real
    <A CLASS="normal" HREF="http://www.lua.org">LUA</A> script 
    (see <A CLASS="normal" HREF="http://lua-users.org">lua-users</A> for a 
    tutorial). Some syntax peculiarity follow:
<UL>
<LI>The following sentences are the same for LUA:

<OL>
<LI><TT> a = { b="3" , c=3 , d=true }</TT></LI>
<LI><TT> a = { b='3' , c=3 , d=true , }</TT></LI>
<LI><TT> a = { b="3" ; c=3 ; d=true ; }</TT></LI>
<LI><TT> a = { b='3' ; c=3 ; d=true  }</TT></LI>
<LI><TT> a = {}<BR/>
a.b = "3"<BR/>
a.c = 3<BR/>
a.d = true</TT></LI>
</OL>
<BR/>
<OL>
<LI><TT> a.b = 3</TT></LI>
<LI><TT> a["b"] = 3</TT></LI>
</OL>
<BR/>
<OL>
<LI><TT> a = { b = "3" }</TT></LI>
<LI><TT> a = { ["b"] = "3" }</TT></LI>
</OL>
<BR/>
</LI>

<LI>Some of these notations are useful if the name contains spaces or 
non alphanumeric characters:
<OL>
<LI><I>invalid</I>:<BR/> <TT>
	a.lp-forum = "3"<BR/>
	c = { hi man = true }</TT></LI>
<LI><I>valid</I>:<BR/><TT>
	a["lp-forum"] = "3"<BR/>
	c = { ["hi man"] = true }</TT></LI>
</OL>
</LI>
</UL>
    The last important thing you must know is tgat you must use the 
    <TT>new</TT> function (ok, in the example it has no <TT>()</TT>, but
    it is automatically understood by LUA) to create a new mailbox or notifyer
    instance (calling new also adds the <TT>id</TT> field, this is why it is a
    required field but not used in the sample configuration).<BR/>
    A <B>relation</B> has the following fields (notice that the tables 
    <B>mailbox</B> and <B>notifyer</B> that must be passed to <TT>bind</TT>
    are subtables of <B>relation</B> and theys fields are listed here):
    <xsl:apply-templates select="relation"/>
    <HR/>
    </body>
  </html>
</xsl:template>


</xsl:stylesheet>

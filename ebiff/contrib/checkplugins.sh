#!/bin/sh

PLUGINS="stdout"

echo "Checking for plugins libraryes:"
echo
echo -n "-> gtk2..."
pkg-config gtk+-2.0 1>/dev/null 2>/dev/null
if [ $? = 0 ]; then
	PLUGINS="$PLUGINS gtk2"
	echo "ok"
else
	echo "pkg-config gtk+-2.0 failed"
	echo "   --> apt-get install libgtk2.0-dev"
fi

echo -n "-> sox..."
type play 1>/dev/null 2>/dev/null
if [ $? = 0 ]; then
	PLUGINS="$PLUGINS sox"
	echo "ok"
else
	echo "unable to find play"
	echo "   --> apt-get install sox"
fi

echo -n "-> xosd..."
type xosd-config 1>/dev/null 2>/dev/null
if [ $? = 0 ]; then
	PLUGINS="$PLUGINS xosd"
	echo "ok"
else
	echo "unable to find xosd-config"
	echo "   --> apt-get install libxosd-dev"
fi

echo -n "-> flite..."
test -e /usr/include/flite/flite.h
if [ $? = 0 ]; then
	PLUGINS="$PLUGINS flite"
	echo "ok"
else
	echo "unable to find /usr/include/flite/flite.h"
	echo "   --> apt-get install flite1-dev"
fi

echo
echo "PLUGINS: $PLUGINS"

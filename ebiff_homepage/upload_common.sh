USER=$(cat CVS/Root | cut -f 3 -d ":" | cut -d @ -f 1)
DIR=/home/groups/e/eb/ebiff/htdocs/
SITE=ebiff.sf.net
URI=$USER@$SITE:$DIR

function setpermissions()
{
ssh $USER@$SITE "find $DIR/ -type d -exec chmod a+x \{\} \;"
ssh $USER@$SITE "chgrp -R ebiff $DIR/*"
ssh $USER@$SITE "chmod -R 664 ebiff $DIR/*"
ssh $USER@$SITE "find $DIR/ -type d -exec chmod a+x \{\} \;"
}

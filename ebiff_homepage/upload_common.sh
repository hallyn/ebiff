USER=$(cat CVS/Root | cut -f 3 -d ":" | cut -d @ -f 1)
DIR=/home/groups/l/li/liberopops/htdocs/
SITE=liberopops.sf.net
URI=$USER@$SITE:$DIR

function setpermissions()
{
ssh $USER@$SITE "find $DIR/ -type d -exec chmod a+x \{\} \;"
ssh $USER@$SITE "chgrp -R liberopops $DIR/*"
ssh $USER@$SITE "chmod -R 664 liberopops $DIR/*"
ssh $USER@$SITE "find $DIR/ -type d -exec chmod a+x \{\} \;"
}

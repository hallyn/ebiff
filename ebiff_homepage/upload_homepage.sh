#!/bin/bash

. upload_common.sh

scp -r homepage/*.html homepage/*.css homepage/*.jpg homepage/*.png $URI

ssh $USER@$SITE find $DIR -name CVS -exec rm -rf "\{\}" "\;" 2> /dev/null
ssh $USER@$SITE find $DIR -name .xvpics -exec rm -rf "\{\}" "\;" 2> /dev/null

setpermissions

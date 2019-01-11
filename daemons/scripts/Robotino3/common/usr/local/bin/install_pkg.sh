#!/bin/bash

dpkg -i /var/www/uploads/*.deb > /tmp/robotinodpkg.tmp
echo "ENDLINE $?" >> /tmp/robotinodpkg.tmp

#!/bin/sh
majorversion=0
subvn=$(git rev-list --merges HEAD | wc -l)
subsubvn=$(git shortlog | grep -E '^[ ]+\w+' | wc -l)
sed -e "s/{major}/$majorversion/" -e "s/{minor}/$subvn/" -e "s/{build}/$subsubvn/" ../include/_version.h > ../include/version.h
echo "0.115"
#!/bin/sh
majorversion=0
subvn=$(git rev-list --merges HEAD | wc -l)
subsubvn=$(git shortlog | grep -E '^[ ]+\w+' | wc -l)
echo $majorversion.$subvn.$subsubvn

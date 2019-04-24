#!/bin/bash

count=2
echo "Searching for : $@"
for term in $@ ; do
    echo "$term"
    search="$search%20$term"
done
imagelink=$(wget --user-agent 'Mozilla/5.0' -qO - "www.google.pl/search?q=$search\&tbm=isch" | sed 's/</\n</g' | grep '<img' | head -n"$count" | tail -n1 | sed 's/.*src="\([^"]*\)".*/\1/')
wget -O ./pictures/currentHotel $imagelink 

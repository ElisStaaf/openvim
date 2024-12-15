#!/usr/bin/bash

bash build.sh
sudo cp openvim /usr/local/bin/

if [ ! $1 == "-R" ]; then
    exit
fi

if [ -f "~/.ov.cfg" ]; then
    rm ~/.ov.cfg
fi

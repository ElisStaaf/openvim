#!/usr/bin/bash

bash build.sh
sudo cp openvim /usr/local/bin/

if [ -f "~/.ov.cfg" ]; then
    rm ~/.ov.cfg
fi

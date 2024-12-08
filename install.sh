#!/usr/bin/bash

bash build.sh
sudo cp openvim /usr/local/bin/

if [ -f "~/.openvimrc" ]; then
    rm ~/.openvimrc
fi

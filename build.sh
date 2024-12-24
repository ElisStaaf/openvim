#!/usr/bin/bash

CC=gcc
CFLAGS="-std=gnu2x -Os -s"

for FILE in $(find "./src" -name "*.c" -not -wholename "*/style/*" -not -wholename "*/bare_x86/*"); do
    if [ $FILE -nt ${FILE}.o ]; then
        echo $FILE
        $CC -c $FILE -o ./${FILE}.o -I./include -Wall -Wextra $CFLAGS
    fi
done

$CC $(find . -name "*.o") $CFLAGS -o ./openvim

#!/bin/sh

if [ ! -f tools/generate-components ]; then
    clang++ -std=c++17 -Ivendor/cxxopts/include -Ivendor/orderedmap/include -Ivendor/toml11 tools/components-generator/*.cpp -o tools/generate-components
fi

if [ ! -d sdk/components ]; then
  mkdir sdk/components
fi

for DIR in ./engine/ `ls -d modules/*/`
do
    FILE="${DIR}components.toml"
    MODULE=`echo $DIR | sed 's/.*\/\(.*\)\/$/\1/'`
    if [ -f $FILE ]; then
        if [ "$MODULE" = "engine" ]; then
            MODULE=core
        fi
        ./tools/generate-components --in $FILE --header sdk/components/$MODULE.hpp --source ${DIR}${MODULE}_components.cpp
    fi
done

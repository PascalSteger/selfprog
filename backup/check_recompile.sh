#!/bin/bash

cd /tmp/cell
mkdir -p recompiles
mkdir -p norecompiles
cd singles
ls cell* > filelist
cd ..


cat singles/filelist |
    while read nextfile
    do
        rm -f outcell
        touch outcell
        od -An -tx1 singles/$nextfile > progcell
        ./singles/$nextfile < progcell > outcell
        if diff singles/$nextfile outcell >/dev/null
        then
            echo "can compile itself"
            mv singles/$nextfile recompiles
        else
            mv singles/$nextfile norecompiles
        fi
    done

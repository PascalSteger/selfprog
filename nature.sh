#!/bin/bash

timestamp(){
    date +"%Y%m%d%H%M%S%N"
}

mkdir -p /tmp/cell
rsync -avqz --exclude 'backup' ~/dev/cell/ /tmp/cell
cd /tmp/cell
mkdir -p backup

for i in {1..200}
do
    cp recompiles/$(ls recompiles|shuf|tail -n1) cell
    cp cell newcell
    cp cell tmpcell
    echo "" > output
    rm -f exitcode
    success=999

    # repeat for a number of times, mostly 1, but sometimes 2..up to 10 bytes in cell
    cycles=$(( $RANDOM % 10 ))
    if [ $cycles -gt 2 ]
    then
        cycles=1
    else
        len=$(wc -c tmpcell|cut -d" " -f1)
        cycles=$(( ($RANDOM % 10)+1 ))
    fi

    for (( j=1; j<=$cycles; j++ ))
    do
        len=$(wc -c tmpcell|cut -d" " -f1)
        pos=$(( $RANDOM % $len ))
        byt=$(od -vAn -N1 -tx1 /dev/urandom | tr -cd 0123456789abcdef)

        # choose of three options: add random byte at random position,
        # change byte at random position
        # delete byte from random position
        option=$(( $RANDOM % 3 ))
        if [ $option -eq 0 ]
        then
            #echo "delete random byte"
            mypos=$(echo "$pos-1"|bc -l|tail -n1)
            mycount=$(echo "$len-$pos"|bc -l|tail -n1)
            dd status=none if=tmpcell of=cell_part1 bs=1 skip=0 count=$mypos
            dd status=none if=tmpcell of=cell_part2 bs=1 skip=$pos count=$mycount
            cat cell_part1 cell_part2 > newcell
        elif [ $option -eq 1 ]
        then
            #echo "Add random byte"
            dd status=none if=tmpcell of=cell_part1 bs=1 skip=0 count=$pos
            mycount=$(echo "$len-$pos"|bc -l|tail -n1)
            dd status=none if=tmpcell of=cell_part2 bs=1 skip=$pos count=$mycount
            echo -ne \\x$byt | cat cell_part1 - cell_part2 > newcell
        else
            #echo "change random byte to newcell"
            echo -ne \\x$byt | dd status=none conv=notrunc bs=1 count=1 seek=$pos of=newcell
        fi
        #cp newcell tmpcell
    done
    chmod 700 newcell

    # check whether cell was already tried, before trying to execute it
    hashi=$(md5sum newcell | cut -d' ' -f1)
    donetried=$(grep -c $hashi history)
    if (( $donetried > 0 ))
    then
        echo "tried already"
        continue
    fi
    echo '$hashi' >> history

    $(./newcell < input > output &> /dev/null; echo $? > exitcode ) &

    success=999
    PID=$(ps ax|grep newcell|grep -iv grep|awk '{print $1}')
    if [[ -n "$PID" ]]
    then
        sleep 1
        PID=$(ps ax|grep newcell|grep -iv grep|awk '{print $1}')
        if [[ ! -z "$PID" ]]; then
            #echo "program took too long"
            pkill newcell
            success=998
        else
            success=0
        fi
    else
        success=$(cat exitcode)
    fi

    if (( $success == 0 ))
    then
        echo "$i: $cycles: success"
        cp newcell backup/cell_$(timestamp)
    fi
done

# single out the new ones
cd backup
mkdir -p singles
ls cell* > filelist
cd /tmp/cell
cat backup/filelist |
    while read nextfile
    do
        hashi=$(md5sum backup/$nextfile | sort | cut -d' ' -f1)
        nof=$(grep -c $hashi recompiles/msum)
        if (( $nof > 0 ))
        then
            echo backup/$nextfile " already exists"
            rm backup/$nextfile
        else
            #echo backup/$nextfile " is different from all others!"
            mv backup/$nextfile singles
        fi
    done

cd /tmp/cell
mkdir -p recompiles
cd singles
ls cell* > filelist
cd /tmp/cell
echo "sorting out non-reproductive cells"
cat singles/filelist |
    while read nextfile
    do
        rm -f outcell
        touch outcell
        od -An -tx1 singles/$nextfile > progcell
        ./singles/$nextfile < progcell > outcell
        if diff singles/$nextfile outcell >/dev/null
        then
            echo "new recompiling: " $nextfile
            mv singles/$nextfile recompiles
        else
            rm singles/$nextfile
        fi
    done

cd /tmp/cell/recompiles
md5sum cell* | cut -d' ' -f1 > msum
cp /tmp/cell/recompiles/* /home/psteger/dev/cell/recompiles
cp /tmp/cell/history /home/psteger/dev/cell/

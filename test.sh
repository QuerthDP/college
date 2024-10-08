#!/bin/bash

CPPFLAGS=(-Wall -Wextra -O2 -g3 -std=c++20)
VALFLAGS=(--error-exitcode=123 --leak-check=full --show-leak-kinds=all
          --errors-for-leak-kinds=all --run-cxx-freeres=yes -q)

for i in *.log
do
    DIR=${i%.log}
    mkdir $DIR
    for h in *.h
    do
        cp $h ./$DIR/$h
        cp ${h%.h}.cc ./$DIR/${h%.h}.cc 2>/dev/null
    done
    cp $i ./$DIR/$i
    cp ${i%.log}.cc ./$DIR/${i%.log}.cc
    cd ./$DIR
    g++ ${CPPFLAGS[@]} *.cc

    echo -e "${i%.log} >>"

    if (valgrind ${VALFLAGS[@]} ./a.out 2>${i%.log}.out)
    then
        echo -e "\tASSERT \033[0;32m"OK"\033[0;0m"
    else
        echo -e "\tASSERT \033[0;31m"NOT OK"\033[0;0m"
    fi

    if (diff ${i%.log}.out $i &>/dev/null)
    then
        echo -e "\tRESULT \033[0;32m"OK"\033[0;0m"
    else
        echo -e "\tRESULT \033[0;31m"NOT OK"\033[0;0m"
        diff ${i%.log}.out $i | head -15
    fi

    cd ..
    rm -r ./$DIR 2>/dev/null
done

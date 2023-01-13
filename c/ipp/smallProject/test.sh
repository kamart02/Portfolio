#!/bin/bash

# checking for proper arguments

if [[ $# -ne 2 ]]
then
    echo "arg1: program, arg2: directory";
    exit 1;
fi

if [[ -e $1 ]]
then
    PROGRAM=$1;
else
    echo "$1 is not a file";
    exit 1;
fi

if [[ -d $2 ]]
then
    DIRECORY=$2;
    # removig trailing /
    DIRECORY=$(echo $DIRECORY | sed 's:/*$::');
else
    echo "$2 is not a directory";
    exit 1;
fi


# colours

RED='\033[0;31m';
GREEN='\033[0;32m';
NC='\033[0m';

# tracking variables

NUM_OF_TESTS=0;
NUM_OF_FAILS=0;
EXIT_CODE=0;
DIFF_ERR_EXIT_CODE=0;
DIFF_OUT_EXIT_CODE=0;

# run and check

for f in $DIRECORY/*.in
do
    #removig previous files
    rm "${f%in}outprog" 2>"/dev/null";
    rm "${f%in}errprog" 2>"/dev/null";

    echo -n ${f%.in};
    # running program
    valgrind --quiet --error-exitcode=2 --leak-check=full "./${PROGRAM}" <"$f" 1>"${f%in}outprog" 2>"${f%in}errprog";
    EXIT_CODE=$?;

    # checking out files
    diff ${f%.in}.err ${f%.in}.errprog > "/dev/null";
    DIFF_ERR_EXIT_CODE=$?;

    diff ${f%.in}.out ${f%in}outprog > "/dev/null";
    DIFF_OUT_EXIT_CODE=$?;

    ((NUM_OF_TESTS++));

    if [[ ${EXIT_CODE} -eq 2 ]]
    then
        echo -e " -------${RED}Valgrind error${NC}-------";
        ((NUM_OF_FAILS++));
    elif [ -s ${f%in}err ]
    then 
        if [[ ${EXIT_CODE} -ne 1 ]]
        then
            echo -e " -------${RED}Wrong exit code${NC}-------";
            ((NUM_OF_FAILS++));
            continue;
        fi
    else
        if [[ ${EXIT_CODE} -ne 0 ]]
        then
            echo -e " -------${RED}Wrong exit code${NC}-------";
            ((NUM_OF_FAILS++));
            continue;
        fi
    fi
    if [[ ${DIFF_ERR_EXIT_CODE} -ne 0 ]]
    then
        echo -e "$ -------${RED}FAIL.err${NC}-------";
        ((NUM_OF_FAILS++));
    elif [[ ${DIFF_OUT_EXIT_CODE} -ne 0 ]]
    then
        echo -e " -------${RED}FAIL.out${NC}-------";
        ((NUM_OF_FAILS++));
    else
        echo -e " ${GREEN}PASS${NC}";    
    fi
done

echo "Tested: ${NUM_OF_TESTS} ";
echo "Failed: ${NUM_OF_FAILS} ";

#!/usr/bin/env bash

EXE=a.out
COMPILER="g++ -std=c++11 -o $EXE"
VERBOSE=true
DEST_DIR="codes"
TEMP_DIR="temp"
JUDGE_DIR="data"
INPUT_NAME="input.txt"
SOL_NAME="sol.txt"
OUT_EXT="out"
CODE_ADDR=$1
TIME_LIM=10s
VERBOSE_DIFF_TOOL="sdiff -sw 60"
DIFF_TOOL="sdiff -s"

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
B="\033[1;4m"
NC="\033[0m"

JUDGE_DIR=$(readlink -f "$JUDGE_DIR")
CODE_ADDR=$(readlink -f "$CODE_ADDR")

param1='echo $JUDGE_DIR/$test_case/courses.csv'
param2='cat $JUDGE_DIR/$test_case/name.txt'

if [[ $1 == "--unzip" ]] || [[ $1 == "-u" ]]; then
    if [[ -e "$DEST_DIR" ]]; then
        echo -e "${B}$DEST_DIR${NC} already existed."
        rm -rI "$DEST_DIR"
    fi
    unzip "$2" -d "$DEST_DIR" > /dev/null
    pushd "$DEST_DIR" > /dev/null
    rm -rf *_onlinetext_
    for f in *; do
        mv "$f" "`echo ${f/_*_assignsubmission_file_/}`"
    done
    for f in **/*.zip; do
        pushd "`dirname "$f"`" > /dev/null
        unzip "`basename "$f"`" > /dev/null && rm "`basename "$f"`"
        popd > /dev/null
    done
    rm -rf **/__MACOSX 2> /dev/null
    rm -rf **/.DS_Store 2> /dev/null
    find . -name *.out -delete
    find . -name *.o -delete
    popd > /dev/null
    echo -e "$PWD/${B}$DEST_DIR${NC} ("$(ls "$DEST_DIR" | wc -l)")"

elif [[ $1 == "--help" ]] || [[ $1 == "-h" ]] || [[ $# != 1 ]]; then
    echo "usage:"
    echo -e "\t$0 --help|-h"
    echo -e "\t$0 --unzip|-u <codes_archive_addr>"
    echo -e "\t$0 <code_file>"

else
    if [[ ! -e $TEMP_DIR ]]; then
        mkdir $TEMP_DIR
    fi

    pushd "$TEMP_DIR" > /dev/null
    rm * 2> /dev/null
    passed=0
    failed=0
    compiled=true
    echo -e "\n${YELLOW}Compiling...${NC}"
    if ! $COMPILER "$CODE_ADDR"; then
        echo -e "${RED}Compile Error${NC}"
        compiled=false
        failed=$(ls -l "$JUDGE_DIR/" | grep "^d" | wc -l)
    else
        echo -e "${GREEN}Compiled!${NC}"
        echo -e "\n${YELLOW}Running...${NC}"
        for test_case in "$JUDGE_DIR/"*"/"; do
            test_case="$(basename "$test_case")"
            input="$JUDGE_DIR/$test_case/$INPUT_NAME"
            sol="$JUDGE_DIR/$test_case/$SOL_NAME"
            output="$test_case.$OUT_EXT"
            printf "Testcase $test_case: "
            if timeout $TIME_LIM ./$EXE $(eval $(eval "echo $param1")) $(eval $(eval "echo $param2")) < "$input" > "$output"; then
                if $DIFF_TOOL "$output" "$sol" > /dev/null; then
                    echo -e "${GREEN}Accepted${NC}"
                    ((passed+=1))
                else
                    echo -e "${RED}Wrong Answer${NC}"
                    if $VERBOSE; then
                        printf "%28s | %28s\n" "< $output" "> $sol"
                        $VERBOSE_DIFF_TOOL "$output" "$sol"
                    fi
                    ((failed+=1))
                fi
            else
                echo -e "${RED}Timed out${NC}"
                ((failed+=1))
            fi
        done
    fi
    echo -e "\n${YELLOW}Report${NC}"
    printf "Code: $1 (compiled: %b)\n" $compiled
    echo -e "Passed:\t${GREEN}$passed${NC} out of $((passed + failed))"
    echo -e "Failed:\t${RED}$failed${NC} out of $((passed + failed))"
    popd  > /dev/null

fi

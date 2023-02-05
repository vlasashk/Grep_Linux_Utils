#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
RESULT=0
DIFF_RES=""

declare -a tests=(
"for s21_grep.c s21_grep.h Makefile VAR"
"Makefile -e for -e ^int s21_grep.c s21_grep.h VAR"
"Makefile -e while -e void s21_grep.c VAR -f test_ptrn_grep.txt"
)

declare -a extra=(
""
"VARn for test_1_grep.txt test_2_grep.txt"
"VARn for test_1_grep.txt"
"VARn -e ^\} test_1_grep.txt"
"VARn -e /\\ test_1_grep.txt"
"-ce ^int test_1_grep.txt test_2_grep.txt"
"test_1_grep.txt -e ^int"
"VARnivh = test_1_grep.txt test_2_grep.txt"
"VARe"
"-ie INT test_5_grep.txt"
"test_2_grep.txt -echar test_1_grep.txt"
"VARne = -e out test_5_grep.txt"
"-iv int test_5_grep.txt"
"-in int test_5_grep.txt"
"-c -l aboba test_1_grep.txt test_5_grep.txt"
"-v test_1_grep.txt -e ank"
"VARnoe ) test_5_grep.txt"
"-l for test_1_grep.txt test_2_grep.txt"
"-o -e int test_4_grep.txt"
"test_5_grep.txt -e = -e out"
"VARnoe ing -e as -e the -e not -e is test_6_grep.txt"
"test_6_grep.txt -e ing -e as -e the -e not -e is"
"test_6_grep.txt -e ing -e as -e the -e not -e is -s"
"-c -e . test_1_grep.txt -e '.'"
"-l for no_file.txt test_2_grep.txt"
"-l for no_file.txt test_2_grep.txt -s"
"-f test_3_grep.txt test_5_grep.txt"
)

testing()
{
{
    t=$(echo $@ | sed "s/VAR/$var/")
    valgrind --log-file="valgrind_s21_grep.log" --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./s21_grep $t > test_s21_grep.log
    leak=$(grep ERROR valgrind_s21_grep.log)
    (( COUNTER++ ))
    if [[ $leak == *"0 errors from 0 contexts"* ]]
    then
      (( SUCCESS++ ))
        echo -e "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[32msuccess\033[0m grep $t"
    else
      (( FAIL++ ))
        echo -e "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNTER \033[31mfail\033[0m grep $t"
#        echo "$leak"
    fi
    rm valgrind_s21_grep.log
    rm test_s21_grep.log

}
}

# специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in i v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 параметра
for var1 in i o
do
    for var2 in v c l n h
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

echo -e "\033[31mFAIL: $FAIL\033[0m"
echo -e "\033[32mSUCCESS: $SUCCESS\033[0m"
echo "ALL: $COUNTER"

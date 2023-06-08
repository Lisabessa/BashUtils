SUCCESS_COUNT=0
FAIL_COUNT=0
DIFF_RES=""

declare -a tests=(
"VAR cat_test_main.txt"
)

declare -a extra_tests=(
"cat_test1.txt -b -e"
"cat_test2.txt"
"-t -b -s cat_test3.txt"
"-e -t cat_test2.txt"
"no_exist.txt"
"-e -v -b -n cat_test2.txt"
"-b -n cat_test1.txt"
"-b -e -s -t -n cat_test1.txt cat_test2.txt cat_test3.txt"
"cat_test1.txt -b -e -s -t -n cat_test2.txt cat_test3.txt"
)

test_cat()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./realized_cat $t > test_realized_cat.txt
    cat $t > test_cat.txt
    DIFF_RES="$(diff -s test_realized_cat.txt test_cat.txt)"
    leaks -quiet -atExit -- ./realized_cat $t > test_leaks_realized_cat.txt
    leak=$(grep -A100000 leaks test_leaks_realized_cat.txt)

    if [ "$DIFF_RES" == "Files test_realized_cat.txt and test_cat.txt are identical" ] && [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
    then
      (( SUCCESS_COUNT++ ))
        echo "\033[32msuccess\033[0m realized_cat $t"
    else
      (( FAIL_COUNT++ ))
        echo "\033[31mfail\033[0m realized_cat $t"
    fi
    rm test_realized_cat.txt test_cat.txt test_leaks_realized_cat.txt
}

for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_cat $i
    done
done

for i in "${extra_tests[@]}"
do
    var="-"
    test_cat $i
done

echo "-------------------------"
echo "SUCCESS: $SUCCESS_COUNT"
echo "FAIL: $FAIL_COUNT"
echo "-------------------------"
if [ "$FAIL_COUNT" == 0 ]
then 
    echo "VERDICT: \033[32mOK\033[0m"
else 
    echo "VERDICT: \033[31mFAILED\033[0m"
fi

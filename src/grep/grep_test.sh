SUCCESS_COUNT=0
FAIL_COUNT=0
DIFF_RES=""

declare -a tests=(
"hello grep_test1.txt VAR"
"hi VAR grep_test2.txt"
"VAR career grep_test2.txt"
"grep_test3.txt rr VAR"
"VAR -e you -e us grep_test2.txt grep_test1.txt"
"VAR -f grep_test_regexes.txt grep_test1.txt"
"-f grep_test_regexes.txt VAR grep_test1.txt grep_test2.txt grep_test3.txt"
"VAR -f grep_test_regexes.txt -e hello grep_test1.txt grep_test3.txt"
)

declare -a extra_tests=(
"lo -ivclnho grep_test2.txt"
"-cl -f grep_test_regexes.txt grep_test1.txt"
"-iv i grep_test2.txt"
"-ivc -fgrep_test_regexes.txt grep_test3.txt"
"-i -v -l -n -c grep_test2.txt grep_test1.txt grep_test3.txt"
"-ce hi grep_test3.txt grep_test1.txt"
"-e 'hi\|e' grep_test3.txt"
"grep_test3.txt -ehello -fgrep_test_regexes.txt"
)

test_grep()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./realized_grep $t > test_realized_grep.txt
    grep $t > test_grep.txt
    DIFF_RES="$(diff -s test_realized_grep.txt test_grep.txt)"
    leaks -quiet -atExit -- ./realized_grep $t > test_leaks_realized_grep.txt
    leak=$(grep -A100000 leaks test_leaks_realized_grep.txt)

    if [ "$DIFF_RES" == "Files test_realized_grep.txt and test_grep.txt are identical" ] && [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
    then
      (( SUCCESS_COUNT++ ))
        echo "\033[32msuccess\033[0m realized_grep $t"
    else
      (( FAIL_COUNT++ ))
        echo "\033[31mfail\033[0m realized_grep $t"
    fi
    rm test_realized_grep.txt test_grep.txt test_leaks_realized_grep.txt
}

for var1 in i v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_grep $i
    done
done

for i in "${extra_tests[@]}"
do
    var="-"
    test_grep $i
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
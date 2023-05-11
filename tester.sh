i=1
while [ $i -le 100 ]
do
    echo "test in $i"
    rm -rf ./data/*
    g++ gen.cpp -o gen && ./gen
    ./build/code <test.in >test.out
    diff test.out test.ans
    if (($?)); then
        break
    else
        echo "pass"
    fi
    i=`expr $i + 1`
done 
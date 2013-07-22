for x in 1 2 3 4 5 6
do
    echo "test$x"
    ./a.out < "test$x" | diff - "out$x"
done

#!/bin/bash

DIR=(simple intercomm async) 
ARGS=("-p world" "-p producer -p consumer" "-p producer -p consumer")
PROCS=(2 4 8) 
PASSED=1
COUNT=0

for directory in ${DIR[*]};
do
    echo "Testing $directory now!"
    cd $directory
    HWL_FILE=$directory".hwl"
    CHAI_FILE=$directory".chai"
    CURR_ARGS=${ARGS[$COUNT]}

    for num in ${PROCS[*]};
    do
        CHAI_PASS=1
        HWL_PASS=1
        HWL_OUT=$HWL_FILE".$num""out"
        CHAI_OUT=$CHAI_FILE".$num""out"

        # So that we don't get false negatives the files have to be run repeatedly.
        # This is due to a weird buffer not getting flushed thing.
        FAIL_REPEAT=0
        for i in $(seq 1 8)
        do
            mpirun -n $num ../../henson-hwl $HWL_FILE 1>$HWL_OUT 2>&1
            mpirun -n $num ../../henson-chai $CHAI_FILE $CURR_ARGS 1>$CHAI_OUT 2>&1

            if grep -q "henson done" $HWL_OUT && grep -q "henson done" $CHAI_OUT
            then
                FAIL_REPEAT=$(($FAIL_REPEAT|1))
                break
            fi
            sleep 1
        done

        if [ $FAIL_REPEAT -eq 0 ]
        then
            PASSED=0
            CHAI_PASS=0
            HWL_PASS=0
            echo "Henson test failed for $directory with $num processors"
        fi

        if [ $CHAI_PASS -eq 1 ]
        then
            rm -f $CHAI_OUT
        fi

        if [ $HWL_PASS -eq 1 ]
        then
            rm -f $HWL_OUT
        fi
    done
    ((COUNT += 1))
    cd ..
done

if [ -e ../libhenson-pmpi.so ] || [ -e ../libhenson-pmpi.dylib ]
then
    echo "Testing pmpi-world now!"
    cd pmpi-world
    HWL_FILE="pmpi-world.hwl"
    CHAI_FILE="pmpi-world.chai"

    for num in ${PROCS[*]};
    do
        group_size=$(($num / 2))
        CHAI_PASS=1
        HWL_PASS=1
        HWL_OUT=$HWL_FILE".$num""out"
        CHAI_OUT=$CHAI_FILE".$num""out"

        FAIL_REPEAT=0
        for i in $(seq 1 8)
        do
            mpirun -n $num ../../henson-hwl $HWL_FILE -p producer=$group_size -p consumer=$group_size expected_size=$group_size 1>$HWL_OUT 2>&1
            mpirun -n $num ../../henson-chai $CHAI_FILE -p producer=$group_size -p consumer=$group_size --var expected_size=$group_size 1>$CHAI_OUT 2>&1

            if grep -q "henson done" $HWL_OUT && grep -q "henson done" $CHAI_OUT && grep -q "PMPI world check passed" $HWL_OUT && grep -q "PMPI world check passed" $CHAI_OUT
            then
                FAIL_REPEAT=$(($FAIL_REPEAT|1))
                break
            fi
            sleep 1
        done

        if [ $FAIL_REPEAT -eq 0 ]
        then
            PASSED=0
            CHAI_PASS=0
            HWL_PASS=0
            echo "Henson test failed for pmpi-world with $num processors"
        fi

        if [ $CHAI_PASS -eq 1 ]
        then
            rm -f $CHAI_OUT
        fi

        if [ $HWL_PASS -eq 1 ]
        then
            rm -f $HWL_OUT
        fi
    done
    cd ..
else
    echo "Skipping pmpi-world because mpi-wrappers are disabled."
fi

if [ $PASSED -eq 1 ]
then
    echo "All tests passed!"
else
    exit 1
fi

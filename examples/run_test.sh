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

        #So that we don't get false negatives the files have to be run three times
        #This is due to a weird buffer not getting flushed thing
        FAIL_REPEAT=0
        for i in $(seq 1 8)
        do
            mpirun -n $num ../../henson $HWL_FILE 1>$HWL_OUT 2>&1
            mpirun -n $num ../../henson_chai $CHAI_FILE $CURR_ARGS 1>$CHAI_OUT 2>&1
            WC_HWL=$(wc -l $HWL_OUT | awk '{ print($1); }') 
            WC_CHAI=$(wc -l $CHAI_OUT | awk '{print($1); }')

            if [ $WC_HWL -eq $WC_CHAI ]
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
            echo "Henson chai and henson don't match output line numbers $directory failed with $num proccessors"
        fi

        FIN=$(tail -n 1 $HWL_OUT | awk '{ print($5, $6) }')
        if [ "$FIN" != "henson done" ]
        then
            PASSED=0
            HWL_PASS=0
            echo "Henson test failed with $HWL_FILE"
        fi

        FIN=$(tail -n 1 $CHAI_OUT | awk '{ print($5, $6) }')
        if [ "$FIN" != "henson done" ]
        then
            PASSED=0
            CHAI_PASS=0
            echo "Henson chai test failed with $CHAI_FILE"
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

if [ $PASSED -eq 1 ]
then
    echo "All tests passed!"
fi



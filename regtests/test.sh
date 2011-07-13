#!/bin/bash

function pad_space
{
	local PREFIX=$1
        # adjust the length of header
	NEW_SPACE=${#PREFIX}
        NEW_SPACE=`expr 25 - ${NEW_SPACE}`
        PAD="";
        if [ "${NEW_SPACE}" -gt "0" ]; then # add space
                for (( i=0; i<${NEW_SPACE}; i=i+1 )); do
                        PAD="${PAD} ";
                done
        fi
	echo -n "${PAD}";
}

DIRS=`find . -maxdepth 1 -type d `
TOPLEVEL=`pwd`
echo "================================================="
for DIR in ${DIRS}; do
	if [ "${DIR}" != "." ]; then
		cd ${DIR}
		source ./test.sh
		TEST=`basename ${DIR}`
		echo -n "${TEST}";
		setup &> /dev/null;
		RESULT1=$?
		testcase &> /dev/null;
		RESULT2=$?
		teardown &> /dev/null
		RESULT3=$?
		test ${RESULT1} -eq 0 && test ${RESULT2} -eq 0 && test ${RESULT3} -eq 0
		RESULT=$?
		pad_space "${TEST}"
		if [ ${RESULT} -eq 0 ]; then
			echo "Passed!";
		else
			echo "Not Passed";
		fi
		cd ${TOPLEVEL};
	fi
done

#!/bin/bash

DIRS=`find . -maxdepth 1 -type d `
TOPLEVEL=`pwd`
echo "=========================="
for DIR in ${DIRS}; do
	if [ "${DIR}" != "." ]; then
		cd ${DIR}
		source ./test.sh
		TEST=`basename ${DIR}`
		echo -n "#${TEST}		";
		setup &> /dev/null;
		RESULT1=$?
		testcase &> /dev/null;
		RESULT2=$?
		teardown &> /dev/null
		RESULT3=$?
		test ${RESULT1} -eq 0 && test ${RESULT2} -eq 0 && test ${RESULT3} -eq 0
		RESULT=$?
		if [ ${RESULT} -eq 0 ]; then
			echo "Passed!";
		else
			echo "Not Passed";
		fi
		cd ${TOPLEVEL};
	fi
done

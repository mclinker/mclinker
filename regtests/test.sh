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
		test &> /dev/null;
		RESULT=$?
		if [ ${RESULT} -eq 1 ]; then
			echo "Not Passed!";
		else
			echo "Passed";
		fi
		cd ${TOPLEVEL};
	fi
done

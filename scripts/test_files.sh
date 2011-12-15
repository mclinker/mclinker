#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function ask_filename
{
	echo -n "What is the class you what to test for? ";
}

function ask_brief
{
	CLASS_NAME=$1
	echo "Please brief the testcase (optional):"
}

function copy_template_header
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/unittests/${NAME}Test.h";
	local SOURCE_FILE="${MCLINKERTOP}/templates/headerTest.h";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}

function copy_template_impl
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/unittests/${NAME}Test.cpp";
	local SOURCE_FILE="${MCLINKERTOP}/templates/implTest.cpp";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}

function replace_author
{
	local TARGET_FILE=$1
	shift
	local AUTHOR=$*

	sed "s/\${AUTHOR}/${AUTHOR}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
}

function repalce_email
{
	local TARGET_FILE=$1
	local EMAIL=$2
	sed "s/\${EMAIL}/${EMAIL}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp

	# adjust the length of header
	NEW_SPACE=`grep '>                                                    *' ${TARGET_FILE}.tmp | wc -c`
	NEW_SPACE=`expr 79 - ${NEW_SPACE}`
	ADDEND=">";
	if [ "${NEW_SPACE}" -gt "0" ]; then # add space
		for (( i=0; i<${NEW_SPACE}; i=i+1 )); do
			ADDEND="${ADDEND} ";
		done
		sed "s/${EMAIL}>/${EMAIL}${ADDEND}/" ${TARGET_FILE}.tmp > ${TARGET_FILE}
	elif [ "${NEW_SPACE}" -lt "0" ]; then # strip space
		for (( i=${NEW_SPACE}; i<0; i=i+1 )); do
			ADDEND="${ADDEND} ";
		done
		sed "s/${EMAIL}${ADDEND}/${EMAIL}>/g" ${TARGET_FILE}.tmp > ${TARGET_FILE}

	else #do nothing
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	fi

	rm ${TARGET_FILE}.tmp
	local EMAIL=$2
}

function replace_class
{
	local TARGET_FILE=$1
	local CLASS_NAME=$2
	local UPCLASS_NAME=`echo ${CLASS_NAME} | tr [:lower:] [:upper:]`

	sed "s/\${class_name}/${CLASS_NAME}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	sed "s/\${CLASS_NAME}/${UPCLASS_NAME}/g" ${TARGET_FILE}.tmp > ${TARGET_FILE}
	rm ${TARGET_FILE}.tmp
}

function replace_brief
{
	local TARGET_FILE=$1
	shift
	local BRIEF=$*
	sed "s/\${brief}/${BRIEF}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
}


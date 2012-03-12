#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function ask_filename
{
	echo -n "What is the class name? ";
}

function ask_brief
{
	CLASS_NAME=$1
	echo "Please brief the class ${CLASS_NAME}: "
}

function copy_template_header
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/include/mcld/${DEPLOYMENT}/${NAME}.h";
	local SOURCE_FILE="${MCLINKERTOP}/templates/header.h";
	mkdir -p ${MCLINKERTOP}/include/mcld/${DEPLOYMENT}
	cp ${SOURCE_FILE} ${TARGET_FILE}
	# replace the template name by target flie name and adjust the padding of '-'
	local header_str="header";
	PADDING_LEN=`expr ${#header_str} - ${#NAME}`;
	ABS_PADDING_LEN=${PADDING_LEN};
	PADDING="";

	if [ "${PADDING_LEN}" -lt "0" ]; then
		ABS_PADDING_LEN=`expr ${ABS_PADDING_LEN} \* -1`;
	fi
	for (( i=0; i < ${ABS_PADDING_LEN}; i = i + 1 )); do
		PADDING="${PADDING}-";
	done

	if [ "${PADDING_LEN}" -gt "0" ]; then # replace and add padding
		sed -e "s/header.h /${NAME}.h ${PADDING}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	elif [ "${PADDING_LEN}" -lt "0" ]; then # replace and strip padding
		sed -e "s/header.h ${PADDING}/${NAME}.h /g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	else # replace directly
		sed -e "s/header/${NAME}/g" ${TARGET_FILE} >  ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	fi

	echo "${TARGET_FILE}";
}

function copy_template_impl
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/lib/${DEPLOYMENT}/${NAME}.cpp";
	local SOURCE_FILE="${MCLINKERTOP}/templates/impl.cpp";
	mkdir -p ${MCLINKERTOP}/lib/${DEPLOYMENT}
	cp ${SOURCE_FILE} ${TARGET_FILE}
	# replace the template name by target flie name and adjust the padding of '-'
	local impl_str="impl";
	PADDING_LEN=`expr ${#impl_str} - ${#NAME}`;
	ABS_PADDING_LEN=${PADDING_LEN};
	PADDING="";
	if [ "${PADDING_LEN}" -lt "0" ]; then
		ABS_PADDING_LEN=`expr ${ABS_PADDING_LEN} \* -1`;
	fi

	for (( i = 0; i < ${ABS_PADDING_LEN}; i = i + 1 )); do
		PADDING="${PADDING}-";
	done

	if [ "${PADDING_LEN}" -gt "0" ]; then # replace and add padding
		sed -e "s/impl.cpp /${NAME}.cpp ${PADDING}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	elif [ "${PADDING_LEN}" -lt "0" ]; then # replace and strip padding
		sed -e "s/impl.cpp ${PADDING}/${NAME}.cpp /g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	else # replace directly
		sed -e "s/impl/${NAME}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	fi

	echo "${TARGET_FILE}";
}

function replace_author
{
	local TARGET_FILE=$1
	shift
	local AUTHOR=$*

	sed -e "s/\${AUTHOR}/${AUTHOR}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
}

function repalce_email
{
	local TARGET_FILE=$1
	local EMAIL=$2
	sed -e "s/\${EMAIL}/${EMAIL}/g" ${TARGET_FILE}  > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}

	# adjust the length of header
	NEW_SPACE=`grep '>                                                    *' ${TARGET_FILE} | wc -c`
	NEW_SPACE=`expr 79 - ${NEW_SPACE}`
	ADDEND=">";
	if [ "${NEW_SPACE}" -gt "0" ]; then # add space
		for (( i=0; i<${NEW_SPACE}; i=i+1 )); do
			ADDEND="${ADDEND} ";
		done
		sed -e "s/${EMAIL}>/${EMAIL}${ADDEND}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	elif [ "${NEW_SPACE}" -lt "0" ]; then # strip space
		for (( i=${NEW_SPACE}; i<0; i=i+1 )); do
			ADDEND="${ADDEND} ";
		done
		sed -e "s/${EMAIL}${ADDEND}/${EMAIL}>/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
		mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	fi
}

function replace_class
{
	local TARGET_FILE=$1
	local CLASS_NAME=$2
	local UPCLASS_NAME=`echo ${CLASS_NAME} | tr [:lower:] [:upper:]`

	sed -e "s/\${class_name}/${CLASS_NAME}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
	sed -e "s/\${CLASS_NAME}/${UPCLASS_NAME}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
}

function replace_brief
{
	local TARGET_FILE=$1
	shift
	local BRIEF=$*
	sed -e "s/\${brief}/${BRIEF}/g" ${TARGET_FILE} > ${TARGET_FILE}.tmp
	mv ${TARGET_FILE}.tmp ${TARGET_FILE}
}


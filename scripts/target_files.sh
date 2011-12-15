#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function copy_template_header
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/lib/${DEPLOYMENT}/${NAME}.h";
	local SOURCE_FILE="${MCLINKERTOP}/templates/header.h";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}

function copy_template_impl
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${MCLINKERTOP}/lib/${DEPLOYMENT}/${NAME}.cpp";
	local SOURCE_FILE="${MCLINKERTOP}/templates/impl.cpp";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}


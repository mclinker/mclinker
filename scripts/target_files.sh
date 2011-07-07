#!/bin/bash
##############################################################################
#   Copyright (C) 2011-
#   Embedded and Web Computing Lab, National Taiwan University
#   MediaTek, Inc.
#

function copy_template_header
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${BOLDTOP}/lib/${DEPLOYMENT}/${NAME}.h";
	local SOURCE_FILE="${BOLDTOP}/templates/header.h";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}

function copy_template_impl
{
	local NAME=$1
	local DEPLOYMENT=$2
	local TARGET_FILE="${BOLDTOP}/lib/${DEPLOYMENT}/${NAME}.cpp";
	local SOURCE_FILE="${BOLDTOP}/templates/impl.cpp";
	cp ${SOURCE_FILE} ${TARGET_FILE}
	echo "${TARGET_FILE}";
}


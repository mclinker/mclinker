#!/bin/bash
##############################################################################
#   Copyright (C) 2011-
#     Embedded and Web Computing Lab, National Taiwan University
#     MediaTek, Inc.
#


function gettop() 
{
	local TOPFILE=src/main.cpp
	if [ -n "${BOLDTOP}" -a -f "${BOLDTOP}/${TOPFILE}" ]; then
		echo ${BOLDTOP}
	else
		if [ -f "${TOPFILE}" ]; then
			echo `pwd`;
		else
			local HERE=$PWD
			T=
			while [ \( ! \( -f $TOPFILE \) \) -a \( $PWD != "/" \) ]; do
				cd .. > /dev/null
				T=`PWD= pwd`
			done
			cd $HERE > /dev/null
			if [ -f "${T}/${TOPFILE}" ]; then
				echo ${T}
			fi
		fi
	fi
	
}


#############################
#  Variable Dictionary
export BOLDTOP=$(gettop)
export PATH="${BOLDTOP}/scripts/bin:$PATH"

#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function gettop() 
{
	local TOPFILE=tools/llvm-mcld/llvm-mcld.cpp
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

function wc_pndk()
{
	local H=`find ${BOLDTOP} | grep '\.h'`
	local C=`find ${BOLDTOP} | grep '\.cpp'`
	wc ${C} ${H}
}

#############################
#  Variable Dictionary
export BOLDTOP=$(gettop)
export PATH="${BOLDTOP}/scripts/bin:$PATH"
alias mk="make -C ${BOLDTOP}"

SERVER=`grep mtksgt01 /etc/hosts`
if [ ! -z "${SERVER}" ]; then
	export PATH="/mtkoss/git/1.7.6-rhel-5.5/x86_64/bin:$PATH";
fi

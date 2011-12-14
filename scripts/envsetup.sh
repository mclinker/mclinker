#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function gettop() 
{
	local TOPFILE=tools/llvm-mcld/llvm-mcld.cpp
	if [ -n "${MCLINKERTOP}" -a -f "${MCLINKERTOP}/${TOPFILE}" ]; then
		echo ${MCLINKERTOP}
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
	local H=`find ${MCLINKERTOP} | grep '\.h'`
	local C=`find ${MCLINKERTOP} | grep '\.cpp'`
	wc ${C} ${H}
}

#############################
#  Variable Dictionary
export MCLINKERTOP=$(gettop)
export PATH="${MCLINKERTOP}/scripts/bin:$PATH"
if [ -x "${MCLINKERTOP}/debug/llvm-mcld" ]; then
	ln -sf ${MCLINKERTOP}/debug/llvm-mcld ${MCLINKERTOP}/scripts/bin/MCLinker
fi

alias mk="make -C ${MCLINKERTOP}"

SERVER=`grep mtksgt01 /etc/hosts`
if [ ! -z "${SERVER}" ]; then
	export PATH="/mtkoss/git/1.7.6-rhel-5.5/x86_64/bin:$PATH";
fi

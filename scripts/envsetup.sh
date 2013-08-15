#!/bin/bash
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

function gettop() 
{
	local TOPFILE=tools/ld.mcld/main.cpp
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

function wc_mcld()
{
	local H=`find ${MCLINKERTOP} | grep '\.h'`
	local T=`find ${MCLINKERTOP} | grep '\.tcc'`
	local C=`find ${MCLINKERTOP} | grep '\.cpp'`
	wc ${C} ${H} ${T}
}

#############################
#  Variable Dictionary
export MCLINKERTOP=$(gettop)
export PATH="${MCLINKERTOP}/scripts/bin:$PATH"
if [ -x "${MCLINKERTOP}/debug/ld.mcld" ]; then
	ln -sf ${MCLINKERTOP}/debug/ld.mcld ${MCLINKERTOP}/scripts/bin/MCLinker
fi

alias mk="make -C ${MCLINKERTOP}"


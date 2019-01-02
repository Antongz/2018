#!/bin/bash - 
#===============================================================================
#
#          FILE: extract.sh
# 
#         USAGE: ./extract.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Yi-Ting, Hsieh(a1691807)
#  ORGANIZATION: 
#       CREATED: 03/23/2018 12:23:51 AM
#      REVISION:  ---
#===============================================================================

#echo $2
file="$1"
output="$2"


if [[ -f "$output" ]] 2>&1
then
	rm $output
fi

touch $output

while read line
do
	count=0
	for p in $line
	do
		count=`expr $count + 1`
		
		if (($count==4))
		then		
			echo "$p" >> $output
		fi
	done 

done < $file



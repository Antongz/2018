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
check=real

if [[ -f "$output" ]] 2>&1
then
	rm $output
fi

touch $output

count=0
while read line
do
	if (( $count>9 ))
		then
			echo "" >> $output
			count=0
	fi

	for p in $line
	do
		if [ "$p" = "real" -o "$p" = "java" ]
		then 
			echo "$line" | sed 's/[mrealsjaTtv]//g' | tr -d "	"  >> $output
			if [ "$p" = "real" ]
			then
				count=`expr $count + 1`
			fi
		fi
		
	done 


done < $file



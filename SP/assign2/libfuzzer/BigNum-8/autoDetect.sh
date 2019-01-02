#!/bin/bash - 
#===============================================================================
#
#          FILE: autoDetect.sh
# 
#         USAGE: ./autoDetect.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 09/22/2018 02:04:35 AM
#      REVISION:  ---
#===============================================================================

echo $1 $2

inputDir="$1"
output="$2"


if [[ -f "$output" ]] 2>&1
then
    rm $output
fi

touch $output

for file in $inputDir/id*;
do
    (./calc_afl_asan < $file) > $output
done

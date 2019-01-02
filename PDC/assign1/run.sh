#!/bin/bash
#SBATCH -p batch        	                                # partition (this is the queue your job will be added to)
#SBATCH -N 1               	                                # number of nodes (use a single node)
#SBATCH -n 32              	                                # number of cores (sequential job uses 1 core)
#SBATCH --time=01:00:00    	                                # time allocation, which has the format (D-HH:MM:SS), here set to 1 hour
#SBATCH --mem=4GB         	                                # memory pool for all cores (here set to 4 GB)

#module load Java/1.4.0_121

boolF=0
if [ $# -ne 0 ]
then 
	boolF=1
	file="$1"
	if [ -f $file ] 2>&1
	then
		rm $file
	fi

	touch $file
	chmod 777 $file
fi

i=0
echo "Please enter the n:"
while read line
do
	DemandArray[$i]=$line
	i=`expr $i + 1`
done

echo "Please enter the t:"
read t


size=${#DemandArray[*]}

i=0
j=0
for((i=0;i<size;i++))
do
	
	if (($boolF==1))
	then
		echo "java Test  ${DemandArray[$i]}  $t" >> $file
	else
		echo "java Test  ${DemandArray[$i]}  $t"
	fi
	for ((j=0;j<10;j++))
	do
		if (($boolF==1))
		then
			(time java Test ${DemandArray[$i]} $t) |& tee -a $file
		else
			(time java Test ${DemandArray[$i]} $t)
		fi
	done
	echo -e
done


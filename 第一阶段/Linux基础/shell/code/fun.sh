#!/bin/bash

sum()
{
	sum=`expr $1 + $2`
	
	#echo $sum
	return $sum
}
#ret=`sum 1 2`
#echo $ret
sum 1 2
echo $?

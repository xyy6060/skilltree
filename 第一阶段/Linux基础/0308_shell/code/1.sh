#########################################################################
# File Name: 1.sh
# Author: csgec
# mail: csgec@163.com
# Created Time: Wed 06 Mar 2019 05:13:07 PM PST
#########################################################################
#!/bin/bash
myname="pxl"
#echo $myname

#name=`date`
#echo $name
#name="$myname gfy"
#echo $name
#echo "the num is $#"
#echo $2

#./1
#ret=$?
#echo $ret

array[0]=11
array[1]=12
echo ${array[0]},${array[1]}

BB=(12333333 3 4 5)
echo ${#BB[0]}
echo ${#array[@]}

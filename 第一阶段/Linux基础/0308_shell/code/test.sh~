#!/bin/bash
if [ -f $1 ] ; then
	echo "is regular file"
	case $1 in
	*.c)
		echo "C code file"
		;;
	*.h)
		echo "header file"
		;;
	*.cpp)
		echo "Cpp code file"
		;;
	esac
else
	echo "is not regular file"

fi

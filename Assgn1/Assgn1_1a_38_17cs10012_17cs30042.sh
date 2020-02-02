#!/bin/sh
#echo "$1$2$3"
case "$2" in
	"+")	
		echo "$1 + $3" | bc
		;;
	"-")
		echo "$1 - $3" | bc
		;;
	"/")
		echo "$1 / $3" | bc
		;;
	"%")
		echo "$1 % $3" | bc
		;;
	*)
		echo "$1 * ${@: -1}" | bc
		;;
esac

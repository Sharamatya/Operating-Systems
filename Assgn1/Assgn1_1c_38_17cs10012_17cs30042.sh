#!/bin/sh
gcd()
{
	if [ $1 == 0 ]; then
		echo $2
	else
		k=`expr $2 % $1`
		res=`gcd $k $1`
		echo $res 
	fi
}

ans=0
for i in "$@"
do 
	ans=`gcd $ans $i`
	a=$i
	i=$((i+1))
done
echo $ans
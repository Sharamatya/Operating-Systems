#!/bin/sh
mkdir 1.d.files.out
for i in *.txt ;
 do
 	sort -nr $i > 1.d.files.out/"${i%%.txt}".out 
done
cd 1.d.files.out 
cat *.out | sort -nr > 1.d.out.txt

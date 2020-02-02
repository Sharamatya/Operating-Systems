#!/bin/sh
read col
awk -v c="$col" '{$c=tolower($c);print}' 1d_input.txt > output.txt
cp output.txt 1d_input.txt
rm output.txt
#awk -v r="$col" '{A[$r]++}END{for (i in A) print i,A[i]}' 1d_input.txt > 1e_output_${col}_column.freq
awk -v r="$col" '{print $r}' 1d_input.txt > colm.txt
sort -n colm.txt | uniq -c > sorted.txt
sort -nr -k1 -o sorted.txt sorted.txt
awk '{ print $2,$1'} sorted.txt > 1e_output_${col}_column.freq
# sort -nr -k2 -o 1e_output_${col}_column.freq 1e_output_${col}_column.freq
# cp output.txt 1e_output_${col}_column.freq
rm colm.txt sorted.txt

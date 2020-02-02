#!/bin/sh
awk '{if($1==$2){} else{print $1, $2} }' 1f.graph.edgelist > output1.txt
awk '{if($1>$2){ print $2, $1} else{print $1, $2}}' output1.txt > output2.txt
sort output2.txt | uniq > output3.txt
# awk '{A[$1]++;A[$2]++}END{for (i  in A) print i, A[i]}' output3.txt > output4.txt
# sort -nr -k2  output4.txt > output5.txt
awk '{print $1}' output3.txt > output4.txt
awk '{print $2}' output3.txt >> output4.txt
sort -n output4.txt | uniq -c > output5.txt
sort -nr -k1 output5.txt > output6.txt
awk '{print $2, $1}' output6.txt > output7.txt
head -5 output7.txt > 1f_output_graph.edgelist
rm output1.txt
rm output2.txt
rm output3.txt
rm output4.txt
rm output5.txt
rm output6.txt
rm output7.txt
# awk '{for (i<=5) {print $0}}' output1.txt > 1f_output_graph.edgelist

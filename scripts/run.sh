#!/bin/bash

## nohup bash run.sh &

threads=$(nproc --all)

for t in 20.000000 19.090969 18.223255 17.394980 16.604351 15.849658 15.129267 14.441618 13.785224 13.158664 12.560583 11.989685 11.444735 10.924554 10.428017 9.954047 9.501620 9.069757 8.657523 8.264025 7.888412 7.529872 7.187627 6.860939 6.549098 6.251432 5.967294 5.696072 5.437176 5.190048 4.954153 4.728979 4.514039 4.308869 4.113025 3.926081 3.747635 3.577299 3.414705 3.259502 3.111352 2.969937 2.834948 2.706096 2.583099 2.465693 2.353624 2.246648 2.144534 2.047062 1.954020 1.865207 1.780430 1.699507 1.622262 1.548527 1.478144 1.410960 1.346830 1.285615 1.227181 1.171404 1.118162 1.067340 1.018828 0.972520 0.928318 0.886124 0.845849 0.807403 0.770706 0.735676 0.702238 0.670321 0.639853 0.610771 0.583011 0.556512 0.531218 0.507073 0.484026 0.462026 0.441026 0.420981 0.401847 0.383582 0.366148 0.349506 0.333620 0.318457 0.303982 0.290166 0.276977 0.264388 0.252371 0.240901 0.229951 0.219500 0.209523 0.200000
do
for i in $(seq 1 10)
do
    ./a.out -g 2 -f ../data/n16d5.random.edges -n 1000000 -s $i -w $t -c $t  > log.$t.$i.txt &

    while [ $(jobs|wc -l) -ge $threads ]; do
	sleep 1
    done
done
done

wait
echo $SECONDS

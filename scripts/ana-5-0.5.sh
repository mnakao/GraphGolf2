#!/bin/bash
K=${1:-10}

TEXT=result.txt
LIST="5 4.770477 4.551491 4.342557 4.143214 3.953022 3.77156 3.598428 3.433244 3.275643 3.125276 2.981812 2.844933 2.714338 2.589737 2.470857 2.357433 2.249216 2.145967 2.047458 1.953470 1.863797 1.778240 1.696611 1.618729 1.544422 1.473526 1.405884 1.341348 1.279774 1.221027 1.164976 1.111498 1.060475 1.011795 0.965349 0.921035 0.878755 0.838416 0.799929 0.763209 0.728174 0.694748 0.662856 0.632428 0.603396 0.575698 0.549271 0.524057 0.5 SA1 SA2"

for t in $LIST; do
  for i in $(seq 1 ${K}); do
    grep "ASPL gap" log.$t.$i.txt | awk '{print $16}'
  done | sort > $t
done

paste $LIST > $TEXT

for t in $LIST; do
  for i in $(seq 1 ${K}); do
    grep Steps log.$t.$i.txt | awk '{print $2}'
  done | sort -n > $t
done

echo "" >> $TEXT

paste $LIST >> $TEXT

rm -f $LIST

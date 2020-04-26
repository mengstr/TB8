#!/bin/bash

L=$(sed -n '/^.[0-9]/p' resultp.txt | wc -l)

export set f=19; echo "EQ" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20
export set f=20; echo "GE" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20
export set f=21; echo "GT" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20
export set f=22; echo "NE" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20
export set f=23; echo "LE" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20
export set f=24; echo "LT" && head -n$L resultc.txt  | cut -c1-18,$f > r1.txt && sed -n '/^.[0-9]/p' resultp.txt | cut -c1-18,$f > r2.txt && diff -y --suppress-common-lines r1.txt r2.txt | head -20

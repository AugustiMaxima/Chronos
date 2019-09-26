#!/usr/bin/env bash
prefix="../"
for i in $(ls ../*a)
do
ln -s $i lib${i#"$prefix"}
done

#!/bin/sh
CRFRAMES=/home/jo/jproxy/coco/frames
export CRFRAMES
../coco/cocor $1
cat export.tpl >> proxyc.c

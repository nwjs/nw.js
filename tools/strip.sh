#!/bin/sh
touch -r $2 $1
strip $2
touch -r $1 $2

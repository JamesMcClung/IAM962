#!/bin/bash

args=($@)
args=("${args[@]/#/-DP1A_}")
cmake -B ../build -S .. ${args[@]}
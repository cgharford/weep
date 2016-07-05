#!/bin/bash

echo "----------------------- Auto run file Started -----------------------"
echo "Hi! Start time: $(date)"

clear

echo "Making files"
make clean
make
echo ""

echo "--- Executing weep. Trying practice.cap ---"
time ./weep practice.cap
echo "--- Execution ended ---"

echo "--- Executing weep. Trying cs-test-wep.cap ---"
time ./weep cs-test-wep.cap
echo "--- Execution ended ---"

echo ""
echo "----------------------- Auto run file End -----------------------"


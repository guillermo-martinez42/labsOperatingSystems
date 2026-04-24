#!/bin/bash

echo ""
echo "Building CPU Scheduling Algorithms..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Running CPU Scheduling Algorithms..."
echo ""

./scheduling
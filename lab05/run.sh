#!/bin/bash

echo "Building log processor..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Running log processor..."
./log_processor access.log

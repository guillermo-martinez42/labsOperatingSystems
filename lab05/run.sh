#!/bin/bash

echo "Generating a new access log file..."
python3 access_log_file_generator.py

echo ""
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

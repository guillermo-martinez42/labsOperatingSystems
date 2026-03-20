#!/bin/bash

echo ""
echo "Building Parking Lot Simulation..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Running Parking Lot Simulation..."
./parking_lot

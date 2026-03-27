#!/bin/bash

echo ""
echo "Building Zombie Bridge Crossing Challenge..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Running Zombie Bridge Crossing Challenge..."
./zombie_bridge

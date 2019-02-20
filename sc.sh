#/bin/bash

declare -a numberOfThreads=(1 2 4 8)
declare -a resultions=(10000 20000 30000 40000 50000 60000)
declare -a resizeFactors=(2 3 4 8)

make

echo "Creating images ... (using 8 threads)."
for resolution in "${resultions[@]}"; do
    echo "$resolution x $resolution ... $(./homework1 inputImage$resolution "$resolution" 8)"
done
echo ""

for resolution in "${resultions[@]}"; do
    for resizeFactor in "${resizeFactors[@]}"; do
        echo "Resolution = $resolution x $resolution, resize factor = $resizeFactor."

        for thread in "${numberOfThreads[@]}"; do
            echo "Resizing (threads = $thread) ... $(./homework inputImage$resolution outputImage $resizeFactor $thread)"
        done

        echo ""
    done
done

make clean
rm inputImage* outputImage

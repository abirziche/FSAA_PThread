make homework1

for resolution in "${resultions[@]}"; do
    echo "Resolution = $resolution."

    for thread in "${numberOfThreads[@]}"; do
        echo "Creating image with $thread threads ... $(./homework1 outputImage "$resolution" "$thread")"
    done

    echo ""
done

make clean
rm outputImage

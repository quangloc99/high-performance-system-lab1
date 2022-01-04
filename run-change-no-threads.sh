for num_threads in {1..4}; do
    echo "=== num_threads = $num_threads"
    OMP_NUM_THREADS=$num_threads ./main "num-threads-$num_threads"
done

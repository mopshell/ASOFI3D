#!/usr/bin/env bash
# Regression test 09.
# Check option readmod=1 (reading model from binary files).
# The test is organized as follows:
# 1. Run simulation in which model is generated on-the-fly and write the model
#    on disk.
# 2. Run simulation in which model is read from files written in the previous
#    step.
# 3. Compare that the seismograms are close to each other.
. tests/functions.sh

TEST_PATH="tests/fixtures/test_09"
TEST_ID="TEST_09"

# Setup function prepares environment for the test (creates directories).
setup

# Copy test model.
cp -R ${TEST_PATH}/* tmp

cd tmp > /dev/null

# Run code.
logfile="ASOFI3D-readmod=-1.log"
echo "${TEST_ID}: Running solver. Output is captured to tmp/$logfile"
mpirun -n 16 ../bin/sofi3D "in_and_out/sofi3D-readmod=-1.json" > "$logfile" &
task_id=$!
animate_progress $task_id "${TEST_ID}: Running solver"

wait $task_id
code=$?

if [ "$code" -ne "0" ]; then
    echo "${TEST_ID}: FAIL Running ASOFI3D failed" > /dev/stderr
    exit 1
fi

# Run code.
logfile="ASOFI3D-readmod=1.log"
echo "${TEST_ID}: Running solver. Output is captured to tmp/$logfile"
mpirun -n 16 ../bin/sofi3D "in_and_out/sofi3D-readmod=1.json" > "$logfile" &
task_id=$!
animate_progress $task_id "${TEST_ID}: Running solver"

wait $task_id
code=$?

if [ "$code" -ne "0" ]; then
    echo "${TEST_ID}: FAIL Running ASOFI3D failed" > /dev/stderr
    exit 1
fi

cd ..

# Convert seismograms in SEG-Y format to the Madagascar RSF format.
convert_segy_to_rsf tmp/su/test-readmod=-1_vx.sgy
convert_segy_to_rsf tmp/su/test-readmod=1_vx.sgy

# Read the files.
# Compare with the recorded output.
tests/compare_datasets.py \
    tmp/su/test-readmod=-1_vx.rsf tmp/su/test-readmod=1_vx.rsf \
    --rtol=1e-15 --atol=1e-15
result=$?
if [ "$result" -ne "0" ]; then
    echo "${TEST_ID}: Traces differ" > /dev/stderr
    exit 1
fi

# Teardown.
# Restore default model.
git checkout -- ${MODEL}

echo "${TEST_ID}: PASS"

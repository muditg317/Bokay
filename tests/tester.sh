set -e

# Bokay="$(realpath ..)/Bokay"

# use full paths
# echo $@
build/tests/test-engine --engine build/engines/cpp/bokay-cpp --test-dir tests $@

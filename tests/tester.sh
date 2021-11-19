set -e

Bokay="$(realpath ..)/Bokay"

# use full paths
# echo $@
$Bokay/build/tests/test-engine --engine $Bokay/build/engines/cpp/bokay-cpp --test-dir $Bokay/tests $@

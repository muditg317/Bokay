set -e

usage() {
  echo "usage: . vcpkg.sh"
}

sourced=0
if [ -n "$ZSH_EVAL_CONTEXT" ]; then
  case $ZSH_EVAL_CONTEXT in *:file) sourced=1;; esac
elif [ -n "$BASH_VERSION" ]; then
  (return 0 2>/dev/null) && sourced=1
else # All other shells: examine $0 for known shell binary filenames
  # Detects `sh` and `dash`; add additional shell filenames as needed.
  echo "Please use bash or zsh for best experience with development on Bokay"
fi
if [ "$sourced" != "1" ]; then
  echo "You must use source for vcpkg.sh"
  usage
  exit # not return because "you can only return from a funtion or sourced script"
fi

# echo "Checking vcpkg..."
if command -v vcpkg >/dev/null; then
  # echo "vcpkg is installed, configuring VCPKG_ROOT..."
  # check if VCPKG_ROOT is already defined
  if [ ! -z "$VCPKG_ROOT" ]; then
    # echo "VCPKG_ROOT is already defined as $VCPKG_ROOT"
    echo "VCPKG_ROOT: $VCPKG_ROOT"
  else
    echo "VCPKG_ROOT is not defined, setting up..."
    vcpkg_path=$(realpath $(whereis vcpkg))
    vcpkg_arr=(${vcpkg_path//;/ })
    vcpkg_dir=${vcpkg_arr[1]}
    export VCPKG_ROOT=$(dirname $vcpkg_dir)
    echo "Set VCPKG_ROOT to $VCPKG_ROOT"
  fi
else
  echo "You need to install vcpkg!"
fi

#!/bin/dash

dir=$(cd $(dirname $(readlink -f $0)); pwd)
cd $dir

version=$(head -n 1 Changelog | egrep -o '[0-9]+\.[0-9]+\.[0-9]+')
echo "Version is ${version}."

make clean

echo "Saving orig."
(
    cd ..
    tar=libyacjp_${version}.orig.tar.bz2
    exec tar cfj $tar --exclude-vcs $(basename $dir)
)

echo "Starting release."
exec make release

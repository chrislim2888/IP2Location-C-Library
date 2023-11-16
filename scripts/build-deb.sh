#!/bin/bash

VERSION=$(grep -Eo '\([0-9.]+\)' ../debian/changelog | grep -Eo '[0-9.]+')

rm -rf ../dist
mkdir -p ../dist/DEBIAN/
mkdir -p ../dist/usr/local/bin/
mkdir -p ../dist/usr/local/lib/
cp ../debian/control ../dist/DEBIAN/
cd ..
autoreconf -i -v --force
./configure
make
cp ip2location ./dist/usr/local/bin/
cp libIP2Location/.libs/* ./dist/usr/local/lib/

sed -i '/^$/d' ./dist/DEBIAN/control
sed -i '/^Depends/d' ./dist/DEBIAN/control
sed -i 's/Architecture:.*/Architecture: '$(dpkg --print-architecture)'/' ./dist/DEBIAN/control
echo "Version: $VERSION" >> ./dist/DEBIAN/control
dpkg-deb -Zgzip --build dist ip2location-$VERSION.deb
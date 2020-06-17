#!/bin/sh

# Compile
mkdir build
cd build
cmake ..
make
cd ..

# Make Folders
mkdir -p build/customtwitchtranscodeclient_debian/DEBIAN
mkdir -p build/customtwitchtranscodeclient_debian/usr/bin
mkdir -p build/customtwitchtranscodeclient_debian/usr/share/applications

# Copy files
cp control_$1 build/customtwitchtranscodeclient_debian/DEBIAN/control
cp build/CustomTwitchTranscodeClient build/customtwitchtranscodeclient_debian/usr/bin/customtwitchtranscodeclient
cp customtwitchtranscodeclient.desktop build/customtwitchtranscodeclient_debian/usr/share/applications/customtwitchtranscodeclient.desktop

# Make Package
dpkg-deb --build build/customtwitchtranscodeclient_debian
mv build/customtwitchtranscodeclient_debian.deb customtwitchtranscodeclient_debian_$1.deb

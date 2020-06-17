# CustomTwitchTranscodeClient
Downloads transcoded twitch stream data from custom transcode server and plays it<br />

Version 0.9 Alpha<br />


## Installation
Check **[Releases](https://github.com/Aslan2142/CustomTwitchTranscodeClient/releases)** section


## How To Use
Setup settings.json and launch program<br />


## How To Compile and Make Package

### Debian Linux:
Run **makedebpkg.sh** script with parameter {amd64/arm64}<br />
This will make .deb package<br />
Install with **dpkg -i customtwitchtranscodeclient_debian_{amd64/arm64}.deb**<br />

Compilation Example: **makedebpkg.sh amd64**<br />
Installation Example: **sudo dpkg -i customtwitchtranscodeclient_debian_amd64.deb**<br />

### Arch Linux:
Run **makearchpkg.sh** script with parameter {amd64/arm64}<br />
This will make .tar.xz package<br />
Install with **pacman -U customtwitchtranscodeclient-{version}-{release}-{x86_64/aarch64}.pkg.tar.xz**<br />

Compilation Example: **makearchpkg.sh amd64**<br />
Installation Example: **sudo pacman -U customtwitchtranscodeclient-1.0.0-1-x86_64.pkg.tar.xz**<br />

### Windows:
Use QT Creator

### Mac OS:
Use QT Creator

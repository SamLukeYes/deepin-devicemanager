## deepin-devicemanager
Device Manager is a handy tool for viewing hardware information and managing the devices.

设备管理器是查看、管理硬件设备的工具软件。

#### Core
The GUI is based on [DTK](https://github.com/linuxdeepin/dtkwidget), Qt (Supported >= 5.12).

Build Depends: `debhelper (>= 11), pkg-config, qt5-qmake, qt5-default, libdtkwidget-dev, libdtkgui-dev, qtbase5-private-dev, libdframeworkdbus-dev, libcups2-dev`

Runtime Depends: `libc6 (>= 2.14), libdframeworkdbus2, libdtkcore5 (>= 5.0.9+r2+g0dc0efd), libdtkgui5 (>= 5.0.9+r0+g773b21d), libdtkwidget5, libgcc1 (>= 1:3.0), libgl1, libqt5core5a (>= 5.11.0~rc1), libqt5dbus5 (>= 5.0.2), libqt5gui5 (>= 5.8.0), libqt5svg5 (>= 5.6.0~beta), libqt5widgets5 (>= 5.11.0~rc1), libqt5xml5 (>= 5.0.2), libstdc++6 (>= 6), qtbase-abi-5-11-3, libcups2 (>= 1.7.0), smartmontools, dmidecode[i386 arm64 amd64], x11-xserver-utils, hwinfo, upower, deepin-shortcut-viewer, lshw, cups`

#### Special Link
- [linux-hardware.org](https://linux-hardware.org/)

#### Dependencies
- [dmidecode](http://www.nongnu.org/dmidecode/)
- [lshw](https://ezix.org/project/wiki/HardwareLiSter)
- [lspci](https://github.com/linuxhw/LsPCI)
- lsb_release
- hciconfig
- bluetoothctl
- [hwinfo](https://github.com/linuxhw/HWInfo)
- lsusb
- lscpu
- [smartctl](https://www.smartmontools.org/)
- [xrandr](https://www.x.org/wiki/Projects/XRandR/)
- [CUPS](https://www.cups.org/index.html)
- [upower](https://upower.freedesktop.org/)
- deepin-shortcut-viewer

#### Thridparty Libraries (source code)
- [docx](https://github.com/lpxxn/docx)
- [QtXlsx](http://qtxlsx.debao.me)

#### Installation
```sh
sudo apt-get install deepin-devicemanager
```

#### Build from Source
```sh
mkdir build
cd build
qmake ..
make
```

#### Usage
deepin-devicemanager depends on polkit. If it's built from source code, 
you have to run the following commands before using it.

```sh
sudo cp authenticateProxy/policy/com.deepin.pkexec.deepin-devicemanager-authenticateProxy.policy /usr/share/polkit-1/actions/
sudo cp build-all-unknown-Debug/authenticateProxy/deepin-devicemanager-authenticateProxy /usr/bin
```

To run in Qt Creator, you should set [Project] [Run Settings] [Run] [Run configuration] to src from 2 projects authenticateProxy and src.


# seL4 SDN Controller Implementation for Master Thesis

This is the repository for the implementation of the master thesis project conducted by Olof Holmberg.

## System setup

This project was conducted using Ubuntu 20.04. To setup the system follow these steps:

Download and install ubuntu-20.04.2.0-desktop-amd64.iso: https://releases.ubuntu.com/20.04/

Estimated free disk space required is around 100-150 GB total. 

First change root password on the Ubuntu installation:

```
sudo passwd root
```

Also change time and keyboard layout of the VM (CEST (UTC +2) and Swedish).

Then install the following packages:

```
sudo apt update
sudo apt install autoconf
sudo apt install automake
sudo apt install autotools-dev
sudo apt install bc
sudo apt install bison
sudo apt install build-essential
sudo apt install ccache
sudo apt install clang
sudo apt install cmake
sudo apt install cmake-curses-gui
sudo apt install curl
sudo apt install flex
sudo apt install g++-aarch64-linux-gnu
sudo apt install g++-arm-linux-gnueabi
sudo apt install g++-arm-linux-gnueabihf
sudo apt install g++-multilib
sudo apt install gawk
sudo apt install gcc-aarch64-linux-gnu
sudo apt install gcc-arm-linux-gnueabi
sudo apt install gcc-arm-linux-gnueabihf
sudo apt install gcc-multilib
sudo apt install gdb
sudo apt install git
sudo apt install gperf
sudo apt install haskell-stack
sudo apt install libclang-dev
sudo apt install libcunit1-dev
sudo apt install libexpat-dev
sudo apt install libglib2.0-dev
sudo apt install libgmp-dev
sudo apt install libhugetlbfs-bin
sudo apt install libmpc-dev
sudo apt install libmpfr-dev
sudo apt install libpixman-1-dev
sudo apt install libssl-dev
sudo apt install libsqlite3-dev
sudo apt install libtool
sudo apt install libvirt-clients
sudo apt install libvirt-daemon-system
sudo apt install libxml2-utils
sudo apt install make
sudo apt install ncurses-dev
sudo apt install ninja-build
sudo apt install patchutils
sudo apt install protobuf-compiler
sudo apt install python-dev
curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py
sudo python2 get-pip.py
sudo apt install python-protobuf
sudo apt install python3-dev
sudo apt install python3-pip
sudo apt install qemu
sudo apt install qemu-kvm
sudo apt install qemu-system
sudo apt install qemu-system-arm
sudo apt install qemu-system-misc
sudo apt install qemu-system-x86
sudo apt install rsync
sudo apt install texinfo
sudo apt install u-boot-tools
sudo apt install zlib1g-dev
```

Then install the required Python Dependencies (has to be done for both python2 and python3):

```
pip3 install --user setuptools
pip3 install --user sel4-deps
pip3 install --user camkes-deps
pip3 install --user aenum
pip3 install --user pyelftools
pip install --user setuptools
pip install --user sel4-deps
pip install --user camkes-deps
pip install --user aenum
pip install --user pyelftools
```

Then use the convenience script (https://get.docker.com/) to install Docker:

```
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
```

Then add your user to the docker group:

```
sudo usermod -aG docker $(whoami)
```

**Important: The machine has to be restarted to apply group changes!**

Then install Repo:

```
mkdir ~/bin
PATH=~/bin:$PATH
curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod a+x ~/bin/repo
```

Add ninja to PATH:

```
PATH=/usr/bin/ninja:$PATH
```

## Get the code

The code and all relevant support repositories can be fetched using the repo tool:

```
mkdir master-thesis
cd master-thesis
repo init -u https://github.com/olofholmberg/master-thesis-manifest.git
repo sync
```

## Build the Yocto artifacts:

This step is both time and space consuming.
Yocto is also not entierly bug free, so errors can still occur during build.
Rerunning the build script usually resolves errors encountered when building.

```
cd projects/master-thesis-vm-images
./build.sh
```

## Building the project

Currently supported platforms:
* QEMU ARM virt machine

The following commands builds the project for the QEMU ARM virt machine.
Run the commands in the master-thesis folder:

```
mkdir build
cd build
../init-build.sh -DPLATFORM=qemu-arm-virt -DSIMULATION=1
ninja
```

## Usage

Once the buildroot has booted it is possible to login as 'root' with password 'root'.
The dataports connecting the VM and the component has to be initialized:

```
/etc/init.d/S90crossvm_module_init
```

## Provide network to the guest VM

In order to pass a network device to the guest VM in seL4 the simulation has to be started with the following command:

```
sudo ./simulate --extra-qemu-args="-netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no -device virtio-net,netdev=mynet0,mac=52:55:00:d1:55:01,disable-modern=on,disable-legacy=off"
```

## Set up network on the host

On the host running the QEMU simulation, run the following commands:

```
sudo ip addr add 192.168.0.11/24 dev tap0
sudo ip link set dev tap0 up
```

It should now be possible to ping the simulation host (192.168.0.11) from the VM and to ping the VM (192.168.0.10) from the simulation host.


## Running the ovs-testcontroller

In the guest VM in seL4 run:

```
ovs-testcontroller ptcp:
```

This will run the testcontroller on port 6653.

Then on the host that is running the seL4 simulation run:

```
sudo mn --topo single,3 --mac --switch ovsk --controller remote,ip=192.168.0.10,port=6653
```

And in mininet run:

```
xterm h1 h2 h3
```

Then the end hosts in mininet should be able to find eachother using the openvswitch controller that is running in the guest VM of seL4.
Test this step by pinging from one mininet host to another. IP addresses are 10.0.0.1 for h1, 10.0.0.2 for h2 and 10.0.0.3 for h3.


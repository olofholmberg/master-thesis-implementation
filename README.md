# seL4 SDN Controller Implementation for Master Thesis

This is the repository for the implementation of the master thesis project conducted by Olof Holmberg.

## System setup

This project was conducted using Ubuntu 20.04. To setup the system follow these steps:

Download and install ubuntu-20.04.2.0-desktop-amd64.iso: https://releases.ubuntu.com/20.04/

Estimated free disk space required is around 100 GB total. Depends mainly on the Docker container sizes but they can be quite large.

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

Then aquire a running build environment for seL4 and Camkes:

Clone the repo in preferred folder (Desktop):

```
git clone https://github.com/SEL4PROJ/seL4-CAmkES-L4v-dockerfiles.git
cd seL4-CAmkES-L4v-dockerfiles
```

Add alias for starting a container:

```
echo $'alias container=\'make -C /home/olof/Desktop/seL4-CAmkES-L4v-dockerfiles user HOST_DIR=$(pwd)\'' >> ~/.bashrc
```

Then include RISC compilers in the container:

```
./build.sh -b sel4 -s riscv
alias sel4_riscv_container='make -C /home/olof/Desktop/seL4-CAmkES-L4v-dockerfiles user_sel4-riscv HOST_DIR=$(pwd)'
```

Build the GCC toolchain for RISC-V (the "make linux" command takes a lot of time, likely several hours):

Clone the repo in preferred folder (Desktop):

```
git clone https://github.com/riscv/riscv-gnu-toolchain.git
cd riscv-gnu-toolchain
git submodule update --init --recursive
export RISCV=/opt/riscv
./configure --prefix="${RISCV}" --enable-multilib
sudo make linux
```

After build add RISC-V bin to PATH:

```
PATH=$RISCV/bin:$PATH
```

Add ninja to PATH:

```
PATH=/usr/bin/ninja:$PATH
```

## Aquire and setup git token

To ease the use of https and avoid ssh a token can be generated and used instead of the password.

Go to github -> settings -> developer settings -> personal access tokens and generate a new token.

Then enable storage of git credentials before using it:

```
git config --global credential.helper store
```

And now the https access of the repo should work.

## Get the code

This step requires access to both this repository and the manifest repository: https://github.com/olofholmberg/master-thesis-sel4-manifest

```
mkdir master-thesis-implementation
cd master-thesis-implementation
repo init -u https://github.com/olofholmberg/master-thesis-sel4-manifest.git
repo sync
```

## Building the project

Currently supported platforms:
* QEMU ARM virt machine

The following example builds the project for the QEMU ARM virt machine:

```
mkdir build
cd build
../init-build.sh -DPLATFORM=qemu-arm-virt -DSIMULATION=1
ninja
```

## Usage

Once the buildroot has booted it is possible to login as 'root' and run the program:

```
/etc/init.d/S91crossvm_test
```

## Set up network in the seL4 guest

First start the simulation with:

```
sudo ./simulate --extra-qemu-args="-netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no -device virtio-net,netdev=mynet0,mac=52:55:00:d1:55:01,disable-modern=on,disable-legacy=off"
```

Then add an ip address to the guest interface:

```
vi /etc/network/interfaces
```

And add the following:

```
auto eth0
iface eth0 inet static
address 192.168.0.10
netmask 255.255.255.0
```

Then save and run:

```
ifup eth0
```

## Set up network on the host

Run the tap.sh script or:

```
sudo ip addr add 192.168.0.11/24 dev tap0
sudo ip link set dev tap0 up
```

It should now be possible to ping the host (192.168.0.11) from the VM and to ping the VM (192.168.0.10) from the host.


## Running the ovs-testcontroller

Start the seL4 simulation with virtio device.

Then in the guest in seL4 run:

```
ovs-testcontroller ptcp:
```

This will run the testcontroller on port 6653. **IMPORTANT:** Currently the testcontroller cannot print verbose output since it causes a segmentation fault. Unclear wether this is because of limitations in the seL4 guest or a bug within the controller.

Then on the host that is running seL4 run:

```
sudo mn --topo single,3 --mac --switch ovsk --controller remote,ip=192.168.0.10,port=6653
```

And in mininet run:

```
xterm h1 h2 h3
```

Then the end hosts in mininet should be able to find eachother using the openvswitch controller that is running in the guest of seL4.


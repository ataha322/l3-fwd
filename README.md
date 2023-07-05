# L1 forwarder, DPDK.

* Basic forwarder, requires at least 2 different ports.

* Tested on Ubuntu Server 22.04.2 with 2 virtual NIC devices.

* Compile using `make` in a DPDK environment.

* Example run: `sudo ./build/l1fwd -l 1 -n 4` will configure 1 core and 4 workers via EAL parameters.

* Need to have an even number of ports.

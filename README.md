# L3 forwarder, DPDK.

* Basic forwarder, requires at least 2 different ports.

* Tested on Ubuntu Server 22.04.2 with 2 virtual NIC devices.

* Compile using `make` in a DPDK environment.

* Example run: `sudo ./build/l3fwd -l 1 -n 4 --vdev=net_tap0,mac="00:11:22:33:44:55"` will configure 1 core and 4 workers via EAL parameters and open tap port to receive packets from host machine.

* Scapy script to send packets to forwarder via TAP interface is `send_packet.py`

* Need to have an even number of ports.

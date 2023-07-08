# L3 forwarder, DPDK.

* Basic forwarder. RT lookup is EM. Last RT entry is default gateway.

* Tested on Ubuntu Server 22.04.2 with 2 virtual QEMU NIC devices and one TAP interface provided by EAL.

* Compile using `make` in a DPDK environment.

* Fill in routing table in `rtable.csv`

* Before running configure hugetables and bind NICs to DPDK. Can simply run script `boot_setup.sh` as root.

* Example run: `sudo ./build/l3fwd -l 1 -n 4 --vdev=net_tap0,mac="00:11:22:33:44:55"` will configure 1 core and 4 workers via EAL parameters and open tap port to receive packets from host machine.

* Scapy script to send packets to forwarder via TAP interface is `send_packet.py`

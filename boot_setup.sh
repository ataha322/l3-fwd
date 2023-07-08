mkdir -p /dev/hugepages
mountpoint -q /dev/hugepages || mount -t hugetlbfs nodev /dev/hugepages
echo 64 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages

ifconfig enp2s0 down
ifconfig enp3s0 down

modprobe uio
modprobe uio_pci_generic

dpdk-devbind.py -b uio_pci_generic 02:00.0
dpdk-devbind.py -b uio_pci_generic 03:00.0


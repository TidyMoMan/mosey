sudo ip link set can0 down

sudo ip link set can0 txqueuelen 2000

sudo ip link set can0 up type can bitrate 1000000 sample-point 0.750

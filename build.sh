source /opt/poky/2.1.3/environment-setup-cortexa9hf-neon-poky-linux-gnueabi

arm-poky-linux-gnueabi-g++ --std=gnu++11 -march=armv7-a -marm -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=/opt/poky/2.1.3/sysroots/cortexa9hf-neon-poky-linux-gnueabi -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -o swipe-capture main.cpp

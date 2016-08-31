This is a simple program to cycle between predefined combinations of LED levels ("colors") with crossfading and delays.
It supports any number of LED types, but one per pin.

- [Video demo >>](https://uoowuo.github.com/avr-ledcontrol/)

It was written for ATtiny45, but should be easily portable to other devices.

- [Physical setup photo >>](https://raw.githubusercontent.com/uoowuo/avr-ledcontrol/master/setup.jpg)

Left to right, top to bottom are 12 V power source, three MOSFETs with heatsinks (nevermind one of them is different), ATtiny45, a 7805 linear regulator and LED strip wires. My LED strip uses 12 V power, a single VCC (black+white wire in the picture) and three GNDs for different colors.
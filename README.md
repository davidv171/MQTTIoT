# MQTTIoT
MQTT IoT solution containing C code for placeholder sensor code and Android client code.
## Sources ##
Most of the library files are from:  https://os.mbed.com/teams/mqtt/code/MQTTPacket/
The chip code for sam4sd32c is authored by dr. Gregor Donaj, who is a professor at FERI Maribor University.

## What it should do ##
Takes the temperature retrieved from the sensor and sends it to a HiveMQ broker. After that the Android client subscribes to the broker and receives the temperature, which it then displays on the screen.

## What it does ##
Sends a "Hello World" type string over TCP in an MQTT format.

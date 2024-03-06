# Powersupply

The powersupply for my gardenproject considers different ideas

# From mains current

## Current Project

I am using an old laptop power supply and transfer the power to the outside. The voltage is 20v and I can use relatively thin cables to get the power to the outside without significant voltage drop. I calculate a drop between 0.4V and 1V for 3 meters cable and an additional 0,4v for the connector (0.4Ω) leaving me with a minimum of 18.6V. The measurement showed that the voltage drop is:
* 10 Watts 0.4V
* 20 Watts 0.8V
* 30 Watts 1.2V

I was not able to measure higher consumption, since my load does not support more watts. These values are quite good since the connector is already included. The power supply has a total power of 90 watts, but I guess this would be too much for the cables since this is already 4.5 Amps at 20V.

On the outside is a power conversion box using step-down modules to create 12v for pumps and 5.3V for microcontrollers which will then be brought down to 3.3V directly at the microcontroller.

### Voltages

So why these voltages?
* 12V is the optimal voltage for bigger pumps and fans. I may reduce the voltage in case the pumps/fans work with lower voltages as well.
* 5.3V is the maximum for USB and after going an USB Connector and some cable there is still enough voltage. An official Raspberry PI power supply has 5.1V and directly connects to the board connector. We have a second connector (USB-A) in between.

### Step Down

The step down modules have an efficiency for 10 watts of:

* 71%-74% for 20V to 5.3V at a maximum temperature of 74C°
* 78%-87% for 20V to 12V at a maximum temperature of 44C°

So the temperature of the 5.3V step down is already critical, but hopefully the 10 Watts are not needed on the 5.3V. Maybe a possibility to add some monitoring for power and temperature here?

### Connector

I am using a waterthight connector designed for automotive use. They are available from 1-6 pins.

I use the 6 pin version to make sure I don't accidentially plug it into another connector used later in the project. So the 6 pin will be only for power supply and have the following layout:

* 20V GND
* 12V GND
* 5V GND
* 5V +
* 12V +
* 20V +

Having 3.3V on the connector is not needed since the voltage drop from the connection alone adds too much instability. I will always to the step down to 3.3 directly next to the microcontroller.

# Solar Power

I have done experiments. Documentation follows

# How to distribute power

When there are some distances to cover there are also some challenges:

* You have to lay wires
* Long cables have too much resistance and the voltage drops significantly
* Each connector adds additional resistance and voltage drop
* Different parts use different voltages:
    * 3.3V microcontrollers
    * 5.0V some sensors and actuators like relays
    * 12.0V fans, pumps ...
    * 110/220V Indoor devices used outdoors (my aquarium air pump)

## Mains power

So the easiest way is to use mains power and a small mains adapter for the use case. Well there are 

Advantages:

* Stable power supply 

Disadvantages:

* Mains current is difficult to handle outside and may have a short circuit when exposed to rain
* Waterproof power distribution is a challenge
* Bulky connectors
* Potentially dangerous

So it looks like the disadvantages are not worth to follow this except where it is not avoidable.

## Use the needed power and distribute it

Transform the voltage down to the needed and use this to distribute the power

Advantages:

* Safe
* Easy to add more devices
* Relatively stable

Disadvantages:

* Multiple wire networks needed for multiple voltages
* Voltage drop is quite high and can be about 1% per meter per watt. An ESP can use this amount of power when connecting to Wifi. Pumps use even more.

## Use a medium voltage (12v-20v)

Another aproach is to use a medium voltage. Old laptop power supplys have often about 20v and can be reused. This voltage is distributed and transformed to the needed voltage at the target location with a step down module.

Advantages:

* Safe
* Easy to add more devices
* Multiple voltages possible with one power supply
* Stable

Disadvantages:

* Step down module has additional power loss (efficiency 60-90%)
* Multiple step-down modules needed

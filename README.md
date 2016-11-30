# AVRcockpit

AVRcockpit Development System for Racing Simulators

Harald W. Leschner, DK6YF - 2015  H9-Laboratory Ltd. 

hari@h9l.net    or visit    labor.h9l.net

Licensed under the GPL license, see LICENCE for details.

All text above must be included in any redistribution.



         __      __  _____
    /\   \ \    / / |  __ \			   __         _ __
   /  \   \ \  / /  | |__) |	  _________  _____/ /  ____  (_) /_
  / /\ \   \ \/ /   |  _  /	 / ___/ __ \/ ___/ //_/ __ \/ / __/
 / ____ \   \  /    | | \ \	/ /__/ /_/ / /__/ ,< / /_/ / / /_
/_/    \_\   \/     |_|  \_\	\___/\____/\___/_/|_/ .___/_/\__/
			                           /_/


# Introduction
This project is intended to provide the link between a car racing simulator and your
personal simulation hardware like dashboard, gauges, switches and so on...

Basically the simulation machine sends telemetric data over UDP to a RaspberryPi wich
prepares the data for a series of AVR microcontrollers connected through I²C and do the
electronic drive of all gadgetery and sort of things.

In my case I do have a tachometer and speedometer from a KAWASAKI ZX9R which is a complete
digital controlled instrument cluster going up to 300 km/h :)



# FILES
The file structure represents the layers and hardware running the code. All plugin files
are located on a pc, all firmware runs on avr, etc...

/avr 		AVR ATmega firmware(s) for gauges and lights control

/pc 		rFactor plugin transmitting telemetry info

/raspberry 	server application transferring data between PC and I²C

/doc 		is everything you need to know about wiring, connections

/dev 		is development code to test functionality

/hardware 	is construction files like AutoCAD drawings and schematics

/common 	is definition files common to all executables and firmwares



# TRANSMISSION PROTOCOL

The basic transmission protocol relies on a serial stream of values coded in ASCII:

STX = Start of transmission
NR	= Data Paket number
RS 	= Record Separator
ETX = End of transmission

Then we have the "WHAT", "TYPE" and "VALUE":

WHAT is a identification code associated to a telemetry data unit (e.g. RPM)

TYPE is the data type identification (char, integer, float, ...)

VALUE is the value itself coded as TYPE (byte representation of the variable)


The data transmission starts like with the first tuple of data:

STX | NR | RS | WHAT | TYPE | VALUE 		: Recors items / single data stream packet
  1 |  2 |  1 |    2 |    1 |  variable 	: bytes length

next records follows with RS:

         | RS | WHAT | TYPE | VALUE
		 | RS | WHAT | TYPE | VALUE
		 | RS | WHAT | TYPE | VALUE
		 | RS | WHAT | TYPE | VALUE

If no more tuples follow, then we terminate with:

| ETX





# SCHEMATIC & ELECTRONICS

 ULN2003	-> ROWS 	-> GND
 ATmega128 5V+ 	-> COLUMNS

 UDN2981 12V+ 	-> Lights 	-> GND 

 12V+		-> BC337	-> TACHO + 10K + 1N4148 + Pullups
















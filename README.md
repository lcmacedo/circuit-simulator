# circuit-simulator
Simulator for frequency response analysis of electronic circuits 

This program computes the bias point of the circuit using the Newton-Raphson method and then, calculates the frequency response with a steady state analysis.

The following electric and electronic components are supported: resistor, inductor, capacitor, transformer, voltage and current sources (dependent or independent), operational amplifiers, diodes and bipolar transistors.

The program takes as input a netlist file with a .net extension describing a circuit. And its output is a text file with the .tab extension containing a table that can be used to plot a frequency response graph.

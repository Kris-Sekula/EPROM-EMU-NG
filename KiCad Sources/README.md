KiCad project files.

One note: Most of the pictures on my page and here are based on ver v1.0 of the PCB. After I built the first 5 prototypes, I discovered an issue with the PCB layout, one of the shift registers (U11) was getting onto a CMOS latch-up state, even though I had a few bypass caps around the PCB, I was still occasionally getting the issues. I fixed the prototypes by "bodging" a few connections at the bottom of the pcb.

In version 1.4 I re-routed the power and improved the PCB design, so what you see here on GitHub is PCB v1.4 desing files, but most pictures are still v1.0

In version 1.6 I added some component description on silkscreen, no changes to tracks routing.

1.7 intoduces a minor fix for a desing flaw as described in the ERRATA document - currently waiting for the PCBs before I recommnend moving to this new desing. Since I was re-ordering the PCBs I also enlarged the holes for J1 as depeding on vendor of the connector in some cases the fit was very tight.

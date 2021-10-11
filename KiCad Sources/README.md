KiCad project files.

Version 2.2: Reshuffled component designations for pcb v1.9c and 2.2c to keep them in sync - help to keep the BoM alligned... only delta is the SRAM

Version 2.1: Moved from using 2x 32k SRAM to single 128k SRAM (64k usable). Firware and software is the same for both.

Version 1.9: Added pull-up resistors to /WE and EN_RST, helps prevent SRAM corruption during Arduino reset (needed only when doing partial uploads)

Version 1.8b: Fixed the footprint for Q1-2N4401 and Q2-2N4403, see Errata document for more details.

Version 1.8: I re-shuffled resistor networks, added RN3 to offer optional pull-up resistors for address line A0-A7, RN2 should be populated, RN1 and RN3 are optional. Added Q3 for my experiments, but there is no need to populate that transistor.

Version 1.7: Minor fix for a desing flaw as described in the ERRATA document. I also enlarged the holes for J1 as depeding on vendor of the connector in some cases the fit was very tight.

Version 1.6: Added some component description on silkscreen, no changes to tracks routing.

Version 1.4: I re-routed the power and improved the PCB design.

Version 1.0: After I built the first 5 prototypes, I discovered an issue with the PCB layout, one of the shift registers (U11) was getting onto a CMOS latch-up state, even though I had a few bypass caps around the PCB, I was still occasionally getting the issues. I fixed the prototypes by "bodging" a few connections at the bottom of the pcb.

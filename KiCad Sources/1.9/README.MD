In this version of the design, I'm adding pull-up resistors to the /WE and the EN_RST lines. 
It helps to handle the state of the emulator when Arduino resets, this prevents the memory content corruption when we re-connect to the Arduino,
remember Arduino Nano has the feature where it resets as soon as you open the serial port. 
All of this is useful for the new software that I'm working on - it allows you to cache the data and only upload new/changed sectors.

Consider this PCB version verified as I already had boards made by PCBWay and they work fine.

This is the last version for the design with 2x 32k SRAMs ... I'm now moving to a new design that is using a single SRAM chip (see the 2.1 folder).

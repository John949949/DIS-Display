# DIS-Display
Displaying ECU data on an AUDI DIS

This project uses an Aduino (mega) and 2 CAN shields to read 'Measuring Block' data from the engine ECU and display it textually on the main area of the DIS.  It was developed and tested on Mk2 Audi TT but may work on other Audi / VW models that use a BNS 5.0 or RNS/E and a monchrome DIS. It should also be relatively easy to adapt to display different data or indeed data from other ECUs.

Thee are some pictures of the display and the hardware installation on the TT Forum.

There is a description of how it works in the file Introduction.TXT and an example of the serial output (with annotations) that it produces showing the sent and received CAN messages.

It works by impersonating the DIS and a Diagnostic Unit (like VCDS) so there are some limitations if you try to use it as the same time as these systems.  You can pause it to use a diagnostic unit or the standard trip computer but you cannot have both this system and the RNS working at the same time.

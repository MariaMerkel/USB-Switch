# USB Switch

The software and PCB (EasyEDA Format) in this repository allows the interception of the external control button for the (UGREEN USB Switch)[https://amzn.eu/d/1z2Qvjh] in order to switch a monitor input in addition to the USB switch on button press.

## Current Limitations
- There is no installer package yet so installation is complicated. The driver package needs to be signed and installed, the USBSwitchSwitcher executable added to autostart and the USBSwitchButton executable needs to be installed as a service.
- The service cannot currently be stopped. This is being fixed.
- The PCB isn't commercially available so you have to order it from a company like JLCPCB. You can generate gerber files by importing the PCB.json into EasyEDA.

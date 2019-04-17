## Preface
I'm working on a beehive monitoring system, which is collecting data daily from each hive in our apiary.  With the analysis of the collected data I can remotely observe the status of the hive or predict swarming.

## Sensor prototype
This project purpose was to design and build a prototype of a slave module in the system. Each hive will have a slave unit which measures the temperature and humidity in the hive. Also, it is recording a small segment of the bee's buzzing.  The slave units communicate with one master unit through RS-485. 
I designed an extension board for the EFM32 Giant Gecko Starter Kit to achieve the task. A python script on my PC represented the master unit. I had a fault with the RS485 - USB converter, but the other parts worked properly. However, it needs a lot of improvements. 

The repository contains the main source, schematic and the PCB Gerber files.

## Finished unit

Here are some pictures of the prototype. I hand soldered the components.

![IMG_20181126_225740](/home/tamasurfi/dev/ref/IMG_20181126_225740.jpg)

![IMG_20181126_225727](/home/tamasurfi/dev/ref/IMG_20181126_225727.jpg)

![IMG_20181126_225938](/home/tamasurfi/dev/ref/IMG_20181126_225938.jpg)
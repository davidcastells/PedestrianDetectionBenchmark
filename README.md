Pedestrian Detection Benchmark
------------------------------

We tried to find an OpenSource HOG-based pedestrian detection software
and we didn't find any written in C/C++, so we decided to write one.

We use the caltech pedestrian detection benchmark data to test our system.

Prerequisites
-------------

You need a system with ...

Executables
- wget (to download files from the web)
- tar (to decompress tar files, since Caltech uses tar.gz files for their datasets)
- unzip (to decompress zip files, since Caltech uses zip files for annotations)

Libraries
- libjpeg (for JPEG image decompressing, since Caltech images are compressed with jpeg)
- libpng (for PNG image compression and decompression. INRIA dataset uses PNG, and we extract images as PNG)


Compile
-------

We use Netbeans 8.2 to manage the project.

From the commandline a simple make would compile the project.

Usage
-----

Do a
./pedestriandetectionbenchmark --help to display all the current options

For instance:
./pedestriandetectionbenchmark --play-input-sequence --dataset USA  
Will play the original Caltech sequence (set01) 

./pedestriandetectionbenchmark --play-input-as-hog --dataset USA  --rotate-hog
Will display a visualization of the HOG transform on the original Caltech sequence (set01) 




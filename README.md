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
- libsvm (to work with Support Vector Machine models)

Compile
-------

We use Netbeans 8.2 to manage the project.

If Netbeans project is correctly working, you can also compile from the commandline a simple 
<pre>make</pre>

Otherwise you can also compile with
<pre>g++ -g -std=c++14 *.cpp -o pedestriandetectionbenchmark -ljpeg -lpng -lm -lX11 -lsvm </pre>

Usage
-----

Do a
<pre>./pedestriandetectionbenchmark --help </pre>
to display all the current options

For instance:

<pre>./pedestriandetectionbenchmark --play-input-sequence --dataset USA  </pre>
Will play the original Caltech sequence (set01) 

<pre>./pedestriandetectionbenchmark --play-input-as-hog --dataset USA  --rotate-hog</pre>
Will display a visualization of the HOG transform on the original Caltech sequence (set01) 


<pre>./pedestriandetectionbenchmark --max-fps  --resize-persons-as 96 160  --min-person-height 100 --start-in-frame 1 --rotate-hog --extract-hog-svm</pre>
Will generate a SVM file with the positive HOG features from annotated pedestrians higher than 100 pixels, and normalized to 96x100 pixels 
and negative HOG features from random rectangular images from the source images.

<pre>./pedestriandetectionbenchmark --train-svm-from-files</pre>
Will create the SVM model from SVM files

<pre>./pedestriandetectionbenchmark --predict --resize-persons-as 96 160</pre>
Will use the SVM model to run the predictor while detecting pedestrians



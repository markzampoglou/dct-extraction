# dct-extraction
This is a combination of C and Java code aimed to extract the DCT coefficients from a JPEG-compressed file. It began with the realization that the only open-source way to get the compression data directly from a JPEG file is the Independent JPEG Group's (http://www.ijg.org/files/) C library (libjpeg).

The project consists of two elements:

1) A C library (libExportDCT), that accesses libjpeg and takes the data of interest in a serialized (and very simplistic) form, that can then be read by Java using JNA. It requires that libjpeg is already available in the system. I'd suggest version 8d, not 9.

2) A Java project (javaDCTExtraction) that accesses libExportDCT and transforms the output to a java int[][] array. JNA is required, included via Maven.

There are many limitations to this version of the code, most importantly that currently only the Y channel coefficients are extracted. I do not currently intend to improve the code, though it should be fairly straightforward. I might change my mind in the future and turn it into a proper library.

In trying to understand how libjpeg stores data internally and how to get it, Phil Sallee's MATLAB JPEG Toolbox (and especially the jpeg_read.m function) was a huge help. Keep it in mind if you intend to improve the extraction framework.

This code has been tested in Ubuntu 14.04, using libjpeg 8. It has been known to work in Windows 7 x64 too, though there you may have to compile the JPEG library into a dll yourself. See the README for some help.

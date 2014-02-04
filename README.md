OpenCVBlobsLib is a library written in C++ on the base of cvblobslib. It allows for labelling, filling, filtering, gathering information when dealing with "zones" with homogeneous features in an image. It uses OpenCV and PThread in order to boost the performance. The used algorithm is very efficient with big images and/or many blobs and can become even faster exploiting the multi-core architecture of modern CPUs.

A list of its features:
* Binary image 8-connected component labelling/blob extraction.
* Blob filtering (based on size or other user-defined features).
* Blob properties computation, e.g.:
* Mean and standard deviation of the pixel values in the covered region.
* Area and perimeter.
* Bounding box.
* Containing ellipse.
* Moments computation.
* Color-fill of the blob region.

OpenCVBlobsLib added Features:
* Multi core support for the extraction stage.
* OpenCV 2.0 compliant interface.
* Blob joining, allowing for distinct regions to be grouped as one.
* Generic bug fixing.

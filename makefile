#
# Makefile for blobs library for OpenCV Version 4 and its examples
# made by: Kumar Vishal

CFLAGS= -fpermissive `pkg-config --cflags opencv` -I. 
LDFLAGS= `pkg-config --libs opencv` -L. -lblob 
CXX=g++

CPPFILES= \
	BlobContour.cpp\
	blob.cpp\
	BlobOperators.cpp\
	BlobResult.cpp\
	ComponentLabeling.cpp

.SUFFIXES: .cpp.o
.cpp.o:	; echo 'Compiling $*.cpp' ; $(CXX) $(CFLAGS) -c $*.cpp

.SILENT:

libblob.a: $(CPPFILES:.cpp=.o)
	ar ru libopencvblobslib.a $(CPPFILES:.cpp=.o) 2> /dev/null
	ranlib libopencvblobslib.a
#
#	@echo Build tests, examples and tools...
#	$(CXX) -g blobdemo.cpp $(LDFLAGS) $(CFLAGS) -o blobdemo
#	$(CXX) -g blobdemo2.cpp $(LDFLAGS) $(CFLAGS) -o blobdemo2
#
#	@echo Copy include files...
#
	@echo Cleaning objects...
	rm -f $(CPPFILES:.cpp=.o)

all: clean

clean:
	@echo Cleaning...
	rm -f $(CFILES:.cpp=.o)
	rm -f libopencvblobslib.a
install:
	@echo Installing...
	sudo cp libopencvblobslib.a /usr/local/lib
	sudo mkdir /usr/local/include/opencvblobslib
	sudo cp *.h /usr/local/include/opencvblobslib
uninstall:
	@echo Uninstalling...
	sudo rm /usr/local/lib/libopencvblobslib.a
	sudo rm -rf /usr/local/include/opencvblobslib


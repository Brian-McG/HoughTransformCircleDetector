platform=$(shell uname -o)

all:
	qmake -o Makefile.run HoughTransformCircleDetector.pro
	make -f Makefile.run
	rm Makefile.run*
ifeq ($(shell uname -o),Cygwin)
	release/HoughTransformCircleDetector
else
	./HoughTransformCircleDetector	
endif
	
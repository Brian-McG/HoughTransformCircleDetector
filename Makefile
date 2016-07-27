EXEC_ARGS = gui

all:
	echo $(EXEC_ARGS)
	qmake -o Makefile.run HoughTransformCircleDetector.pro
	make -f Makefile.run
	rm Makefile.run*
ifeq ($(shell uname -o),Cygwin)
	release/HoughTransformCircleDetector
else
	./HoughTransformCircleDetector	
endif
	
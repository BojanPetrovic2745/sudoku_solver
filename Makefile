CFLAGS = -std=c++11 -Wall -ggdb
LIBS  = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml
LIBS += -lopencv_videoio -lopencv_features2d -lopencv_calib3d
LIBS += -lopencv_imgcodecs -lopencv_dnn

INCS = -I/usr/local/include/opencv4

OBJ = main.o Functions.o Recognizer.o
OBJ_BUILD = build/main.o build/Functions.o build/Recognizer.o

sudoku_ar_solver: $(OBJ)
	g++ -o $@ $(OBJ_BUILD) $(LIBS) $(CFLAGS)

%.o: src/%.cpp
	g++ $(INCS) -c -o build/$@ $< $(CFLAGS)

clean:
	rm -f build/*
	rm -f sudoku_ar_solver

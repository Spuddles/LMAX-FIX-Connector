INC_DIRS = -I ../quickfix/include
QFLIB = /usr/local/lib/libquickfix.so
CC_ARGS = -g -std=c++0x -DDEBUG $(INC_DIRS)
SOURCES=FIXAdapter.cpp CSGuardian.cpp FIXOrder.cpp FIXOrderCollection.cpp
SOURCES+=TimeObject.cpp Platform.cpp CurrencyCollection.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.h)
EXE=app

all: $(SOURCES) $(HEADERS) $(EXE)

release: CC_ARGS = -O3 -std=c++0x $(INC_DIRS)
release: $(SOURCES) $(HEADERS) $(EXE)

$(EXE): $(OBJECTS) FIXConnector.cpp
	g++ $(CC_ARGS) $(LIB_DIRS) $(LIBRARIES) $(OBJECTS) $(QFLIB) FIXConnector.cpp -o $(EXE)

.cpp.o: 
	g++ -c $(CC_ARGS) $< -o $@

clean:
	@rm -f *.o
	@rm -f *~
	@rm -f $(EXE)


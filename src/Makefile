
miniSQL : miniSQL.o API.o BufferManager.o CatalogManager.o \
	RecordManager.o Interpreter.o IndexManager.o condition.o
	g++ -std=c++11 -o minSQL miniSQL.o API.o BufferManager.o CatalogManager.o \
	RecordManager.o Interpreter.o IndexManager.o condition.o

miniSQL.o : miniSQL.cpp Interpreter.h
	g++ -std=c++11 -c miniSQL.cpp
API.o : API.cpp API.h DataStructure.h RecordManager.h CatalogManager.h IndexManager.h
	g++ -std=c++11 -c API.cpp
BufferManager.o : BufferManager.cpp DataStructure.h
	g++ -std=c++11 -c BufferManager.cpp
CatalogManager.o : CatalogManager.cpp Attribute.h BufferManager.h IndexInfo.h
	g++ -std=c++11 -c CatalogManager.cpp
RecordManager.o : RecordManager.cpp condition.h Attribute.h BufferManager.h
	g++ -std=c++11 -c RecordManager.cpp
Interpreter.o : Interpreter.cpp API.h DataStructure.h CatalogManager.h condition.h
	g++ -std=c++11 -c Interpreter.cpp
IndexManager.o : IndexManager.cpp DataStructure.h
	g++ -std=c++11 -c IndexManager.cpp
condition.o : condition.cpp condition.h
	g++ -std=c++11 -c condition.cpp

.PHONY: clean
clean :
	rm -f minSQL miniSQL.o API.o BufferManager.o CatalogManager.o \
	RecordManager.o Interpreter.o IndexManager.o condition.o

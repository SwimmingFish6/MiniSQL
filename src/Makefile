
miniSQL : miniSQL.o API.o BufferManager.o CatalogManager.o \
	RecordManager.o Interpreter.o IndexManager.o condition.o
	gcc -o minSQL miniSQL.o API.o BufferManager.o CatalogManager.o\
	RecordManager.o Interpreter.o IndexManager.o condition.o

miniSQL.o : miniSQL.cpp Interpreter.h
	gcc -c miniSQL.cpp
API.o : API.cpp API.h DataStructure.h RecordManager.h CatalogManager.h IndexManager.h
	gcc -c API.cpp
BufferManager.o : BufferManager.cpp DataStructure.h
	gcc -c BufferManager.cpp
CatalogManager.o : CatalogManager.cpp Attribute.h BufferManager.h Indexinfo.h
	gcc -c CatalogManager.cpp
RecordManager.o : RecordManager.cpp condition.h Attribute.h BufferManager.h
	gcc -c insert.cpp
Interpreter.o : Interpreter.cpp API.h DataStructure.h CatalogManager.h condition.h
	gcc -c search.cpp
IndexManager.o : IndexManager.cpp DataStructure.h
	gcc -c IndexManager.cpp
condition.o : condition.cpp condition.h
	gcc -c condition.cpp

.PHONY: clean
clean :
	rm -f minSQL miniSQL.o API.o BufferManager.o CatalogManager.o \
	RecordManager.o Interpreter.o IndexManager.o condition.o

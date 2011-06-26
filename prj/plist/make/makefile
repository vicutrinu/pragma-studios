CC=g++
CC_FLAGS=-G -I"../" -I"../.."
SRC=../
TINY_XML=../../../3rdParty/tinyxml/
3RD_PARTY_INCLUDE=-I "../../../3rdParty/"

a.out: tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o plist.o Array.o AnyData.o Dictionary.o main.o utils.o
	$(CC) tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o plist.o Array.o AnyData.o Dictionary.o main.o utils.o -G -o a.out

tinystr.o: $(TINY_XML)tinystr.cpp
	$(CC) -c $(CC_FLAGS) $(TINY_XML)/tinystr.cpp -o tinystr.o

tinyxml.o: $(TINY_XML)tinyxml.cpp
	$(CC) -c $(CC_FLAGS) $(TINY_XML)/tinyxml.cpp -o tinyxml.o

tinyxmlerror.o: $(TINY_XML)tinyxmlerror.cpp
	$(CC) -c $(CC_FLAGS) $(TINY_XML)/tinyxmlerror.cpp -o tinyxmlerror.o

tinyxmlparser.o: $(TINY_XML)tinyxmlparser.cpp
	$(CC) -c $(CC_FLAGS) $(TINY_XML)/tinyxmlparser.cpp -o tinyxmlparser.o

plist.o: $(SRC)plist.cpp
	$(CC) -c $(CC_FLAGS) $(3RD_PARTY_INCLUDE) $(SRC)plist.cpp -o plist.o

Array.o:	$(SRC)Array.cpp
		$(CC) -c $(CC_FLAGS) $(3RD_PARTY_INCLUDE) $(SRC)Array.cpp -o Array.o

AnyData.o:	$(SRC)AnyData.cpp
	$(CC) -c $(CC_FLAGS) $(SRC)AnyData.cpp -o AnyData.o

Dictionary.o:	$(SRC)Dictionary.cpp
	$(CC) -c $(CC_FLAGS) $(3RD_PARTY_INCLUDE) $(SRC)Dictionary.cpp -o Dictionary.o

main.o:	$(SRC)main.cpp
	$(CC) -c $(CC_FLAGS) $(SRC)main.cpp -o main.o

utils.o:	$(SRC)utils.cpp
	$(CC) -c $(CC_FLAGS) $(SRC)utils.cpp -o utils.o

clean:
	rm plist.o
	rm Dictionary.o
	rm Array.o
	rm AnyData.o
	rm tinyxml.o
	rm tinystr.o
	rm tinyxmlerror.o
	rm tinyxmlparser.o
	rm utils.o
	rm main.o
	rm a.out
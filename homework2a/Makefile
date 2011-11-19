GCJ = gcj
COMPOPTS = -O3

SOURCES= PQSort/SwappableList.java\
	PQSort/TPSortThread.java\
	PQSort/TruongPQSort.java

OBJFILES = $(SOURCES:%.java=%.o)

OUTFILE = TruongPQSort

all: $(OUTFILE)

TruongPQSort: $(OBJFILES)
	$(GCJ) --main=PQSort.TruongPQSort $(OBJFILES) -o $@

%.o:%.java
	$(GCJ) $(COMPOPTS) -c -o $@ $<
	objcopy -L '_ZGr8_$$_dummy' $@    

clean:
	rm -f $(OBJFILES) $(OUTFILE)

unit_test:
	./unit_test.sh ./$(OUTFILE) unit_test.txt
	./unit_test.sh ./$(OUTFILE) unit_test2.txt
	./unit_test.sh ./$(OUTFILE) unit_test3.txt


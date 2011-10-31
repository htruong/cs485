all: truongpspell
truongpspell: truongpspell.cc truongpspell.h
	clang++ -O0 -ggdb -Wall -lpthread -o truongpspell truongpspell.cc -DSUPRESS_CLOCK
clean:
	rm truongpspell

#unit_test:
#	./unit_test.sh


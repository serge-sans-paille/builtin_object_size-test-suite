CFLAGS:=-O2
CXXFLAGS:=$(CFLAGS)

check: check-static check-staticxx
	./check-static ; static=$$? ; ./check-staticxx ; staticxx=$$?; exit `echo $$static + $$staticxx | bc`

clean:
	$(RM) check-static check-staticxx

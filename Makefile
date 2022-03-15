CFLAGS:=-O2

check: check-static
	./check-static

clean:
	$(RM) check-static

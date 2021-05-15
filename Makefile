default:
	gcc -D_XOPEN_SOURCE=700 -std=c99 -lX11 -O2 -pipe -march=native -mtune=native -o abstouch main.c

install: default
	cp abstouch /usr/local/bin/abstouch

uninstall:
	rm /usr/local/bin/abstouch

clean:
	rm -f abstouch

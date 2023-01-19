scroll_ifilter.so: scroll_ifilter.cpp
	gcc -g -shared -o scroll_ifilter.so scroll_ifilter.cpp

install: scroll_ifilter.so
	cp scroll_ifilter.so /boot/home/config/non-packaged/add-ons/input_server/filters
	/system/servers/input_server -q

all: scroll_ifilter.so

FLAGS:=-Os -g -Wall -DDATE="\"`date +'%Y'`\""
SRCS=weather.c output.o file.o tcp.o string.o
TARGET=weather-cli

all: $(TARGET)

weather-cli: $(SRCS)
	gcc $(FLAGS)  -o $(TARGET) $(SRCS)

clean:
	rm -f *.d *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin/$(TARGET)
	cp pics /usr/share/weather-cli -R
	chmod go+rx /usr/bin/$(TARGET)
	chmod go+rX /usr/share/weather-cli -R



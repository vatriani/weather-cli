FLAGS:=-Os -g -Wall -DDATE="\"`date +'%Y'`\""
SRCS=weather.c output.c file.c tcp.c string.c
TARGET=weather-cli

all: $(TARGET)

weather-cli: $(SRCS)
	gcc $(FLAGS)  -o $(TARGET) weather.c

clean:
	rm -f *.d *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin/$(TARGET)
	cp pics /usr/share/weather-cli -R
	chmod go+rx /usr/bin/$(TARGET)
	chmod go+rX /usr/share/weather-cli -R



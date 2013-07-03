FLAGS:=-Os -march=native -mtune=generic -Wall -DDATE="\"`date +'%Y'`\""
SRCS=weather.c output.c config.c file.c tcp.c
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



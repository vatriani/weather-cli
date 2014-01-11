weather-cli
===========

cli programm for linux, it's speak to weather.tuxnet24.de to grab
actual weather informations


for database enable in weather-cli.c uncomment #define DBASE

Compile:
make

Install (to use symlinking actual weather icon):
make install

Usage:
weather-cli  for Linux, BSD and Windows
use: weather-cli gmxxXXXX [OPTION]

  -s		short output
  -f [FORMAT]	 string
  -l		linking image

  -v		version
  -h		help

FORMAT: example
"%lc\n%cm, %ctC°\nWind: %cws, %cwd°\n"

%lo	longitude
%la	latitude
%lc	city
%lr	region
%lR	country
%hu	humidity
%vi	visibility
%pr	pressure
%ri	rising
%sr	sunrise
%ss	sunset
%cc	current code
%cd	current date
%ct	current temp
%cm	current text
%ci	current image
%cws	speed
%cwd	direction
%cwc	chill
%td	tomorrow day
%tD	tomorrow date
%ttl	tomorrow temp low
%tth	tomorrow temp high
%tm	tomorrow text
%tc	tomorrow code
%ti	tomorrow image

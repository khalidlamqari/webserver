#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

WEATHER=$(curl -s "https://wttr.in/Khouribga?format=%C+%t+%h")

CONDITION=$(echo "$WEATHER" | cut -d ' ' -f1)
TEMP=$(echo "$WEATHER" | cut -d ' ' -f2)
HUM=$(echo "$WEATHER" | cut -d ' ' -f3)

echo "<html>"
echo "<head><title>Weather in Khouribga</title></head>"
echo "<body>"
echo "<h1>Current Weather in Khouribga, Morocco</h1>"
echo "<p>Condition: $CONDITION</p>"
echo "<p>Temperature: $TEMP</p>"
echo "<p>Humidity: $HUM</p>"
echo "</body>"
echo "</html>"

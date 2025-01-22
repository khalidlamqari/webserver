#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

TZ_MOROCCO=$(TZ="Africa/Casablanca" date)
TZ_UTC=$(date -u)
TZ_PARIS=$(TZ="Europe/Paris" date)
TZ_TOK=$(TZ="Asia/Tokyo" date)

echo "<html>"
echo "<head><title>Time Zones</title></head>"
echo "<body>"
echo "<h1>Current Time in Khouribga and Other Cities</h1>"
echo "<ul>"
echo "<li><strong>Khouribga, Morocco:</strong> $TZ_MOROCCO</li>"
echo "<li><strong>UTC:</strong> $TZ_UTC</li>"
echo "<li><strong>Paris, France:</strong> $TZ_PARIS</li>"
echo "<li><strong>Tokyo, Japan:</strong> $TZ_TOK</li>"
echo "</ul>"
echo "</body>"
echo "</html>"

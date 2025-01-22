#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

DAY=$(date +%d)
MOON_PHASE=$(( (DAY + 1) % 30 ))

if [ "$MOON_PHASE" -le 1 ] || [ "$MOON_PHASE" -ge 29 ]; then
  PHASE="New Moon"
elif [ "$MOON_PHASE" -le 7 ]; then
  PHASE="First Quarter"
elif [ "$MOON_PHASE" -le 15 ]; then
  PHASE="Full Moon"
else
  PHASE="Last Quarter"
fi

echo "<html>"
echo "<head><title>Moon Phase</title></head>"
echo "<body>"
echo "<h1>Moon Phase</h1>"
echo "<p>Today's moon phase: <strong>$PHASE</strong></p>"
echo "</body>"
echo "</html>"

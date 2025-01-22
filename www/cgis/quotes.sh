#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

QUOTES_URL="https://gist.githubusercontent.com/robatron/a66acc0eed3835119817/raw/77493d3ddf69fbd9d69997e22e1a7c6c70c8bdf2/quotes.txt"
QUOTES_FILE="/tmp/quotes.txt"

curl -s "$QUOTES_URL" > "$QUOTES_FILE"

len=$(wc -l < "$QUOTES_FILE")

ran=$((RANDOM % len + 1))

RANDOM_QUOTE=$(sed -n "${ran}p" "$QUOTES_FILE")

QUOTE_TEXT=$(echo "$RANDOM_QUOTE" | awk -F'~' '{print $1}')
QUOTE_AUTHOR=$(echo "$RANDOM_QUOTE" | awk -F'~' '{print $2}')

echo "<html>"
echo "<head><title>Random Quote</title></head>"
echo "<body>"
echo "<h1>Inspirational Quote</h1>"
echo "<blockquote><em>$QUOTE_TEXT</em></blockquote>"
echo "<p><strong>- $QUOTE_AUTHOR</strong></p>"
echo "</body>"
echo "</html>"
rm -rf $QUOTES_FILE
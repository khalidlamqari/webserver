#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

WEBSITE="google.com" 

if ! nslookup "$WEBSITE" &> /dev/null; then
  STATUS="The website <strong>$WEBSITE</strong> does not exist or cannot be resolved."
else
  if ping -c 1 -W 2 "$WEBSITE" &> /dev/null; then
    STATUS="The website <strong>$WEBSITE</strong> is currently: <strong>Online</strong>."
  else
    STATUS="The website <strong>$WEBSITE</strong> is currently: <strong>Offline</strong>."
  fi
fi

echo "<html>"
echo "<head>"
echo "<style>"
echo "body {"
echo "    font-family: Arial, sans-serif;"
echo "    background-color: #f4f4f9;"
echo "    color: #333;"
echo "    text-align: center;"
echo "    padding: 20px;"
echo "}"
echo "h1 {"
echo "    color: #444;"
echo "    font-size: 2em;"
echo "    margin-bottom: 20px;"
echo "}"
echo "p {"
echo "    font-size: 1.2em;"
echo "    padding: 10px;"
echo "    background: #e0f7fa;"
echo "    border: 1px solid #b2ebf2;"
echo "    display: inline-block;"
echo "    border-radius: 5px;"
echo "}"
echo "</style>"
echo "<title>Website Status</title></head>"
echo "<body>"
echo "<h1>Website Status Checker</h1>"
echo "<p>$STATUS</p>"
echo "</body>"
echo "</html>"

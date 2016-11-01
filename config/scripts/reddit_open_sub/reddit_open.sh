#!/bin/bash
echo [$@]
echo "command=xdg-open \"https://reddit.com/r/$@\""
echo "icon=reddit.png"
echo "subtext=Open sub-reddit $@"

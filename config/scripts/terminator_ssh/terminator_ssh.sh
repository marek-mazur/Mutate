#!/bin/bash
echo [$@]
echo "command=terminator -x "ssh $@""
echo "icon=terminator.png"
echo "subtext=SSH to $@"

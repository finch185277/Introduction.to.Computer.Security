#!/bin/sh

rm -rf /home/victim/.etc
rm -rf /home/victim/.Launch_Attack
sed -i '/Launching_Attack/d' /etc/crontab
pkill -f /home/victim/.Launch_Attack/Launching_Attack

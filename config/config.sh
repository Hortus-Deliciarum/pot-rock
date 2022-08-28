#!/usr/bin/env bash

# COLORS

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
GREY='\033[1;30m'
NC='\033[0m'

DIR_SYSTEMD="/etc/systemd/system"
DIR_HOMEROCK="/home/rock"
DIR_PATCH="$DIR_HOMEROCK/Documents/Hortus-Deliciarum/pot-rock/patches"
PUREDATA=".puredata"
COPY_SERVICES=( "potrock_serial_dispatcher.service" )
COPY_FILES=( ".jackdrc" ".puredata" )
SERVICES=( "potrock_serial_dispatcher" "rock_jackd" "rock_puredata")
FILES=(`ls -C $DIR_PATCH/*.pd`)

length=${#FILES[@]}
counter=1

echo_error() {
    local ERROR
    ERROR="${NC}[${RED} ERROR ${NC}]"
    echo -e "${ERROR}${GREY}\t$1${NC}"
}

echo_ok() {
    local OK
    OK="${NC}[${GREEN}  OK   ${NC}]"
    echo -e "${OK}${GREY}\t$1${NC}"
}

echo_debug() {
    local DEBUG 
    DEBUG="${NC}[${YELLOW} DEBUG ${NC}]"
    echo -e "${DEBUG}${GREY}\t$1${NC}"
}

set_patch() {
    echo
    echo "=== SETTING PATCH ==="
    echo_ok "set following patch: $1"    
    echo "#!/usr/bin/env bash" > $PUREDATA
    echo "puredata -nogui -jack $1" >> $PUREDATA
}

# BANNER

clear
echo
figlet -ck -f pagga "POTROCK"
figlet -ck -f future configuration
sleep 2

# CHOOSE PATCH

echo
echo "=== SET PATCH TO PLAY ==="
echo 

for i in ${FILES[@]}; do 
    echo -e "${CYAN}\t($counter) $(basename $i)"
    counter=$(( $counter + 1))
done 

echo -e "${NC}"

control=1
echo 

while [[ control -eq 1 ]]; do 
    printf "Choose a patch by index: "
    read n
    index=$(( $n - 1 ))
    
    if [[ $index -gt $(( $length - 1 )) || $index -lt 0 ]]
        then 
            echo_error "Index out of range, please retry..."
        else
            set_patch ${FILES[$index]}
	    control=0
    fi 
done

# OPERATIONS

echo
echo "=== COPYING SERVICES AND FILES... ==="
sleep 1

for service in ${COPY_SERVICES[@]}; do
    cp $service $DIR_SYSTEMD
    echo_ok "copied $service in $DIR_SYSTEMD"
done

for file in ${COPY_FILES[@]}; do
    cp $file $DIR_HOMEROCK
    echo_ok "copied $file in $DIR_HOMEROCK"
done

echo -e "${NC}"

echo
echo "=== STARTING & ENABLING SERVICES ==="
sleep 1

for service in ${SERVICES[@]}; do
    systemctl restart $service > /dev/null 2>&1
    systemctl enable $service > /dev/null 2>&1
    echo_ok "starting/enabling ${service}"
done

echo -e "${NC}"
echo
echo "=== RUNNING daemon-reload... ==="
sleep 1

systemctl daemon-reload
echo_ok "Done"
echo

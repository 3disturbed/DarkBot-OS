#!/bin/bash

# Function to update the password
update_password() {
    echo "Please enter the new password for the Raspberry Pi:"
    read -s new_password
    echo "Please confirm the new password:"
    read -s confirm_password

    if [ "$new_password" == "$confirm_password" ]; then
        echo "pi:$new_password" | sudo chpasswd
        echo "Password successfully updated."
    else
        echo "Passwords do not match. Please run the script again."
        exit 1
    fi
}

# Function to list WiFi networks and update credentials
update_wifi() {
    echo "Scanning for available WiFi networks..."
    networks=$(nmcli device wifi list | awk 'NR>1 {print NR-1, $2}')

    if [ -z "$networks" ]; then
        echo "No WiFi networks found. Please check your WiFi adapter."
        exit 1
    fi

    echo "Available WiFi networks:"
    echo "$networks"

    echo "Please select the network index:"
    read index

    ssid=$(echo "$networks" | awk -v idx="$index" '$1 == idx {print $2}')

    if [ -z "$ssid" ]; then
        echo "Invalid selection. Please run the script again."
        exit 1
    fi

    echo "You have selected SSID: $ssid"
    echo "Please enter the WiFi password for $ssid:"
    read -s wifi_password

    wpa_supplicant_conf="/etc/wpa_supplicant/wpa_supplicant.conf"
    sudo bash -c "cat > $wpa_supplicant_conf" <<EOL
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=US

network={
    ssid="$ssid"
    psk="$wifi_password"
}
EOL

    echo "WiFi credentials updated. Rebooting to apply changes..."
    sudo reboot
}

# Main script execution
echo "Welcome! This script will update your Raspberry Pi's password and WiFi credentials."

update_password
update_wifi

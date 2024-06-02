#!/bin/bash

echo "Welcome to DarkBot-OS setup!"
echo "Please choose the environment you want to set up:"
echo "1. C++"
echo "2. Python"
echo "3. Node.js"

read -p "Enter your choice (1/2/3): " choice

case $choice in
    1)
        echo "Setting up C++ environment..."

        # Install required packages
        sudo apt-get update
        sudo apt-get install -y g++ git cmake libboost-all-dev libwebsocketpp-dev

        # Clone the repository
        git clone https://github.com/3disturbed/DarkBot-OS.git
        cd DarkBot-OS

        # Compile server.cpp
        g++ -std=c++11 -o server server.cpp -lwebsocketpp -lboost_system -lpthread

        # Compile updater.cpp
        g++ -std=c++11 -o updater updater.cpp -lpthread

        # Set up rc.local for auto-start
        sudo bash -c 'cat <<EOT >> /etc/rc.local
/home/pi/DarkBot-OS/updater &
/home/pi/DarkBot-OS/server &
EOT'
        echo "C++ environment setup complete!"
        ;;
    2)
        echo "Setting up Python environment..."

        # Install required packages
        sudo apt-get update
        sudo apt-get install -y python3-pip git
        pip3 install requests

        # Clone the repository
        git clone https://github.com/3disturbed/DarkBot-OS.git
        cd DarkBot-OS

        # Set up rc.local for auto-start
        sudo bash -c 'cat <<EOT >> /etc/rc.local
python3 /home/pi/DarkBot-OS/updater.py &
/home/pi/DarkBot-OS/server &
EOT'
        echo "Python environment setup complete!"
        ;;
    3)
        echo "Setting up Node.js environment..."

        # Install required packages
        sudo apt-get update
        sudo apt-get install -y nodejs npm git

        # Clone the repository
        git clone https://github.com/3disturbed/DarkBot-OS.git
        cd DarkBot-OS

        # Install Node.js dependencies
        npm install express http ws serialport node-fetch tar fs-extra pm2 -g

        # Start applications using PM2
        pm2 start updater.js --name darkbot-updater
        pm2 start server.js --name darkbot-server
        pm2 save
        pm2 startup

        echo "Node.js environment setup complete!"
        ;;
    *)
        echo "Invalid choice. Please run the script again and choose a valid option."
        ;;
esac

echo "Setup complete! Rebooting..."
sudo reboot

import os
import time
import requests
import subprocess
import tarfile

# GitHub repository details
owner = '3disturbed'
repo = 'DarkBot-OS'
branch = 'main' # or the branch you want to track
local_dir = '/home/pi/DarkBot-OS' # Change this to your local directory
current_commit_file = os.path.join(local_dir, '.current_commit')
update_check_interval = 3600 # Check for updates every hour

def get_latest_commit_hash():
    url = f'https://api.github.com/repos/{owner}/{repo}/commits/{branch}'
    response = requests.get(url)
    response.raise_for_status()
    data = response.json()
    return data['sha']

def initialize_current_commit_hash():
    latest_commit_hash = get_latest_commit_hash()
    with open(current_commit_file, 'w') as commit_file:
        commit_file.write(latest_commit_hash)
    print(f'Initialized current commit hash: {latest_commit_hash}')

def read_current_commit_hash():
    if os.path.exists(current_commit_file):
        with open(current_commit_file, 'r') as commit_file:
            return commit_file.read().strip()
    return None

def update_server(latest_commit_hash):
    tarball_url = f'https://github.com/{owner}/{repo}/archive/refs/heads/{branch}.tar.gz'
    response = requests.get(tarball_url, stream=True)
    response.raise_for_status()
    
    tarball_path = os.path.join(local_dir, 'update.tar.gz')
    with open(tarball_path, 'wb') as tarball_file:
        for chunk in response.iter_content(chunk_size=8192):
            tarball_file.write(chunk)

    with tarfile.open(tarball_path, 'r:gz') as tar:
        tar.extractall(path=local_dir)

    os.remove(tarball_path)

    with open(current_commit_file, 'w') as commit_file:
        commit_file.write(latest_commit_hash)
    
    print('Restarting server...')
    subprocess.run(['sudo', 'reboot'])

def check_for_updates():
    latest_commit_hash = get_latest_commit_hash()
    current_commit_hash = read_current_commit_hash()

    if current_commit_hash != latest_commit_hash:
        print('New update available. Downloading and installing...')
        update_server(latest_commit_hash)
    else:
        print('No updates available.')

if __name__ == '__main__':
    if not os.path.exists(current_commit_file):
        initialize_current_commit_hash()

    while True:
        check_for_updates()
        time.sleep(update_check_interval)

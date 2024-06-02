#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>

const std::string REPO_URL = "https://github.com/3disturbed/DarkBot-OS.git";
const std::string LOCAL_DIR = "/home/pi/DarkBot-OS"; // Change this to your local directory
const std::string CURRENT_COMMIT_FILE = LOCAL_DIR + "/.current_commit";
const int UPDATE_INTERVAL = 3600; // Check for updates every hour

std::string getLatestCommitHash() {
    std::string command = "git ls-remote " + REPO_URL + " refs/heads/main";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    return result.substr(0, 40); // Get the first 40 characters (commit hash)
}

void initializeCurrentCommitHash() {
    std::string latestCommitHash = getLatestCommitHash();
    std::ofstream commitFile(CURRENT_COMMIT_FILE);
    if (commitFile.is_open()) {
        commitFile << latestCommitHash;
        commitFile.close();
        std::cout << "Initialized current commit hash: " << latestCommitHash << std::endl;
    } else {
        std::cerr << "Unable to open file to write current commit hash." << std::endl;
    }
}

std::string readCurrentCommitHash() {
    std::ifstream commitFile(CURRENT_COMMIT_FILE);
    std::string currentCommitHash = "";
    if (commitFile.is_open()) {
        getline(commitFile, currentCommitHash);
        commitFile.close();
    }
    return currentCommitHash;
}

void updateServer(const std::string& latestCommitHash) {
    std::string command = "cd " + LOCAL_DIR + " && git pull origin main && make";
    system(command.c_str());

    std::ofstream commitFile(CURRENT_COMMIT_FILE);
    if (commitFile.is_open()) {
        commitFile << latestCommitHash;
        commitFile.close();
    } else {
        std::cerr << "Unable to open file to write current commit hash." << std::endl;
    }

    std::cout << "Restarting server..." << std::endl;
    system("sudo reboot");
}

void checkForUpdates() {
    std::string latestCommitHash = getLatestCommitHash();
    std::string currentCommitHash = readCurrentCommitHash();

    if (currentCommitHash != latestCommitHash) {
        std::cout << "New update available. Downloading and installing..." << std::endl;
        updateServer(latestCommitHash);
    } else {
        std::cout << "No updates available." << std::endl;
    }
}

int main() {
    struct stat buffer;
    if (stat(CURRENT_COMMIT_FILE.c_str(), &buffer) != 0) {
        initializeCurrentCommitHash();
    }

    while (true) {
        checkForUpdates();
        sleep(UPDATE_INTERVAL);
    }

    return 0;
}

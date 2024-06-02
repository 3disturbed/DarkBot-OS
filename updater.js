const fetch = require('node-fetch');
const tar = require('tar');
const fs = require('fs-extra');
const { exec } = require('child_process');

// GitHub repository details
const owner = '3disturbed';
const repo = 'DarkBot-OS';
const branch = 'main'; // or the branch you want to track

// Path to the directory where the server is running
const serverPath = __dirname;
const updateCheckInterval = 3600000; // Check for updates every hour

async function checkForUpdates() {
  try {
    // Fetch the latest commit hash from GitHub
    const res = await fetch(`https://api.github.com/repos/${owner}/${repo}/commits/${branch}`);
    const data = await res.json();
    const latestCommitHash = data.sha;

    // Compare with the stored latest commit hash
    const currentCommitHash = fs.readFileSync(`${serverPath}/.current_commit`, 'utf-8');
    if (currentCommitHash !== latestCommitHash) {
      console.log('New update available. Downloading and installing...');
      await updateServer(latestCommitHash);
    } else {
      console.log('No updates available.');
    }
  } catch (error) {
    console.error('Error checking for updates:', error);
  }
}

async function updateServer(latestCommitHash) {
  try {
    // Download the latest release tarball from GitHub
    const tarballUrl = `https://github.com/${owner}/${repo}/tarball/${branch}`;
    const res = await fetch(tarballUrl);
    const tarballStream = fs.createWriteStream(`${serverPath}/update.tar.gz`);
    res.body.pipe(tarballStream);

    tarballStream.on('finish', async () => {
      // Extract the tarball
      await tar.x({
        file: `${serverPath}/update.tar.gz`,
        C: serverPath,
        strip: 1
      });

      // Clean up
      fs.removeSync(`${serverPath}/update.tar.gz`);

      // Save the latest commit hash
      fs.writeFileSync(`${serverPath}/.current_commit`, latestCommitHash);

      // Restart the server
      restartServer();
    });
  } catch (error) {
    console.error('Error updating server:', error);
  }
}

function restartServer() {
  console.log('Restarting server...');
  exec('npm restart', (error, stdout, stderr) => {
    if (error) {
      console.error('Error restarting server:', error);
    } else {
      console.log('Server restarted successfully:', stdout);
    }
  });
}

// Initial update check
checkForUpdates();

// Schedule periodic update checks
setInterval(checkForUpdates, updateCheckInterval);

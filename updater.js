const fetch = require('node-fetch');
const tar = require('tar');
const fs = require('fs-extra');
const { exec } = require('child_process');

// GitHub repository details
const owner = '3disturbed';
const repo = 'DarkBot-OS';

// Path to the directory where the server is running
const serverPath = __dirname;
const updateCheckInterval = 3600000; // Check for updates every hour

async function getLatestTag() {
  try {
    const res = await fetch(`https://api.github.com/repos/${owner}/${repo}/tags`);
    const data = await res.json();
    return data.length > 0 ? data[0].name : null;
  } catch (error) {
    console.error('Error fetching the latest tag:', error);
    return null;
  }
}

async function initializeCurrentTag() {
  const latestTag = await getLatestTag();
  if (latestTag && !fs.existsSync(`${serverPath}/.current_tag`)) {
    fs.writeFileSync(`${serverPath}/.current_tag`, latestTag);
    console.log('Initialized current tag:', latestTag);
  }
}

async function checkForUpdates() {
  try {
    const latestTag = await getLatestTag();
    if (!latestTag) return;

    const currentTag = fs.readFileSync(`${serverPath}/.current_tag`, 'utf-8');
    if (currentTag !== latestTag) {
      console.log('New update available. Downloading and installing...');
      await updateServer(latestTag);
    } else {
      console.log('No updates available.');
    }
  } catch (error) {
    console.error('Error checking for updates:', error);
  }
}

async function updateServer(latestTag) {
  try {
    const tarballUrl = `https://github.com/${owner}/${repo}/archive/refs/tags/${latestTag}.tar.gz`;
    const res = await fetch(tarballUrl);
    const tarballStream = fs.createWriteStream(`${serverPath}/update.tar.gz`);
    res.body.pipe(tarballStream);

    tarballStream.on('finish', async () => {
      await tar.x({
        file: `${serverPath}/update.tar.gz`,
        C: serverPath,
        strip: 1
      });

      fs.removeSync(`${serverPath}/update.tar.gz`);
      fs.writeFileSync(`${serverPath}/.current_tag`, latestTag);
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

initializeCurrentTag().then(() => {
  checkForUpdates();
  setInterval(checkForUpdates, updateCheckInterval);
});

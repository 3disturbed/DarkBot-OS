const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
const express = require('express');
const http = require('http');
const WebSocket = require('ws');

// Create an Express application
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Replace with your serial port name
const port = new SerialPort('/dev/ttyS1', {
  baudRate: 115200
});

const parser = port.pipe(new Readline({ delimiter: '\n' }));

// WebSocket connection event
wss.on('connection', (ws) => {
  console.log('Client connected');

  ws.on('message', (message) => {
    console.log(`Received: ${message}`);
    port.write(message + '\n'); // Send the message to the Teensy
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
});

// Handle incoming data from the Teensy
parser.on('data', (data) => {
  console.log(`Teensy: ${data}`);
  // Broadcast to all connected WebSocket clients
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
});

// Serve the frontend (if any)
app.use(express.static('public'));

server.listen(3000, () => {
  console.log('Server is listening on port 3000');
});

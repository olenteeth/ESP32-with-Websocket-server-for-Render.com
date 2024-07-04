express = require('express');
const WebSocket = require('ws');

const app = express();
const server = require('http').Server(app);
const wss = new WebSocket.Server({ server:server });
var PORT = process.env.PORT || 3000;

app.use('/',express.static('public'));

wss.on('connection', function connection(message) {
  console.log("new conection");

  message.on('message', function incoming(test){
    console.log("message from client:" + test);
      wss.clients.forEach(function(client){
        client.send(String(test));
      });
    });
    });

  wss.on('close', function close() {
    console.log('Client disconnected');
  });

server.listen(PORT, () => console.log(`Listening on ${PORT}`));

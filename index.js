express = require('express');
const WebSocket = require('ws');

const app = express();
const server = require('http').Server(app);
const wss = new WebSocket.Server({ server:server });
var PORT = process.env.PORT || 80;

app.use('/',express.static('public'));

wss.on('connection', function connection(nhantin) {
  console.log("Ayo co nguoi moi vao");

  nhantin.on('message', function incoming(test){
    console.log("Nhận tin từ Client nào đó:" + test);
      wss.clients.forEach(function(client){
        client.send(String(test));
      });
    });
    });

  wss.on('close', function close() {
    console.log('Client disconnected');
  });

server.listen(PORT, () => console.log(PORT))
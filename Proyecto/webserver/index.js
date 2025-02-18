// Imports
const mqtt = require('mqtt');
const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');
var bodyParser = require('body-parser')

var jsonParser = bodyParser.json()

// Conection
const app = express();
// MQTT Port
const wss_vars = new WebSocket.Server({ port: 8080 });
// Broker IP
const client = mqtt.connect('mqtt://192.168.0.9');

app.use(express.static('public'));
app.use(cors()); 


// Publish parameters
app.post('/pid', jsonParser, (req, res) => {
  const { kp, ki, kd } = req.body;

  // Validate parametes
  if (typeof kp !== 'number' || typeof ki !== 'number' || typeof kd !== 'number') {
    return res.status(400).send({ error: 'Kp, Ki y Kd deben ser números.' });
  }

  // Pass values to hex
  const message = JSON.stringify({
    kp: float2hex(kp),
    ki: float2hex(ki),
    kd: float2hex(kd),
  });

  // Publish values
  client.publish('/seg/PID', message, (err) => {
    if (err) {
      console.error('Error al publicar los parámetros PID:', err);
      return res.status(500).send({ error: 'Error al enviar los parámetros PID.' });
    }
    console.log('Parámetros PID enviados:', message);
    res.status(200).send({ message: 'Parámetros PID enviados correctamente.' });
  });
});


// Subscription to topics (For receiving)
client.on('connect', () => {
  client.subscribe('/seg/vars');
  console.log('Conectado al broker MQTT');
});

// Callback if a message comes from a topic
client.on('message', (topic, message) => {
    if (topic === '/seg/vars') {
      // Convertir el mensaje en un objeto JSON
      let data;
      try {
          data = JSON.parse(message.toString());
      } catch (error) {
        console.log('Error al parsear el mensaje:', error);
        return;
      }
  
      // Convert hex to float
      let kp = hexToDecimal(data.kp);
      let ki = hexToDecimal(data.ki);
      let kd = hexToDecimal(data.kd);

      decimalKp = kp.map(filterSmallValues);
      decimalKi = ki.map(filterSmallValues);
      decimalKd = kd.map(filterSmallValues);

      decimalKp.shift();
      decimalKi.shift();
      decimalKd.shift();
  
      //Send data by websocket
      wss_vars.clients.forEach(ws => {
        ws.send(JSON.stringify({
          kp: decimalKp,
          ki: decimalKi,
          kd: decimalKd
        }));
      });
    }
  });




// Utils for encoding messages to hex
function hexToDecimal(hexString) {
    const hexArray = hexString.split(',');
    const floatArray = hexArray.map(hex => {
        const buffer = Buffer.from(hex, 'hex');
        return buffer.readFloatBE(0);
    });
    
    return floatArray;
}

function float2hex(value) {
  const buffer = Buffer.alloc(4);
  buffer.writeFloatBE(value);
  return buffer.toString('hex');
}

function filterSmallValues(value) {
    const threshold = 1e-3;
    return Math.abs(value) < threshold ? 0 : value;
}


// Start web server
app.listen(3000, () => {
  console.log('Servidor HTTP en http://localhost:3000');
});

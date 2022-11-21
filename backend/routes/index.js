var express = require('express');
const { Test } = require('../database');
var router = express.Router();
var db = require('../database');
const config = require('../hivetracker-config.json');
const mqtt = require('mqtt');

// MQTT
const options = {
  port: config.mqttConfig.mqttPort,
  clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
  username: config.mqttConfig.mqttUser,
  password: config.mqttConfig.mqttPassword,
  keepalive: 60,
  rejectUnauthorized: false
};

const client = mqtt.connect(config.mqttConfig.mqttHost, options);

client.on('connect', function () {
  client.subscribe('hivewatch/#', err => {
    if (err) {
      console.log(err)
    } else {
      client.publish('hivewatch/events', 'client connected');
    }
  })
})

client.on('message', function (topic, message) {
  switch (topic.toString()) {
    case 'hivewatch/data/temperature':
      if(message.toString() != 'NaN' && message.toString() != null) {
        db.Temperature.create({
          temperature: message.toString(),
          hiveId: 1
        });
      }
      break;

    case 'hivewatch/data/weight':
      if(!isNaN(message.toString()) && message.toString() != null) {
        db.Weight.create({
          weight: message.toString(),
          hiveId: 1
        });
      }
      break;

    case 'hivewatch/events':
      console.log(message.toString());
      break;

    default:
      console.log('Unknown topic: ' + topic.toString());
      break;
  }
});

client.on('error', function (err) {
  console.log(err);
  client.end();
});

router.get('/', async function (req, res, next) {
  let data = {
    temperature: await db.Temperature.findAll(),
    weight: await db.Weight.findAll()
  }
  res.json(data);
});

module.exports = router;

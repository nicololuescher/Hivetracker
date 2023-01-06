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
  client.subscribe('hivetracker/#', err => {
    if (err) {
      console.log(err)
    } else {
      client.publish('hivetracker/events', 'client connected');
    }
  })
})

client.on('message', function (topic, message) {
  switch (topic.toString()) {
    case 'hivetracker/data/temperature':
      if(message.toString() != 'NaN' && message.toString() != null) {
        db.Temperature.create({
          temperature: message.toString(),
          hiveId: 1
        });
      }
      break;

    case 'hivetracker/data/weight':
      if(!isNaN(message.toString()) && message.toString() != null) {
        db.Weight.create({
          weight: message.toString(),
          hiveId: 1
        });
      }
      break;

    case 'hivetracker/events':
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

router.get('/getData', async function (req, res, next) {
  let data = {
    temperature: await db.Temperature.findAll(),
    weight: await db.Weight.findAll()
  }
  res.json(data);
});

router.get('/getHives', async function (req, res, next){
  let data = await db.Hives.findAll();
  res.json(data);
});

router.post('/setOffset', async function (req, res, next){
  console.log('Got body:', req.body);
  db.Hives.update({
    hiveWeightOffset: req.body.offset
  }, {
    where: {
      id: req.body.id
    }
  })
  .then(result => {
    res.send(result);
  })
  .catch(err => {
    res.send(err);
  });
});

router.post('/setScale', async function (req, res, next){
  console.log('Got body:', req.body);
  db.Hives.update({
    hiveWeightCallibrationFactor: req.body.scale
  }, {
    where: {
      id: req.body.id
    }
  })
  .then(result => {
    res.send(result);
  })
  .catch(err => {
    res.send(err);
  });
});

module.exports = router;

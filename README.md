# HiveTracker

HiveTracker is a simple visualizer for your bee hive metrics. It saves and visualizes data collected from your MQTT broker.

## Prerequisites
 - docker
 - Mqtt broker
 - Node.js
 - NPM
 - Angular
 - A bee hive with sensors

## Installation
Clone the repository and install the dependencies:
```bash
git clone git@github.com:nicololuescher/Hivetracker.git

cd Hivetracker/database
docker-compose up -d

cd ../frontend
npm install

cd ../backend
npm install
npm run migrate
npm run pm2
```

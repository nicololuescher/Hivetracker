var express = require('express');
var router = express.Router();
var db = require('../database')

/* GET home page. */
router.get('/', function (req, res, next) {
  db.Test.findAll()
    .then(data => {
        res.status(200).send(JSON.stringify(data))
    })
    .catch(error => {
      res.status(500).send(error)
    });
});

module.exports = router;

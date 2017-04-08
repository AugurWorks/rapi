var extend = require('extend');
var demo = require('./lib/demo');
var datastore = require('./lib/datastore');

var express = require('express');
var app = express();

var child = require('child_process');

var defaultOptions = {
  exchange: 'CME',
  ticker: 'ESM7',
  username: process.env.USERNAME,
  password: process.env.PASSWORD
};

app.get('/', function (req, res) {
  var options = extend(true, {}, defaultOptions, req.query);

  var data = process.env.DEMO === 'true' ? demo(options) : datastore(options);

  res.send(data);
});

app.listen(3000, function () {
  console.log('RAPI is listening on port 3000');
});

var extend = require('extend');
var demo = require('./lib/demo');
var datastore = require('./lib/datastore');

var express = require('express');
var app = express();

var defaultOptions = {
  exchange: 'CME',
  ticker: 'ESM7',
  start: '1491226200',
  end: '1491312600',
  period: 15,
  type: 'MINUTE',
  username: process.env.USERNAME,
  password: process.env.PASSWORD
};

app.get('/', function (req, res) {
  var options = extend(true, {}, defaultOptions, req.query);
  options.start = parseInt(options.start);
  options.end = parseInt(options.end);

  var data = process.env.DEMO === 'true' ? demo(options) : datastore(options);

  res.send(data);
});

app.listen(3000, function () {
  console.log('RAPI is listening on port 3000');
});

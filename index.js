var extend = require('extend');
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

  var result = child.execSync('./SampleMD ' + [options.username, options.password, options.exchange, options.ticker].join(' '), {
    cwd: '/opt/app/datastore/samples'
  });
  var lines = new Buffer(result).toString().split('\n').filter(line => line.indexOf('DATA ') === 0);
  var data = lines.map(line => {
    var cols = line.slice("DATA ".length).split(' ');
    return {
      val: cols[0],
      date: new Date(parseInt(cols[1]) * 1000)
    };
  });
  res.send(data);
});

app.listen(3000, function () {
  console.log('RAPI is listening on port 3000');
});

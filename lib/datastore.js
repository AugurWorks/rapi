var child = require('child_process');

module.exports = datastore;

function datastore(options) {
  try {
    var result = child.execSync('./SampleMD ' + [options.username, options.password, options.exchange, options.ticker, options.period, options.start, options.end, options.type].join(' '), {
      cwd: '/opt/app/datastore/samples'
    });
    var lines = new Buffer(result).toString().split('\n').filter(line => line.indexOf('DATA ') === 0);
    return lines.map(line => {
      var cols = line.slice("DATA ".length).split(' ');
      return {
        val: cols[0],
        date: parseInt(cols[1]) * 1000
      };
    });
  } catch (e) {
    return datastore(options);
  }
}

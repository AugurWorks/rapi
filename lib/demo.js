var moment = require('moment');

module.exports = function(options) {
  var period = options.period * 60;
  return Array.apply(null, Array(Math.floor((options.end - options.start) / period))).map((empty, i) => {
    return options.start + i * period;
  }).filter(duringTradingHours(options.type)).map(epochTime => {
    return {
      date: epochTime,
      val: Math.round(20000 * (Math.random() + 1)) / 100
    };
  });
};

function duringTradingHours(type) {
  return function(epochTime) {
    if (type === 'DAY') {
      return true;
    }
    var date = moment(epochTime * 1000);
    var day = date.day();
    var minuteOfDay = date.hour() * 60 + date.minute();
    return day > 0 && day < 7 && minuteOfDay >= (9.5 * 60) && minuteOfDay <= (16 * 60);
  };
}

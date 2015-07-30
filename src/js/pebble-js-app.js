Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://kirby.ngrok.io';

  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

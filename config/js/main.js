var $submitButton = $('#submitButton');

$submitButton.on('click', function() {
  console.log('Submit');

  var return_to = getQueryParam('return_to', 'pebblejs://close#');
  document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
});

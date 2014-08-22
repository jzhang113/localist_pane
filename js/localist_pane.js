(function ($) {
  Drupal.behaviors.localist = {
    attach: function(context, settings) {
      $('.lwd', context).once('remove-attributes', function() {
        $(this).find('*').removeAttr('style');
      });
    }
  };

})(jQuery);

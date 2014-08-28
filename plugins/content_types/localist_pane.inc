<?php
/**
 * @file
 * Ctools content type plugin file for highcarts pane module.
 */

$plugin = array(
  "single" => TRUE, // Just do this one, it is needed.
  "title" => t("Localist"), // Title to show up on the pane screen.
  "description" => t("Pane with localist data"), // Description to show up on the pane screen.
  "category" => t("Localist"), // A category to put this under.
  "edit form" => "localist_pane_edit_form", // A function that will return the settings form for the pane.
  "render callback" => "localist_pane_render", // A function that will return the renderable content.
  "admin info" => "localist_pane_admin_info", // A function that will return the information displayed on the admin screen (optional).
  "defaults" => array( // Array of defaults for the settings form.
    "number_results" => "5",
    "days_ahead" => "31",
    "featured" => 0,
    "sponsored" => 0,
    "matching_instances" => 0,
    "venues" => "",
    "match" => "",
    "hidedesc" => 0,
    "hideimage" => 0,
    "image_size" => "",
    "show_times" => 1,
    "tags" => "",
  ),
  "all contexts" => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

function localist_pane_admin_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass();
    $block->title = t("Localist");
    return $block;
  }
}

/**
 * The 'Edit form' callback for the content type.
 */
function localist_pane_edit_form($form, &$form_state) {
  $conf = $form_state["conf"];

  $form["number_results"] = array(
    "#type" => "textfield",
    "#title" => t("Number of Results"),
    "#description" => t("Number of events to display. Maxinimum of 50."),
    "#default_value" => $conf["number_results"],
  );

  $form["days_ahead"] = array(
    "#type" => "textfield",
    "#title" => t("Days Ahead"),
    "#description" => t("Number of days in the future from which to display events. Maximum of 365."),
    "#default_value" => $conf["days_ahead"],
  );

  $form["tags"] = array(
    "#type" => "textfield",
    "#title" => t("Keywords and Tags"),
    "#description" => t("Separate keywords with commas."),
    "#default_value" => $conf["tags"],
  );

  $form["featured"] = array(
    "#type" => "checkbox",
    "#title" => t("Only show Featured Events"),
    "#default_value" => $conf["featured"],
  );

  $form["sponsored"] = array(
    "#type" => "checkbox",
    "#title" => t("Only Show Sponsored Events"),
    "#default_value" => $conf["sponsored"],
  );

  $form["matching_instances"] = array(
    "#type" => "checkbox",
    "#title" => t("Include All Matching Instances"),
    "#default_value" => $conf["matching_instances"],
  );

  $form["venues"] = array(
    "#type" => "select",
    "#title" => t("Places"),
    "#multiple" => TRUE,
    "#default_value" => $conf["venues"],
    "#options" => localist_pane_get_places(),
  );

  $form["match"] = array(
    "#type" => "select",
    "#title" => t("Match"),
    "#default_value" => $conf["match"],
    "#options" => array(
      "" => "At least one keyword or tag, and one filter item",
      "any" => "Any keyword, tag, or filter item",
      "all" => "All keywords, tags, and filter items",
    ),
  );

  $form["output"] = array(
    "#type" => "fieldset",
    "#title" => t("Output"),
  );

  $form["output"]["hidedesc"] = array(
    "#type" => "checkbox",
    "#title" => t("Hide Descriptions"),
    "#default_value" => $form_state["conf"]["hidedesc"],
  );

  $form["output"]["hideimage"] = array(
    "#type" => "checkbox",
    "#title" => t("Hide Event Image"),
    "#default_value" => $form_state["conf"]["hideimage"],
  );

  $form["output"]["image_size"]  = array(
    "#type" => "select",
    "#title" => t("Image Size"),
    "#options" => array(
      "tiny" => "Tiny (20x20)",
      "" => "Small (50x50)",
      "medium" => "Medium (80x80)",
      "big" => "Big (200x150)",
      "big_300" => "Large (300x225)",
    ),
    "#default_value" => $form_state["conf"]["image_size"],
    "#states" => array(
      // Hide the setting when the Hide Event Image checkbox is checked.
      "invisible" => array(
        ':input[name="hideimage"]' => array("checked" => TRUE),
      ),
    ),
  );

  $form["output"]["show_times"] = array(
    "#type" => "checkbox",
    "#title" => t("Show Event Times"),
    "#default_value" => $form_state["conf"]["show_times"],
  );

  return $form;
}

function localist_pane_edit_form_submit(&$form, &$form_state) {
  // Puts all values in the $conf array.
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function localist_pane_render($subtype, $conf, $panel_args, $context) {
  $src_url = "http://events.uiowa.edu/widget/view?schools=iowa&style=none&html_descriptions=1";

  $localist_args = array(
    "number_results", 
    "days_ahead",
    "tags",
    "featured", 
    "sponsored", 
    "matching_instances", 
    "venues", 
    "match", 
    "hidedesc", 
    "hideimage",
    "image_size",
    "show_times",
  );

  foreach ($localist_args as $arg) {
    if (!empty($conf[$arg])) {
      switch ($arg) {
        case "tags":
          $src_url .= "&" . $arg . "=" . str_replace(array(',', ', '), '%2C', check_plain($conf[$arg]));
          break;
        case "venues":
          $src_url .= "&" . $arg . "=" . implode('%2C', $conf[$arg]);
          break;
        default:
          $src_url .= "&" . $arg . "=" . $conf[$arg];
          break;
      }
    }
  }


  // https://api.drupal.org/api/drupal/includes!theme.inc/function/theme_html_tag/7
  $content = array(
    "#theme" => "html_tag",
    "#tag" => "script",
    "#value" => "",
    "#attributes" => array(
      "type" => "text/javascript",
      "src" => $src_url,
    ),
  );

  $block = new stdClass();
  $block->title = 'Events'; // The title may be overridden by the user within the panel options.
  $block->content = $content; // The inital content is blank.

  return $block;
}

function localist_pane_get_places() {
  $url = "http://events.uiowa.edu/api/2/places?pp=100";
  // Open up a cURL channel.
  $ch = curl_init($url);
  // Tell cURL to return the data as a string rather than print to the screen.
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  // Decode the JSON string returned by cURL into an associative array.
  $result = json_decode(curl_exec($ch), true);
  // Save the data we got back from the first page
  $places_data = $result["places"];
  curl_close($ch);

  // Initialize the page number and the URL for the second page.
  $page_number = 2;
  $url = $url . "&page=2";
  // cURL and loop through all the pages, combining the data until we're out of
  // pages.
  while ($page_number <= $result["page"]["total"]) {
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $results = json_decode(curl_exec($ch), true);
    curl_close($ch);
    $places_data = array_merge($places_data, $results["places"]);
    $page_number++;
    $url = "http://events.uiowa.edu/api/2/places?pp=100&page=" . strval($page_number);
  }

  $select_list_data = array();
  // Create an array to be used for the select list, where the key is
  // the urlname of the place and the value is the actual place name.
  foreach ($places_data as $place) {
    $select_list_data[$place["place"]["urlname"]] = $place["place"]["name"];
  }
  return $select_list_data;
}

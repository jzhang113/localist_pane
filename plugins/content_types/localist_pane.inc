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
"text" => "",
),
"all contexts" => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

function localist_pane_admin_info($subtype, $conf, $contexts) {
  $block = new stdClass();
  $block->title = t("Localist");
  return $block;
}

/**
 * The 'Edit form' callback for the content type.
 */
function localist_pane_edit_form($form, &$form_state) {
  $conf = $form_state["conf"];

  $form["widget_type"] = array(
    "#type" => "select",
    "#title" => t("Widget Type"),
    "#options" => array(
      "view" => t("List"),
      "combo" => t("Mini-Calendar + List"),
    ),
    "#default_value" => isset($conf["widget_type"]) ? $conf["widget_type"] : "",
  );

  $form["number_results"] = array(
    "#type" => "textfield",
    "#title" => t("Number of Results?"),
    "#description" => t("Number of events to grab"),
    "#default_value" => isset($conf["number_results"]) ? $conf["number_results"] : "",
  );

  $form["days_ahead"] = array(
    "#type" => "textfield",
    "#title" => t("Days Ahead?"),
    "#default_value" => isset($conf["days_ahead"]) ? $conf["days_ahead"] : "",
  );

  $form["featured"] = array(
    "#type" => "checkbox",
    "#title" => t("Only show Featured Events"),
    "#default_value" => isset($conf["featured"]) ? $conf["featured"] : 0,
  );

  $form["sponsored"] = array(
    "#type" => "checkbox",
    "#title" => t("Only Show Sponsored Events"),
    "#default_value" => isset($conf["sponsored"]) ? $conf["sponsored"] : 0,
  );

  $form["matching_instances"] = array(
    "#type" => "checkbox",
    "#title" => t("Include All Matching Instances"),
    "#default_value" => isset($conf["matching_instances"]) ? $conf["matching_instances"] : 0,
  );

  $form["places"] = array(
    "#type" => "select",
    "#title" => t("Calendar ID"),
    "#description" => t("ID of the Calendar to pull events from"),
    "#default_value" => isset($conf["places"]) ? $conf["places"] : "",
    "#options" => localist_pane_get_places(),
  );

  return $form;
}

function localist_pane_edit_form_submit(&$form, &$form_state) {
  // Puts all values in the $conf array.
  foreach (array("calendar_id") as $key) {
    $form_state["conf"][$key] = $form_state["values"][$key];
  }
}

function localist_pane_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();
  $block->title = $conf["title"];
  $block->content = array();
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

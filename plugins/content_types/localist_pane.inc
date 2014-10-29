<?php
/**
 * @file
 * Ctools content type plugin file for highcarts pane module.
 */

$plugin = array(
  "single" => TRUE, // Just do this one, it is needed.
  "title" => t("Localist Events Pane"), // Title to show up on the pane screen.
  "description" => t("A list of events from Localist"), // Description to show up on the pane screen.
  "category" => t("Localist Events"), // A category to put this under.
  "edit form" => "localist_pane_edit_form", // A function that will return the settings form for the pane.
  "render callback" => "localist_pane_render", // A function that will return the renderable content.
  "admin info" => "localist_pane_admin_info", // A function that will return the information displayed on the admin screen (optional).
  "defaults" => array( // Array of defaults for the settings form.
    "num" => "5",
    "days" => "31",
    "featured" => 0,
    "sponsored" => 0,
    "matching_instances" => 0,
    "venues" => "",
    "match" => "",
    "hidedesc" => 0,
    "hideimage" => 0,
    "image_size" => "",
    "show_times" => 0,
    "tags" => "",
    "departments" => "",
    "event_types" => "",
    "audience" => "",
    "general_interest" => "",
    "more" => 0,
    "moretext" => "more",
    "moreurl" => "http://events.uiowa.edu",
  ),
  "all contexts" => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);

function localist_pane_admin_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass();
    $block->title = t("Localist Pane");
    return $block;
  }
}

/**
 * The 'Edit form' callback for the content type.
 */
function localist_pane_edit_form($form, &$form_state) {
  $conf = $form_state["conf"];

  $form["num"] = array(
    "#type" => "textfield",
    "#title" => t("Number of Results"),
    "#description" => t("Number of events to display. Maxinimum of 50."),
    "#default_value" => $conf["num"],
  );

  $form["days"] = array(
    "#type" => "textfield",
    "#title" => t("Days Ahead"),
    "#description" => t("Number of days in the future from which to display events. Maximum of 365."),
    "#default_value" => $conf["days"],
  );

  $form["filter_events"] = array(
    "#type" => "fieldset",
    "#title" => t("Filter Events"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form["filter_events"]["match"] = array(
    "#type" => "select",
    "#title" => t("Match"),
    "#default_value" => $conf["match"],
    "#options" => array(
      "" => "At least one keyword or tag, and one filter item",
      "any" => "Any keyword, tag, or filter item",
      "all" => "All keywords, tags, and filter items",
    ),
  );

  $form["filter_events"]["featured"] = array(
    "#type" => "checkbox",
    "#title" => t("Only Show Featured Events"),
    "#default_value" => $conf["featured"],
  );

  $form["filter_events"]["sponsored"] = array(
    "#type" => "checkbox",
    "#title" => t("Only Show Sponsored Events"),
    "#default_value" => $conf["sponsored"],
  );

  $form["filter_events"]["matching_instances"] = array(
    "#type" => "checkbox",
    "#title" => t("Include All Matching Instances"),
    "#default_value" => $conf["matching_instances"],
  );

  $form["filter_events"]["tags"] = array(
    "#type" => "textfield",
    "#title" => t("Keywords and Tags"),
    "#description" => t("Separate keywords with commas."),
    "#default_value" => $conf["tags"],
  );

  // Grab the all the filters.
  $filters = localist_pane_get_filters();

  $form["filter_events"]["filter_events_filters"] = array(
    "#type" => "fieldset",
    "#title" => t("Filters"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form["filter_events"]["filter_events_filters"]["event_types"] = array(
    "#type" => "select",
    "#multiple" => TRUE,
    "#title" => t("Event Types"),
    "#default_value" => $form_state["conf"]["event_types"],
    "#options" => $filters["event_types"],
    '#size'=> 7,
  );

  $form["filter_events"]["filter_events_filters"]["departments"] = array(
    "#type" => "select",
    "#multiple" => TRUE,
    "#title" => t("Department"),
    "#default_value" => $form_state["conf"]["departments"],
    "#options" => $filters["departments"],
    '#size'=> 10,
  );

  $form["filter_events"]["filter_events_filters"]["audience"] = array(
    "#type" => "select",
    "#multiple" => TRUE,
    "#title" => t("Audience"),
    "#default_value" => $form_state["conf"]["audience"],
    "#options" => $filters["event_audience"],
    '#size'=> 6,
  );

  $form["filter_events"]["filter_events_filters"]["general_interest"] = array(
    "#type" => "select",
    "#multiple" => TRUE,
    "#title" => t("General Interest"),
    "#default_value" => $form_state["conf"]["general_interest"],
    "#options" => $filters["event_general_interest"],
    '#size'=> 7,
  );

  $form["filter_events"]["filter_events_filters"]["venues"] = array(
    "#type" => "select",
    "#title" => t("Places"),
    "#multiple" => TRUE,
    "#default_value" => $conf["venues"],
    "#options" => localist_pane_get_places(),
    '#size'=> 10,
  );

  $form["output"] = array(
    "#type" => "fieldset",
    "#title" => t("Output"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form["output"]["hidedesc"] = array(
    "#type" => "checkbox",
    "#title" => t("Hide Descriptions"),
    "#default_value" => $form_state["conf"]["hidedesc"],
  );

  $form["output"]["show_times"] = array(
    "#type" => "checkbox",
    "#title" => t("Hide Event Times"),
    "#default_value" => $form_state["conf"]["show_times"],
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

  $form["output"]["more"] = array(
    "#type" => "checkbox",
    "#title" => t("Display a more link"),
    "#description" => t("Add a link to a list of more events."),
    "#default_value" => $form_state["conf"]["more"],
  );

  $form["output"]["moretext"] = array(
    "#type" => "textfield",
    "#title" => t("Link text"),
    "#description" => t("The text shown that the user will click on."),
    "#default_value" => $form_state["conf"]["moretext"],
    "#states" => array(
      // Hide the setting when the More Link checkbox is not checked.
      "invisible" => array(
        ':input[name="more"]' => array("checked" => FALSE),
      ),
    ),
  );

  $form["output"]["moreurl"] = array(
    "#type" => "textfield",
    "#title" => t("Link url"),
    "#description" => t("The link location. Usually to a filter landing page on http://events.uiowa.edu"),
    "#default_value" => $form_state["conf"]["moreurl"],
    "#states" => array(
      // Hide the setting when the More Link checkbox is not checked.
      "invisible" => array(
        ':input[name="more"]' => array("checked" => FALSE),
      ),
    ),
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
  // Combine all filters into one array.
  $types = array_merge(
    $form_state["conf"]["departments"],
    $form_state["conf"]["event_types"],
    $form_state["conf"]["audience"],
    $form_state["conf"]["general_interest"]
  );


  $form_state["conf"]["types"] = implode("%2C", $types);
}

function localist_pane_render($subtype, $conf, $panel_args, $context) {
  $src_url = "http://events.uiowa.edu/widget/view?schools=iowa&style=none&html_descriptions=1";

  $localist_args = array(
    "num",
    "days",
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
    "types",
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
        case "show_times":
          $src_url .= "&show_times=0";
          break;
        case "featured":
          $src_url .= "&picks=1";
          break;
        default:
          $src_url .= "&" . $arg . "=" . $conf[$arg];
          break;
      }
    }
  }

  // https://api.drupal.org/api/drupal/includes!theme.inc/function/theme_html_tag/7
  $content = array(
    "events" => array(
      "#theme" => "html_tag",
      "#tag" => "script",
      "#value" => "",
      "#attributes" => array(
        "type" => "text/javascript",
        "src" => $src_url,
      ),
    ),
  );

  if(!empty($conf["more"])) {
    // Provide a more link.
    $content['more'] = array(
      "#type" => 'link',
      "#title" => $conf["moretext"],
      "#href" => $conf["moreurl"],
      "#options" => array(
        "attributes" => array("class" => array("more-link")),
      ),
    );
  }

  $block = new stdClass();
  $block->title = 'Events'; // The title may be overridden by the user within the panel options.
  $block->content = $content; // The inital content is blank.

  return $block;
}

function localist_pane_get_places() {
  $url = "http://events.uiowa.edu/api/2/places?pp=100";
  // Grab the JSON decoded data.
  $results = _localist_pane_decode($url);
  // Get the places array.
  $places_data = $results["places"];

  // Initialize the page number and the URL for the second page.
  $page_number = 2;
  $url = $url . "&page=2";
  // Loop through all the pages, combining the data until we're out of pages.
  while ($page_number <= $results["page"]["total"]) {
    $results = _localist_pane_decode($url);
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


function localist_pane_get_filters() {
  $url = "http://events.uiowa.edu/api/2/events/filters";
  $results = _localist_pane_decode($url);
  $query_data = array();
  // Get a handle on each filter grouping (Department, event types, etc.)
  foreach ($results as $result => $items) {
    $query_data[$result] = array();
    foreach ($items as $item) {
      // Add each filter term to its group, using the terms ID as the key
      // and the name as the value.
      $query_data[$result][$item["id"]] = $item["name"];
    }
    // Order the filter terms in each group alphabetically.
    asort($query_data[$result]);
  }
  return $query_data;
}

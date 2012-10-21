<?php

/**
 * Helper function called by preprocess_page().
 */
function _sneakerz_menu_preprocess_page(&$variables) {
  if (theme_get_setting('toggle_main_menu')) {
    $level = 2;
    $tree = menu_tree_page_data(variable_get('menu_main_links_source', 'main-menu'), $level);
    $router_item = menu_get_item();
    $variables['main_menu'] = sneakerz_menu_tree_format($tree, $router_item);
  }
  else {
    $variables['main_menu'] = array ();
  }
}

/**
 * Replace theme_menu_item_link() since special_menu_items does the same.
 **/
function _sneakerz_menu_theme_registry_alter(&$registry) {
  // Save previous value from registry in case another theme overwrites
  // menu_item_link
  $registry['menu_link_default'] = $registry['menu_link'];
  $registry['menu_link']['function'] = 'sneakerz_menu_link';
}

/**
 * Overrides theme_menu_link().
 **/
function sneakerz_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  if (isset($element['#href']) && ($element['#href'] == $_GET['q'] || ($element['#href'] == '<front>' && drupal_is_front_page()))) {
     $element['#attributes']['class'][] = 'active';
  }
  switch ($element['#href']) {
      case '<nolink>':
        $tag = variable_get('special_menu_items_nolink_tag', '<span>');
        $title = strip_tags(l($element['#title'], $element['#href'], $element['#localized_options']));
        $output = special_menu_items_render_menu_item($tag, $title);
        $element['#attributes']['class'][] = 'nav-header';
        break;
      case '<separator>':
        $output = variable_get('special_menu_items_separator_value', '<hr>');
        $element['#attributes']['class'][] = 'divider';
        break;
      default:
        $output = l($element['#title'], $element['#href'], $element['#localized_options']);
        break;
    }
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Overrides theme_menu_tree().
 **/
function sneakerz_menu_tree($variables) {
  return '<ul class="nav nav-list">' . $variables['tree'] . '</ul>';
}

/**
 * Helper function to format menu tree data.
 */
function sneakerz_menu_tree_format($tree, $router_item) {
  $links = array();
  foreach ($tree as $item) {
    if (!$item['link']['hidden']) {
      $class = '';
      $l = $item['link']['localized_options'];
      $l['href'] = $item['link']['href'];
      $l['title'] = $item['link']['title'];
      if ($item['link']['in_active_trail']) {
        $class = ' active-trail';
        $l['attributes']['class'][] = 'active-trail';
      }
      // Normally, l() compares the href of every link with $_GET['q'] and sets
      // the active class accordingly. But local tasks do not appear in menu
      // trees, so if the current path is a local task, and this link is its
      // tab root, then we have to set the class manually.
      if ($item['link']['href'] == $router_item['tab_root_href'] && $item['link']['href'] != $_GET['q']) {
        $l['attributes']['class'][] = 'active';
      }

      // Child items
      if (isset($item['below']) && !empty($item['below'])) {
        $l['below'] = sneakerz_menu_tree_format($item['below'], $router_item);
      }

      // Keyed with the unique mlid to generate classes in theme_links().
      $links['menu-' . $item['link']['mlid'] . $class] = $l;
    }
  }
  return $links;
}

/**
 * Overrides theme_links().
 */
// function sneakerz_links($variables) {
//   kpr($variables);
//   global $language_url;

//   $links = $variables['links'];
//   $attributes = $variables['attributes'];
//   $attributes['class'] = array();
//   $heading = $variables['heading'];

//   if ($variables['attributes']['class'][0] != 'contextual-links') {
//     $output = '';
//     $output = _sneakerz_menu_links($links, $attributes);
//     return $output;
//   } 
// }

/**
 * Overrides theme_links() for system_main_menu.
 */
function sneakerz_links__system_main_menu($variables) {
  $links = $variables['links'];
  $attributes = $variables['attributes'];
  $attributes['class'] = array();
  $heading = $variables['heading'];
  global $language_url;
  $output = '';
  $output = _sneakerz_menu_links($links, $attributes, TRUE);
  return $output;
}

/**
 * Helper function to format HTML output for a menu.
 */
function _sneakerz_menu_links($links, $attributes, $horizontal = FALSE, $depth = 0) {
  $output = '';
  if (count($links) > 0) {
    if ($depth == 0) {
      $attributes['class'][] = 'nav';
    }
    $output = '<ul' . drupal_attributes($attributes) . '>';
    $attributes = array();

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
      $class = array($key);

      // Add first, last and active classes to the list of links to help out
      // themers.
      if ($i == 1) {
        $class[] = 'first';
      }
      if ($i == $num_links) {
        $class[] = 'last';
      }
      if (isset($link['href']) && ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()))
           && (empty($link['language']) || $link['language']->language == $language_url->language)) {
        $class[] = 'active';
      }

      $close_tag = FALSE;
      // Seperators
      if (isset($link['href']) && $link['href'] == '<separator>') {
        if ($depth == 0 && $horizontal) {
          $class[] = 'divider-vertical';
        }
        else {
          $class[] = 'divider';
        }
        $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';
        $close_tag = TRUE;
      }
      // Horizontal Dropdowns
      else if ($horizontal && $depth == 0 && isset($link['below']) && !empty($link['below'])) {
        $class[] = 'dropdown';
        $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';
        $link['attributes']['class'][] = 'dropdown-toggle';
        $link['attributes']['data-toggle'] = 'dropdown';
        $link['attributes']['data-target'] = '#';
        $link['html'] = TRUE;
        $link['title'] = $link['title'] . '<b class="caret"></b>';
        if ($link['href'] == '<nolink>') {
          $link['href'] = '';
          $link['external'] = TRUE;
        }
        // Pass in $link as $options, they share the same keys.
        $output .= l($link['title'], $link['href'], $link);
        $close_tag = TRUE;
        $attributes['class'][] = 'dropdown-menu';
      }
      // Nav Headers
      else if (isset($link['href']) && $link['href'] == '<nolink>') {
        $class = 'nav-header';
        $output .= '<li' . drupal_attributes(array('class' => $class)) . '>' . $link['title'];
        $close_tag = TRUE;
      }
      else {
        $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';
        if (isset($link['href'])) {
          // Pass in $link as $options, they share the same keys.
          $output .= l($link['title'], $link['href'], $link);
        }
        elseif (!empty($link['title'])) {
          // Some links are actually not links, but we wrap these in <span> for
          // adding title and class attributes.
          if (empty($link['html'])) {
            $link['title'] = check_plain($link['title']);
          }
          $span_attributes = '';
          if (isset($link['attributes'])) {
            $span_attributes = drupal_attributes($link['attributes']);
          }
          $output .= '<span' . $span_attributes . '>' . $link['title'] . '</span>';
        }
        $close_tag = TRUE;

      }

      if ($close_tag) {
        // Child links
        if (isset($link['below']) && !empty($link['below'])) {
          $output .= _sneakerz_menu_links($link['below'], $attributes, $horizontal, $depth + 1);
        }

        $i++;
        $output .= "</li>\n";
      }
    }

    $output .= '</ul>';
  }
  return $output;
}

/**
 * Overrides theme_menu_local_tasks().
 */
function sneakerz_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="nav nav-tabs primary">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="nav nav-pills secondary">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

<?php

/**
 * Override for theme_breadcrumb().
 */
function sneakerz_breadcrumb($variables) {
  $breadcrumbs = $variables['breadcrumb'];

  if (!empty($breadcrumbs)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    $output .= '<ul class="breadcrumb">';
    $count = count($breadcrumbs) - 1;
    foreach ($breadcrumbs as $i => $breadcrumb) {
      if ($i == $count) {
        $output .= '<li>' . $breadcrumb . '</li>';
      }
      else {
        $output .= '<li>' . $breadcrumb . '<span class="divider">/</span></li>';
      }
    }

    $output .= '</ul>';
    return $output;
  }
}

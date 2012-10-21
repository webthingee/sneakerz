<?php

function sneakerz_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  $status_heading = array(
    'status' => array('', ' alert-info'), 
    'error' => array(t('Error'), ' alert-error'), 
    'warning' => array(t('Warning'), ''),
  );
  foreach (drupal_get_messages($display) as $type => $messages) {
    foreach ($messages as $message) {
      $output .= '<div class="alert alert-block' . $status_heading[$type][1] . '">';
      //$output .= '<button class="close" data-dismiss="alert">×</button>';
      $output .= '<h4 class="alert-heading">' . $status_heading[$type][0] . '</h4>';
      $output .= $message;
      $output .= '</div>';
    }
  }
  return $output;
}


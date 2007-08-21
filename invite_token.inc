<?php
// $Id: invite_token.inc,v 1.2 2007-08-21 19:46:22 smk Exp $

/**
 * For a given context, builds a formatted list of tokens and descriptions
 * of their replacement values.
 *
 * @param type
 *    The token types to display documentation for. Defaults to 'all'.
 * @param prefix
 *    The prefix your module will use when parsing tokens. Defaults to '['
 * @param suffix
 *    The suffix your module will use when parsing tokens. Defaults to ']'
 * @return An HTML table containing the formatting docs.
 **/
function theme_invite_token_help($type = 'all', $prefix = '[', $suffix = ']') {
  token_include();

  // see http://drupal.org/node/127072
  $full_list = array();
  foreach ((array)$type as $t) {
    $full_list = array_merge($full_list, token_get_list($t));
  }
  
  $headers = array(t('Token'), t('Replacement value'));
  $rows = array();
  foreach ($full_list as $key => $category) {
    $rows[] = array(array('data' => drupal_ucfirst($key) . ' ' . t('tokens'), 'class' => 'region', 'colspan' => 2));
    foreach ($category as $token => $description) {
      $row = array();
      $row[] = $prefix . $token . $suffix;
      $row[] = $description;
      $rows[] = $row;
    }
  }

  $output = theme('table', $headers, $rows, array('class' => 'description'));
  return $output;
}

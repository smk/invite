<?php
// $Id: invite_token.inc,v 1.1.2.1.2.1 2008-02-13 01:51:36 smk Exp $

/**
 * Implementation of hook_token_values().
 */
function invite_token_values($type = 'all', $object = NULL) {
  $values = array();
  if ($type == 'invite' && is_object($object)) {
    // NOTE: Invite is currently only capable of sending plain text e-mails.
    // If you intend to change that, you need to properly escape all user input
    // by adding check_plain() around below values!
    $values['inviter-raw']        = $object->inviter->name;
    $values['invite-mail']        = $object->email;
    $values['invite-message-raw'] = $object->data['message'];
    $values['join-link']          = url('invite/accept/'. $object->code, NULL, NULL, TRUE);
  }
  return $values;
}

/**
 * Implementation of hook_token_list().
 */
function invite_token_list($type = 'all') {
  if ($type == 'invite' || $type == 'all') {
    $tokens['invite']['inviter-raw']        = t("Inviter's name. WARNING - raw user input.");
    $tokens['invite']['invite-mail']        = t('The e-mail address of the invited user.');
    $tokens['invite']['invite-message-raw'] = t('The personal message for the invitee as unfiltered text. WARNING - raw user input.');
    $tokens['invite']['join-link']          = t('The link to the registration page of the site.');
    return $tokens;
  }
}

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
 */
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


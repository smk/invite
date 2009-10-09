<?php
// $Id$

/**
 * Form builder; generate the invite page form.
 *
 * @param $edit
 *   Previous values when resending an invite.
 *
 * @ingroup forms
 * @see invite_form_validate()
 * @see invite_default_validators()
 * @see invite_form_submit()
 */
function invite_page_form(&$form_state, $edit = array()) {
  global $user;

  $remaining_invites = invite_get_remaining_invites($user);

  if ($remaining_invites == 0 && !$edit) {
    // Deny access when NOT resending an invite.
    drupal_set_message(t("Sorry, you've reached the maximum number of invitations."), 'error');
    drupal_goto(referer_uri());
  }

  $form['sent'] = array(
    '#type' => 'value',
    '#value' => $edit ? $edit['sent'] : 0,
  );
  $form['code'] = array(
    '#type' => 'value',
    '#value' => $edit ? $edit['code'] : NULL,
  );
  if ($remaining_invites != INVITE_UNLIMITED) {
    $form['remaining_invites'] = array(
      '#type' => 'value',
      '#value' => $remaining_invites,
    );
  }

  // Remaining invites.
  if ($remaining_invites != INVITE_UNLIMITED) {
    $form['remaining_invites_markup']['#value'] = format_plural($remaining_invites, 'You have 1 invite remaining.', 'You have @count invites remaining.');
  }

  // Sender e-mail address.
  if ($user->uid && variable_get('invite_use_users_email', 0)) {
    $from = $user->mail;
  }
  else {
    $from = variable_get('site_mail', ini_get('sendmail_from'));
  }
  // Personalize displayed e-mail address.
  // @see http://drupal.org/project/pmail
  if (module_exists('pmail')) {
    $from = personalize_email($from);
  }
  $form['from'] = array(
    '#type' => 'item',
    '#title' => t('From'),
    '#value' => check_plain($from),
  );

  // Recipient email address.
  if (!$edit) {
    $failed_emails = '';
    $allow_multiple = user_access('send mass invitations');
    if (isset($_SESSION[INVITE_SESSION_FAILED])) {
      $failed_emails = implode("\n", unserialize($_SESSION[INVITE_SESSION_FAILED]));
      unset($_SESSION[INVITE_SESSION_FAILED]);
    }
    $form['email'] = array(
      '#title' => t('To'),
      '#default_value' => $failed_emails,
      '#description' => format_plural($allow_multiple ? 99 : 1, 'Enter the e-mail address of the person you would like to invite.', 'Enter the e-mail addresses of the persons you would like to invite. To specify multiple recipients, enter one e-mail address per line or separate each address with a comma.'),
      '#required' => TRUE,
    );
    if ($allow_multiple) {
      $form['email']['#type'] = 'textarea';
      $form['email']['#rows'] = 3;
    }
    else {
      $form['email']['#type'] = 'textfield';
      $form['email']['#maxlength'] = 64;
    }
    if ($failed_emails) {
      $form['email']['#attributes']['class'] = 'error';
    }
  }
  else {
    // The email is not editable when resending an invite.
    $allow_multiple = FALSE;
    $form['email_markup'] = array(
      '#type' => 'item',
      '#title' => t('To'),
      '#value' => check_plain($edit['email']),
    );
    $form['email'] = array(
      '#type' => 'value',
      '#value' => $edit['email'],
    );
  }

  // Message subject.
  if ($edit && !empty($edit['data']['subject'])) {
    $subject = $edit['data']['subject'];
  }
  else {
    $subject = invite_get_mail_subject();
  }
  // Add prefix.
  $prefix = t('Re:');
  if ($edit && drupal_substr($subject, 0, strlen($prefix)) != $prefix) {
    $subject = $prefix .' '. $subject;
  }
  if (variable_get('invite_subject_editable', FALSE)) {
    $form['subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Subject'),
      '#default_value' => $subject,
      '#maxlength' => 64,
      '#description' => t('Type the subject of the invitation e-mail.'),
      '#required' => TRUE,
    );
  }
  else {
    $form['subject'] = array(
      '#type' => 'item',
      '#title' => t('Subject'),
      '#value' => check_plain($subject),
    );
  }

  // Message body.
  $form['body'] = array(
    '#type' => 'item',
    '#title' => t('Message'),
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#default_value' => ($edit && !empty($edit['data']['message'])) ? $edit['data']['message'] : '',
    '#description' => format_plural($allow_multiple ? 1 : 99, 'This message will be added to the mail sent to the person you are inviting.', 'This message will be added to the mail sent to the persons you are inviting.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send invite'),
  );

  $form['#validate'] = array('invite_form_validate');
  if (!$edit) {
    $form['#validate'] = array_merge($form['#validate'], invite_default_validators());
  }
  $form['#submit'] = array('invite_form_submit');

  return $form;
}

/**
 * Theme invite page form.
 *
 * @ingroup themeable
 */
function theme_invite_page_form($form) {
  $output = '';

  $output .= drupal_render($form['remaining_invites_markup']);
  $output .= drupal_render($form['remaining_invites']);
  $output .= drupal_render($form['from']);
  if (isset($form['email_markup'])) {
    $output .= drupal_render($form['email_markup']);
  }
  $output .= drupal_render($form['email']);
  $output .= drupal_render($form['subject']);

  // Show a preview of the invitation message. Replace the [message] token
  // with the rendered message input form field.
  $output .= drupal_render($form['body']);
  $output .= '<div class="invite-message"><div class="opening">';
  $message_form = "</p></div>\n". drupal_render($form['message']) ."\n<div class=\"closing\"><p>";
  module_load_include('inc', 'invite');
  $invite = new invite(array('data' => array('message' => $message_form)));
  $output .= $invite->get_message_body(TRUE);

  $output .= "</div></div>\n";

  // Render all remaining form elements.
  $output .= drupal_render($form);

  return $output;
}

/**
 * Forms API callback; filter out already registered users.
 *
 * @see invite_page_form()
 * @see invite_block_form()
 */
function invite_form_registered_validate($form, &$form_state) {
  $emails = &$form_state['emails']['valid'];

  if (count($emails) > 0) {
    $registered = invite_filter_emails("SELECT mail AS email FROM {users} WHERE mail IN (". db_placeholders($emails, 'varchar') .")", $emails);
    if ($registered) {
      foreach ($registered as $key => $email) {
        $registered[$key] = theme('username', user_load(array('mail' => $email)));
      }
      $error = format_plural(count($registered), 'The following recipient is already registered at this site:', 'The following recipients are already registered at this site:') .'<br />';
      $error .= implode(', ', $registered);
      drupal_set_message($error, 'error');
    }
  }
}

/**
 * Forms API callback; filter out already invited e-mail addresses.
 *
 * @see invite_page_form()
 * @see invite_block_form()
 */
function invite_form_invited_validate($form, &$form_state) {
  global $user;

  $emails = &$form_state['emails']['valid'];

  if (count($emails) > 0) {
    $invited = invite_filter_emails("SELECT email FROM {invite} WHERE email IN (". db_placeholders($emails, 'varchar') .") AND uid = %d AND canceled = 0", $emails, $user->uid);
    if ($invited) {
      $error = format_plural(count($invited), 'You have already invited the following recipient:', 'You have already invited the following recipients:') .'<br />';
      $error .= implode(', ', array_map('check_plain', $invited));
      drupal_set_message($error, 'error');
    }
  }
}

/**
 * Forms API callback; check invite limits.
 *
 * @see invite_page_form()
 * @see invite_block_form()
 */
function invite_form_limit_validate($form, &$form_state) {
  $remaining = $form_state['values']['remaining_invites'];
  if (count($form_state['emails']['valid']) > $remaining) {
    form_set_error('email', format_plural($remaining, 'You have only 1 invite left.', 'You have only @count invites left.'));
  }
}

/**
 * Forms API callback; check that at least one e-mail address is left after
 * processing the filter chain. This validator should always run last.
 *
 * @see invite_page_form()
 * @see invite_block_form()
 */
function invite_form_final_validate($form, &$form_state) {
  if (count($form_state['emails']['valid']) == 0) {
    form_set_error('email');
  }
}

/**
 * Forms API callback; process submitted form data.
 *
 * @see invite_page_form()
 * @see invite_block_form()
 */
function invite_form_submit($form, &$form_state) {
  global $user;

  // Set this now, so other modules can change it later.
  $form_state['redirect'] = 'invite';

  // Initialize invite object options.
  $options = $form_state['values'];
  $options['subject'] = isset($options['subject']) ? trim($options['subject']) : invite_get_mail_subject();
  $options['message'] = isset($options['message']) ? trim($options['message']) : '';

  $succeeded = $failed = 0;
  foreach ($form_state['emails']['valid'] as $email) {
    if (!invite_send('invite', $email, $options)) {
      $form_state['emails']['failed'][] = $email;
      $failed++;
    }
    else {
      $succeeded++;
    }
  }

  // Store failed e-mails for re-display.
  if ($form_state['emails']['failed']) {
    $_SESSION[INVITE_SESSION_FAILED] = serialize($form_state['emails']['failed']);
  }

  if ($succeeded > 0) {
    if (isset($form_state['values']['remaining_invites'])) {
      // Update user property if user has limited invites.
      user_save($user, array('invites' => $form_state['values']['remaining_invites'] - $succeeded));
    }
    drupal_set_message(format_plural($succeeded, 'Your invitation has been successfully sent. You will be notified when the recipient joins the site.', '@count invitations have been successfully sent. You will be notified when any recipient joins the site.'));
  }

  if ($failed > 0) {
    drupal_set_message(format_plural($num_failed, 'The entered e-mail address is invalid. Please correct it.', '@count entered e-mail addresses are invalid. Please correct them.'), 'error');
  }
  else if (user_access('track invitations') && $user->uid) {
    // Everything went well: redirect to pending invites page.
    $form_state['redirect'] = "user/$user->uid/invites/pending";
  }
}

/**
 * Extract valid e-mail addresses from a string.
 *
 * E-mails must be separated by newlines or commas. E-mails are allowed to
 * include a display name (eg. Some Name <foo@example.com>). Invalid addresses
 * are filtered out and stored in a session variable for re-display.
 *
 * @param $string
 *   The string to process. Recognized delimiters are comma, NL and CR.
 * @return
 *   Array of valid e-mail addresses.
 */
function invite_extract_emails($string) {
  $valid = $failed = array();

  $user = '[a-zA-Z0-9_\-\.\+\^!#\$%&*+\/\=\?\`\|\{\}~\']+';
  $domain = '(?:(?:[a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.?)+';
  $ipv4 = '[0-9]{1,3}(\.[0-9]{1,3}){3}';
  $ipv6 = '[0-9a-fA-F]{1,4}(\:[0-9a-fA-F]{1,4}){7}';
  $rx = "/($user@($domain|(\[($ipv4|$ipv6)\])))>?$/";

  $emails = array_unique(split("[,\n\r]", $string));
  foreach ($emails as $email) {
    $email = preg_replace('/^.*<(.*)>$/', '${1}', trim($email));
    if ($email) {
      if (preg_match($rx, $email, $match)) {
        $valid[] = $match[1];
      }
      else {
        $failed[] = $email;
      }
    }
  }

  return array(
    'valid' => $valid,
    'failed' => $failed
  );
}

/**
 * Filter out e-mails based on a database query.
 *
 * @param $sql
 *   The query to execute. The query is expected to return one field named
 *   email which is considered a "bad" address to be filtered out.
 * @param &$emails
 *   The list of e-mail addresses to validate. When this function returns, all
 *   invalid e-mails have been removed.
 * @param ...
 *   Any required arguments for the query.
 * @return
 *   An array of invalid e-mail addresses.
 */
function invite_filter_emails($sql, &$emails) {
  $filtered = array();
  // Build query arguments.
  $args = func_get_args();
  $args = array_merge($emails, array_slice($args, 2));
  $result = db_query($sql, $args);
  while ($row = db_fetch_object($result)) {
    $filtered[] = $row->email;
  }
  // Keep only valid e-mails.
  $emails = array_diff($emails, $filtered);
  return $filtered;
}

/**
 * Menu callback; accept an invite.
 *
 * @param $invite
 *   An invite object.
 */
function invite_accept($invite) {
  global $user;

  if ($user->uid) {
    drupal_goto();
  }
  else if (!$invite || !$invite->inviter) {
    drupal_set_message(t('This invitation has been withdrawn.'));
    drupal_goto();
  }
  else if ($invite->joined) {
    drupal_set_message(t('This invitation has already been used. Please login now with your username and password.'));
    drupal_goto('user');
  }
  else if ($invite->expiry < time()) {
    drupal_set_message(t('Sorry, this invitation has expired.'));
    drupal_goto();
  }

  // Save invite code to session and redirect to user register form.
  $_SESSION[INVITE_SESSION] = $invite->code;
  drupal_goto('user/register');
}

/**
 * Menu callback; resend an expired invite.
 *
 * @param $invite
 *   An invitate object.
 */
function invite_resend($invite) {
  global $user;

  // Inviter must match current user and invitation must have expired.
  if ($invite->inviter->uid != $user->uid || $invite->expiry > time()) {
    return drupal_access_denied();
  }

  return drupal_get_form('invite_page_form', $invite);
}

/**
 * Form builder; display confirm form to delete an invitation.
 *
 * @param $form_satate
 *   A keyed array containing the current state of the form.
 * @param $origin
 *   A string denoting the orginating status page to return the user to
 *   afterwards.
 * @param $code
 *   A registration code to remove the invitation for.
 *
 * @ingroup forms
 * @see invite_cancel_form_submit()
 */
function invite_cancel_form(&$form_state, $origin, $code) {
  global $user;

  $invite = invite_load($code);

  // Inviter must be the current user.
  if ($invite->inviter->uid == $user->uid) {
    // Verify the invitation may be deleted.
    if (!$invite->joined || user_access('withdraw accepted invitations')) {
      $form['#redirect'] = "user/$user->uid/invites/$origin";
      $form['invite'] = array(
        '#type' => 'value',
        '#value' => $invite,
      );
      $description = (!$invite->joined && $invite->expiry > time()) ? t("The invitee won't be able to register any more using this invitation.") : '';

      return confirm_form(
        $form,
        t('Are you sure you want to withdraw the invitation to %email?', array('%email' => $invite->email)),
        "user/$user->uid/invites/{$origin}",
        $description .' '. t('This action cannot be undone.'),
        t('Withdraw'),
        t('Cancel')
      );
    }
    else {
      drupal_set_message(t('Invitations to registered users cannot be withdrawn.'), 'error');
    }
  }
  else {
    watchdog('invite', 'Detected malicious attempt to delete an invitation.', array(), WATCHDOG_WARNING, l(t('view'), 'user/'. $user->uid));
    return drupal_access_denied();
  }

  drupal_goto("user/$user->uid/invites/$origin");
}

/**
 * Submit handler to delete an invitation.
 *
 * @see invite_cancel_form()
 */
function invite_cancel_form_submit($form, &$form_state) {
  $invite = $form_state['values']['invite'];

  db_query("UPDATE {invite} SET canceled = 1 WHERE code = '%s'", $invite->code);
  drupal_set_message(t('Invitation to %email has been withdrawn.', array('%email' => $invite->email)));

  // Notify other modules.
  invite_invoke_all($invite, 'cancel');
}

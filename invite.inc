<?php
// $Id$

/**
 * Base class for invites.
 */
class invite {
  /**
   * Fully loaded user object of the inviting user.
   */
  private $inviter;

  /**
   * Fully loaded user object of the invited user.
   */
  private $invitee;

  /**
   * The e-mail address of the invited user.
   */
  private $email;

  /**
   * The registration code used to identify an invitee upon registration.
   */
  private $code;

  /**
   * Creation timestamp.
   */
  private $created;

  /**
   * Expiry timestamp.
   */
  private $expiry;

  /**
   * Joined timestamp.
   */
  private $joined;

  /**
   * Canceled timestamp.
   */
  private $canceled;

  /**
   * How many times the invitation has been (re-)sent.
   */
  private $sent = 0;

  /**
   * Arbitrary additional data stored with the invitation. By default, this
   * includes the invitation subject and message text.
   */
  private $data = array(
    'subject' => NULL,
    'message' => NULL,
  );

  /**
   * Constructor.
   */
  function __construct(array $args = array()) {
    // Load user objects.
    if (isset($args['uid'])) {
      $this->inviter = user_load($args['uid']);
      unset($args['uid']);
    }
    if (isset($args['invitee']) && is_numeric($args['invitee'])) {
      $this->invitee = user_load($args['invitee']);
      unset($args['invitee']);
    }

    // Prepare the data storage array.
    if (isset($args['data'])) {
      if (is_string($args['data'])) {
        $args['data'] = unserialize($args['data']);
      }
      if (is_array($args['data'])) {
        $this->data = $args['data'] + array(
          'subject' => NULL,
          'message' => NULL,
        );
      }
      unset($args['data']);
    }

    // Assign properties.
    foreach ($args as $name => $value) {
      if (isset($this->$name)) {
        $this->$name = $value;
        unset($args[$name]);
      }
    }

    // Add remaining properties to the data storage.
    unset($args['form_token'], $args['submit'], $args['op'], $args['form_id'], $args['destination']);
    foreach ($args as $name => $value) {
      if ($value !== NULL) {
        $this->data[$name] = $value;
      }
    }
  }

  /**
   * Getter.
   */
  function __get($name) {
    if (!isset($this->$name)) {
      switch ($name) {
        case 'inviter':
          // Set current user as inviter. Call user_load() to have a fully
          // loaded user object available for token generation.
          $this->inviter = user_load($GLOBALS['user']->uid);
          break;

        case 'code':
          // Return a dummy code for token replacement. send() invokes
          // generate_code() just before sending to generate the 'real' code.
          return '--invite-code--';

        case 'created':
          $this->created = time();
          break;

        case 'expiry':
          $this->expiry = time() + variable_get('invite_expiry', 30) * 86400;
          break;

        default:
          return NULL;
      }
    }
    return $this->$name;
  }

  /**
   * Generate a unique invite code to track registrations.
   *
   * @return
   *   A unique tracking code.
   */
  function generate_code($length = 3) {
    // Start with three characters in length and the same count of tries
    // to find an unused code. Increment length until a unqiue code has been
    // found.
    $i = $length;
    do {
      $this->code = user_password($length);
      if (!db_result(db_query("SELECT 1 FROM {invite} WHERE code = '%s'", $this->code))) {
        break;
      }
      if (--$i == 0) {
        // Increment length and restart iterations.
        $i = ++$length;
      }
    } while (1);

    return $this->code;
  }

  /**
   * Send an invitation.
   *
   * @return
   *   Result code from drupal_mail().
   */
  function send($from = NULL) {
    global $user, $language;

    if (!isset($from)) {
      // Set From to either the inviting user's e-mail, or a manually
      // configured address.
      $from = (variable_get('invite_use_users_email', FALSE) && $user->uid) ? $user->mail : variable_get('invite_manual_from', '');
    }
    if (!valid_email_address($from)) {
      // Fall back to site default.
      $from = NULL;
    }

    // Generate a unique invite code. The code already exists when resending
    // an invite.
    if (!isset($this->code)) {
      $this->generate_code();
    }

    // Generate and send the e-mail.
    // @see invite_mail()
    $mail = drupal_mail('invite', 'invite', $this->email, $language, array('invite' => $this), $from);
    if ($mail['result']) {
      // Increment sent count and save the invite.
      $this->sent++;
      $this->save();

      // Notify other modules of the sent invitation.
      invite_invoke_all($this, 'invite');
    }

    return $mail['result'];
  }

  /**
   * Save an invite to the database.
   */
  function save() {
    if ($this->sent > 1) {
      db_query("UPDATE {invite} SET expiry = %d, sent = %d, data = '%s' WHERE code = '%s' AND uid = %d", $this->expiry, $this->sent, serialize($this->data), $this->code, $this->inviter->uid);
    }
    else {
      db_query("INSERT INTO {invite} (code, email, uid, created, expiry, data) VALUES ('%s', '%s', %d, %d, %d, '%s')", $this->code, $this->email, $this->inviter->uid, $this->created, $this->expiry, serialize($this->data));
    }
  }

  /**
   * Create e-mail message subject.
   */
  function get_message_subject() {
    // FIXME Translating user-entered text is considered bad behavior.
    $subject = t(variable_get('invite_subject', t('[inviter-raw] has sent you an invite!')));
    return token_replace_multiple($subject, $this->get_token_objects());
  }

  /**
   * Create e-mail message body.
   *
   * @param $autop
   *   Whether to run the template through filter_autop().
   */
  function get_message_body($autop = FALSE) {
    // FIXME Translating user-entered text is considered bad behavior.
    $body = t(invite_get_mail_template());
    return token_replace_multiple($autop ? _filter_autop($body) : $body, $this->get_token_objects());
  }

  /**
   * Provide token objects for use in message replacements.
   *
   * @return
   *   Array of token types suitable as input for token_replace() or
   *   token_replace_multiple().
   */
  function get_token_objects() {
    module_load_include('inc', 'invite', 'invite.token');
    return array('user' => $this->inviter, 'invite' => $this);
  }
}

/**
 * Return the invite e-mail subject.
 *
 * @param $substitutions
 *   Associative array of placeholders for token replacement.
 * @return
 *   The e-mail subject.
 */
function invite_get_mail_subject($placeholders = array()) {
  $subject = variable_get('invite_subject', NULL);
  if (!isset($subject)) {
    $subject = t('[inviter-raw] has sent you an invite!');
  }
  $invite = new invite($placeholders);
  // FIXME Translating user-entered text is considered bad behavior.
  return token_replace_multiple(t($subject), $invite->get_token_objects());
}

function invite_get_mail_template() {
  $mail_template = variable_get('invite_default_mail_template', NULL);
  if (!isset($mail_template)) {
    $mail_template = t("Your friend, [inviter-raw], has invited you to join [site-name] at [site-url].

To become a member of [site-name], click the link below or paste it into the address bar of your browser.

[join-link]

----------
[invite-message-raw]");
  }
  return $mail_template;
}

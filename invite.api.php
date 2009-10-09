<?php
// $Id$

/**
 * @file
 * Hooks provided by the Invite module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Act on invitation events.
 *
 * @param $op
 *   What kind of action is being performed. Possible values:
 *   - "invite": the invitation has just been sent.
 *   - "escalate": the invitation has been accepted and the user has just
 *     registered at the site.
 *   - "cancel": the invitation has been canceled.
 * @param &$roles
 *   - For "escalate", passes in the target roles the new user will receive.
 */
function hook_invite($op, $invite, &$roles = NULL) {
  switch ($op) {
    case 'invite':
      // Act on newly sent invite. If sent > 1, the invite has been resent.
      if ($invite->sent == 1) {
        // TODO
      }
      break;
    case 'escalate':
      // May alter the target roles before they are assigned to the user.
      $roles[123] = 123;
      break;
    case 'cancel':
      if ($invite->invitee) {
        drupal_set_message(t('Canceled invitation to: %name', array('%name' => $invite->invitee->name)));
      }
      break;
  }
}

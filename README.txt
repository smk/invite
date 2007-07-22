/* $Id: README.txt,v 1.3.2.4 2007-07-22 20:55:18 smk Exp $ */

SYNOPSIS
--------

Blogs are more fun when you have an audience. People who share common interests, who can commiserate with you and celebrate with you. Folks that leave comments, and have blogs of their own for you to read.

The invite.module provides an 'Invite a friend' feature. It allows your users to send and track invitations to join your site, and notifies them when their friends have joined so they can add them to their buddylist. Registrations successfully completed in this way will automatically be set active, and can be escalated to a new role as designated by the administrator.

This module can also be used to allow new user registrations "by invitation only", which allows you to maintain a semi-private site.

GENERAL USE
-----------

To invite a friend :

  1. Click on the 'Invite your friends and colleages' link.
  3. At the bottom of the Invite page, input the e-mail address(es) of the
     person(s) you would like to invite, and add a personal message.
  4. Press Submit
  5. This will send an invitation which you can now track from your invite
     page.

Invitations show up with one of three statuses : Pending, Joined and Expired.

- Pending: The invitation has been sent, but your friend has still not accepted
  the invitation. You may send a reminder, by clicking the 'Remind Me' link.

- Joined: Shows that the person you have invited has used your invitation to
  join your site. Click on the e-mail address to take you to their profile page
  where you can add them to your buddylist.

- Expired: The invitation has not been used to register within the expiration
  period (default: 30 days)

At any time, you may delete either 'pending' or 'expired' invitations. 'Joined' invitations cannot be deleted and count permanently toward your invitation allotment (this behavior can be altered by changing the configuration settings below).

INVITE API
----------

Several third-party modules are able to react on invite's events:

  - Buddylist
    When an invitee registers he/she will be automagically added to each others
    buddy list.

  - Userpoints
    Credit some points for sending registrations and/or when an invited user
    registers.

TWEAKS
------

When the site is set to allow new accounts by invitation only, it would be nice to remove the 'Create new account' tab that shows up if a user clicks on the 'Request a new password' link.

To solve this issue, you could add the following lines to your template.php:

function phptemplate_menu_item_link($item, $link_item) {
  if ($item['title'] == t('Create new account') && $item['path'] == 'user/register') return;
  return l($item['title'], $link_item['path'], !empty($item['description']) ? array('title' => $item['description']) : array(), isset($item['query']) ? $item['query'] : NULL);
}

This prevents the 'Create new account' menu item from being rendered.

CONTACT
-------

For bug reports, feature suggestions and latest developments visit the project page: http://drupal.org/project/invite

Project maintainer:
Stefan Kudwien (smk) <smk@unleashedmind.com>

Original Author:
David Hill a.k.a. Tatonca  <tatonca_@hotmail.com>


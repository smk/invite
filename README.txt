/* $Id: README.txt,v 1.4.2.4 2007-08-29 10:50:07 smk Exp $ */

SUMMARY

Invitations are important to create network effects and exponential growth of a 
community of interest. This module adds an 'Invite a friend' feature that 
allows your users to send and track invitations to join your site.

Users are able to customize the message text and subject of the e-mail, which 
is then sent either on their behalf or in the name of the site. Administrators 
are given options to escalate invited users to a specific role and limit the 
total number of invitations a user can send. This module can also be used to 
allow new user registrations 'by invitation only', which allows you to maintain 
a semi-private site.

Also available is a quick invite block, as well as a block listing the top 
inviters of the site. More statistics are available on the user's profile page, 
the block will display the absolute rank among the other users in this case.


PREREQUISITES

Invite requires Token v1.8 (or newer) http://drupal.org/project/token.


INSTALLATION

* Copy the invite module to your modules directory and enable it on the Modules
  page (admin/build/modules).

* Give some roles permission to send invites at the Access control page
  (admin/user/access).

* Optionally configure the registration settings at the User settings page
  (admin/user/settings). Invite adds a new mode to the existing settings called
  'New user registration by invitation only', which allows you to maintain a
  semi-private site.

* Configure the module at User management > Invite settings (admin/user/invite).


CONFIGURATION

--- User settings ---

* Target roles
  Allows to specify the role invited users will be added to when they
  register, depending on the role of the inviting user. The default is
  'authenticated user'.
* Invitation expiry
  Specify how long sent invitations are valid (in days). After an invitation
  expires the registration link becomes invalid.
* Allow deletion of joined users
  Whether to allow your users to delete (ie. revoke) invitations that are still
  in the 'pending' state.

--- Role limitations ---

* Limit
  Allows to limit the total number of invitations each role can send.

--- Multiple invitations ---

* Limit per turn
  Allows to limit the number of e-mail addresses a role can invite per turn.

--- E-mail settings ---

* Subject
  The default subject of the invitation e-mail.
* Editable subject
  Whether the user should be able to customize the subject.
* Mail template
  The e-mail body.
* From e-mail address
  Choose whether to send the e-mail on behalf of the user or in the name of the
  site.
* Manually override From/Reply-To e-mail address (Advanced settings)
  Allows to override the sender and reply-to addresses used in all e-mails.
  Make sure the domain matches that of your SMTP server, or your e-mails will
  likely be marked as spam.

--- Invite page customization ---

* Invite page title
  The title of the page (and the menu item) where users invite friends.
* Invite help text
  Allows to customize the introductory text that appears above the invite form.


GENERAL USE

To invite a friend :

1. Click the 'Invite your friends and colleages' link.
3. Fill in the e-mail address(es) of the person(s) you would like to invite,
   and add a personal message.
4. Press submit.
5. This will send an invitation e-mail which you can now track from the
   'Your invitations' page.

Invitations show up with one of three statuses: joined, pending and expired.

* Joined: Shows that the person you have invited has accepted the invitation to
  join the site. Click on the e-mail address to watch the user's profile page.
* Pending: The invitation has been sent, but the invitee has still not accepted
  the invitation.
* Expired: The invitation has not been used to register within the expiration
  period.

At any time, you may delete either 'pending' or 'expired' invitations. 'Joined' 
invitations can only be deleted if the configuration allows it to.


INVITE API

Several third-party modules can act on invite events:

* Buddylist http://drupal.org/project/buddylist and
  User Relationships http://drupal.org/project/user_relationships
  Inviter and invitee are automagically put on their respective buddy list.

* Userpoints http://drupal.org/project/userpoints
  Credit some points for sending registrations and/or when an invited user
  registers.


TWEAKS

When the site is set to allow new accounts by invitation only, it would be nice 
to remove the 'Create new account' tab that shows up if a user clicks on the 
'Request a new password' link. There seems no way to remove existing menu 
entries.

To solve this issue, you could add the following function to your template.php:

function phptemplate_menu_item_link($item, $link_item) {
  if ($link_item['path'] == 'user/register') return;
  return l($item['title'], $link_item['path'], !empty($item['description']) ? array('title' => $item['description']) : array(), isset($item['query']) ? $item['query'] : NULL);
}

This prevents the 'Create new account' menu item from being rendered.


CONTACT

For bug reports, feature suggestions and latest developments visit the project 
page: http://drupal.org/project/invite.

Project maintainer:
Stefan Kudwien (smk) <smk@unleashedmind.com>

Original Author:
David Hill a.k.a. Tatonca  <tatonca_@hotmail.com>


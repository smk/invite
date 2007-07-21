/* $Id: README.txt,v 1.4.2.2 2007-07-21 00:13:42 smk Exp $ */

INTRO
-----

Blogs are more fun when you have an audience. People who share common interests, who can commiserate with you and celebrate with you. Folks that leave comments, and have blogs of thier own for you to read.

The invite.module provides an 'Invite a friend' feature. It allows your users to send and track invitations to join your site, and notifies them when thier friends have joined so they can add them to thier buddylist. A random code is generated for the invitation, along with a link to the registration form.  The new user can then input the code when submiting thier registration.  Registration successfully completed in this way will automatically be set active, and escalated to a new role as designated by the administrator.

This module is meant to be used when user registration settings have been set to: 

   'Visitors can create accounts but administrator approval is required.' 

GENERAL USE
-----------

To invite a friend :

   1. Go to account information by clicking on My Account in the Navigation menu.
   2. In your Buddies section, click on the 'Invite a Friend' link.
   3. At the bottom of the Invite page, input the email address of the person you would like to invite.
   4. Press Submit
   5. This will send an invitation which you can now track from your invite page.

Invitations show up with one of three statuses : Joined, Pending and Expired.

Joined : Shows that the person you have invited has used your invitation to join blogtown. Click on the email address to take you to thier profile page where you can add them to your buddylist

Pending : The invitation has been sent, but your friend has still not accepted the invitation. You may send a reminder, by clicking the 'Remind Me' link.

Expired: The invitation has not been used to register on blogtown within the expiration period (30 days)

At any time, you may delete either 'pending' or 'expired' invitations. 'Joined' invitation cannot be deleted and count permanently toward your invitation allotment.

SETTINGS
--------

Settings:  Target Role: the role invited members will be added to

           Include Features: if the feature module is installed, this will include
                              the feature.module output in the email, stripped of HTML

           Invitation Expiry: time, in days, for how long an invitation can stay valid

           Number of invitations per user: how many invites total can each user send

           Sample Email : not really a feature, this shows an example of what the email 
                           will look like

MISC
----

The module makes use of the features.module if available, and will allow you to add your features output to the email.  

I created the module with buddylist in mind, however is not dependant on buddylist in anyway.  The link will display in the same section as buddylist in the users account view, or alone in the same section if buddylist is not installed.

It occured to me pretty early on that you could use this in conjunction with automember.module, to allow the users that acheive the higher role level when using that module, the ability to invite new people to the site, and new invitees to be relagated to a lower role, making for a kinda of self perpetuating ecosystem... 

TWEAKS
------

When the site is set to allow new accounts by invitation only, it would be nice to remove the Create a new account tab that shows up if a user clicks on the Request a new password link.

To solve this issue, you could add the following lines to your template.php:

function phptemplate_menu_item_link($item, $link_item) {
  if ($item['title'] == t('Create new account') && $item['path'] == 'user/register') return;
  return l($item['title'], $link_item['path'], !empty($item['description']) ? array('title' => $item['description']) : array(), isset($item['query']) ? $item['query'] : NULL);
}

This prevents the "Create new account" menu item from being rendered.

CONTACT
-------

Project page: http://drupal.org/project/invite

Original Author:
David Hill a.k.a. Tatonca  <tatonca_@hotmail.com>



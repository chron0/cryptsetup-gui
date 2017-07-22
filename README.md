cryptsetup-gui is a fairly simple application for unlocking and mounting LUKS
paritions with a GUI. The intended use is for mounting an encrypted home
partition after login, but it can be used for any encrypted partition.

It is invoked with the name of an encrypted partition, looks up that name in
/etc/crypttab to find the device, prompts the user for a password, decrypts the
device and mounts it according to /etc/fstab.

Mounting /home on login
-----------------------

In order to enable the mounting of /home AFTER a user has logged in, we employ a
little trick:

  1. The user's home directory is still set to /home/#username#.
  2. We create this directory when /home is unmounted
  3. We add a .xinitrc file that calls cryptsetup-gui, mounts /home on top of
     us, and then executes the "real" user .xinitrc

The included xinitrc file should take care of this, and is installed into
/etc/skel. It defaults to the name of the encrypted home partition to be
*chome*, but this can be changed by modifying the file after copying it to your
unmounted home directory.

Security Implications
---------------------

**Note:** The binary cryptsetup-gui installed into /usr/bin has the setuid
flag enabled and is owned by root. This is so that it can be invoked by users
and still be permitted to unlock a device. This is done on your own risk!

The application only accepts a single parameter, the name of the encrypted
device, and this is enforced to be only a-z. Thus, I do not believe there to be
any risk of that causing a security risk with setuid. The file does execute two
shell commands using cryptsetup and luks, however the parameters for these are
read from /etc/crypttab and /etc/fstab. Thus, **should a malicious user be able
to modify either of these files, he/she will be able to use cryptsetup-gui to
run arbitrary commands as root**. That said, if they can edit those files, you
already have a security breach.

License
-------

Copyright (C) 2016 chrono
Copyright (C) 2012 Jon Gjengset

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

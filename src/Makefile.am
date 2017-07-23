bin_PROGRAMS = cryptsetup-gui cryptsetup-gui-gtk

cryptsetup_gui_SOURCES = cryptsetup-gui.c

if DEBUG
  cryptsetup_gui_CFLAGS = -D_GNU_SOURCE -Wall -O2 -g -DDEBUG
else
  cryptsetup_gui_CFLAGS = -D_GNU_SOURCE -Wall -O2 -DNDEBUG
endif



cryptsetup_gui_gtk_SOURCES = cryptsetup-gui-gtk.c
cryptsetup_gui_gtk_CFLAGS = -D_GNU_SOURCE -Wall -O2 $(GTK_CFLAGS)
cryptsetup_gui_gtk_LDADD = $(GTK_LIBS)

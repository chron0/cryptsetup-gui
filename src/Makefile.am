bin_PROGRAMS = cryptsetup-gui cryptsetup-gui-gtk

cryptsetup_gui_SOURCES = cryptsetup-gui.c

if DEBUG
  cryptsetup_gui_CFLAGS = $CFLAGS -DDEBUG -g
else
  cryptsetup_gui_CFLAGS = $CFLAGS -s
endif

cryptsetup_gui_gtk_SOURCES = cryptsetup-gui-gtk.c
cryptsetup_gui_gtk_CFLAGS = $(GTK_CFLAGS)
cryptsetup_gui_gtk_LDADD = $(GTK_LIBS)

# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6
inherit eutils git-r3

DESCRIPTION="Simple cryptsetup GUI for unlocking LUKS volumes during login"
HOMEPAGE="https://github.com/chron0/cryptsetup-gui"
EGIT_REPO_URI="https://github.com/chron0/cryptsetup-gui.git"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="amd64 x86"
IUSE="debug"

DEPEND=">=x11-libs/gtk+:2"
RDEPEND="${DEPEND}
    sys-fs/cryptsetup"

src_configure() {
    econf $(use_debug debug) 
}

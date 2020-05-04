DESCRIPTION = "Library to play files in enigma2 using ffmpeg"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING.GPLv2;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = "ffmpeg ${@bb.utils.contains('TARGET_ARCH', 'sh4', 'sh4-dvb-modules', '', d)}"
RDEPENDS_${PN} = "ffmpeg"
RPROVIDES_${PN} += "libeplayer"

CXXFLAGS_append += " -std=c++11"

inherit gitpkgv autotools

SRCREV = "${AUTOREV}"

PV = "git${SRCPV}"
PKGV = "git${GITPKGV}"
PKG_${PN} = "${PN}"

SRC_URI = "git://github.com/OpenVisionE2/libeplayer3.git"

S = "${WORKDIR}/git"

do_install_append () {
	install -d ${D}${includedir}/libeplayer3
	install -m 644 ${S}/include/*.h ${D}${includedir}/libeplayer3
}

FILES_${PN}-dev += "${includedir}/libeplayer3"

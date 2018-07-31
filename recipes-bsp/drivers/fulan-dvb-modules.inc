SUMMARY = "Fulan driver modules from TDT"
DESCRIPTION = "Fulan driver modules from TDT"
HOMEPAGE = "http://github.com/Duckbox-Developers"
SECTION = "base"
PRIORITY = "required"

LICENSE = " GPLv2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYING;md5=751419260aa954499f7abaabaa882bbe"

RDEPENDS_${PN} = "stinit"

KV = "2.6.32.71-stm24-0217"
SRCREV = "${AUTOREV}"

inherit module

PACKAGES = "${PN} ${PN}-dev"

INC_PR = "r1"
PV = "${KV}+${SRCDATE}"

KERNEL_MODULE_PACKAGE_SUFFIX = ""
PTI_NP_PATH ?= "/data/pti_np"

FILESEXTRAPATHS_prepend := "${THISDIR}/fulan-dvb-modules:"

SRC_URI = " \
    git://github.com/PLi-metas/fulan-dvb-drivers.git;protocol=git \
    file://ddbootup \
    file://sh4booster \
    file://modules.conf \
    file://modules-conf.conf \
    file://RT2870STA.dat \
    file://vdstandby \
    file://COPYING \
" 

FILES_${PN} = "${sysconfdir}/init.d ${sysconfdir}/rcS.d ${sysconfdir}/modules-load.d ${sysconfdir}/modprobe.d ${sysconfdir}/Wireless/RT2870STA /bin"
FILES = ""

S = "${WORKDIR}/git"

EXTRA_OEMAKE = "-e MAKEFLAGS="

do_configure_prepend () {

    # if a custom pti source is present, add it to the sources
    if [ -e ${PTI_NP_PATH}/Makefile ]; then
        echo "Found custom pti sources.."
        cp -r ${PTI_NP_PATH} ${S}
    fi

    if [ -L include/stmfb ]; then
        rm include/stmfb
    fi
    if [ -L include/player2 ]; then
        rm include/player2
    fi
    if [ -L include/multicom ]; then
        rm include/multicom
    fi
    if [ -L player2 ]; then
        rm player2
    fi
    if [ -L stgfb/stmfb ]; then
        rm stgfb/stmfb
    fi
    if [ -L multicom ]; then
        rm multicom 
    fi
    ln -sf ${S}/include/stmfb-3.1_stm24_0104 ${S}/include/stmfb
    ln -sf ${S}/include/player2_191 ${S}/include/player2
    ln -sf ${S}/multicom-3.2.4/include ${S}/include/multicom
    ln -sf ${S}/player2_191 ${S}/player2
    ln -sf ${S}/multicom-3.2.4 ${S}/multicom
    ln -sf ${S}/stgfb/stmfb-3.1_stm24_0104 ${S}/stgfb/stmfb
    rm -f .config 
    printf "export CONFIG_PLAYER_191=y\nexport CONFIG_MULTICOM324=y\n" > .config
    export KERNEL_LOCATION="${STAGING_KERNEL_DIR}"
    cp -R ${S}/multicom-3.2.4 ${STAGING_KERNEL_DIR}/multicom
}

do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    oe_runmake KERNEL_PATH=${STAGING_KERNEL_DIR}   \
        KERNEL_SRC=${STAGING_KERNEL_DIR}    \
        KERNEL_VERSION=${KERNEL_VERSION}    \
        -C ${STAGING_KERNEL_DIR}   \
	O=${STAGING_KERNEL_BUILDDIR} \
        ${@d.getVar('MACHINE',1).upper()}=1 \
        M=${S} V=1 \
        ARCH=sh \
        PLAYER191=player191 \
        DRIVER_TOPDIR="${S}" \
        KERNEL_LOCATION="${STAGING_KERNEL_DIR}" \
        CONFIG_KERNEL_BUILD="${STAGING_KERNEL_BUILDDIR}" \
        CONFIG_KERNEL_PATH="${STAGING_KERNEL_DIR}" \
        CONFIG_MODULES_PATH="${D}" \
        CONFIG_PLAYER_191=y \
        CCFLAGSY="-I${STAGING_DIR_HOST}/usr/include" \
        modules 
}

do_install() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    oe_runmake KERNEL_PATH=${STAGING_KERNEL_DIR}   \
        KERNEL_SRC=${STAGING_KERNEL_DIR}    \
        KERNEL_VERSION=${KERNEL_VERSION}    \
        -C ${STAGING_KERNEL_DIR}   \
	O=${STAGING_KERNEL_BUILDDIR} \
        ${@d.getVar('MACHINE',1).upper()}=1 \
        M=${S} V=1 \
        ARCH=sh \
        PLAYER191=player191 \
        DRIVER_TOPDIR="${S}" \
        KERNEL_LOCATION="${STAGING_KERNEL_DIR}" \
        CONFIG_KERNEL_BUILD="${STAGING_KERNEL_BUILDDIR}" \
        CONFIG_KERNEL_PATH="${STAGING_KERNEL_DIR}" \
        CONFIG_MODULES_PATH="${D}" \
        CONFIG_PLAYER_191=y \
        CCFLAGSY="-I${STAGING_DIR_HOST}/usr/include" \
        INSTALL_MOD_PATH=${D} modules_install

    # install header files
    install -d ${D}/${includedir}/
    install -m 644 multicom/include/mme.h ${D}/${includedir}
    install -m 644 include/player2/JPEG_VideoTransformerTypes.h ${D}/${includedir}
    install -m 644 include/player2/JPEG_TransformerTypes.h ${D}/${includedir}
    install -m 644 include/player2/JPEGDECHW_VideoTransformerTypes.h ${D}/${includedir}
    install -m 644 include/player2/PNGDecode_interface.h ${D}/${includedir}
    install -m 644 include/player2/RLEDecode_interface.h ${D}/${includedir}
    install -m 644 frontcontroller/aotom/aotom_main.h ${D}/${includedir}
    install -m 644 player2/components/include/stddefs.h ${D}/${includedir}

    #install modutils config
    install -d ${D}/${sysconfdir}/modules-load.d
    install -m 644 ${WORKDIR}/modules.conf ${D}/${sysconfdir}/modules-load.d/_${MACHINE}.conf
    install -d ${D}/${sysconfdir}/modprobe.d
    install -m 644 ${WORKDIR}/modules-conf.conf ${D}/${sysconfdir}/modprobe.d/_${MACHINE}.conf
    install -d ${D}/${sysconfdir}/init.d
    install -d ${D}/${sysconfdir}/rcS.d
    install -m 0755 ${WORKDIR}/ddbootup ${D}${sysconfdir}/init.d
    ln -sf ../init.d/ddbootup ${D}${sysconfdir}/rcS.d/S04ddbootup
    install -d ${D}/bin
    install -m 755 ${WORKDIR}/vdstandby ${D}/bin
    install -d ${D}${sysconfdir}/Wireless
    install -d ${D}${sysconfdir}/Wireless/RT2870STA
    install -m 0644 ${WORKDIR}/RT2870STA.dat ${D}/${sysconfdir}/Wireless/RT2870STA
    install -m 0755 ${WORKDIR}/sh4booster ${D}${sysconfdir}/init.d
    ln -sf ../init.d/sh4booster ${D}${sysconfdir}/rcS.d/S05sh4booster
    
    # if no pti_np sources are available and a custom pti.ko is present, overwrite the tdt one
    if [ ! -e ${PTI_NP_PATH}/Makefile ]; then
        if [ -e ${PTI_NP_PATH}/pti.ko ]; then
            echo "Found custom pti binary.." 
            install -m 644 ${PTI_NP_PATH}/pti.ko ${D}/lib/modules/${KERNEL_VERSION}/extra/pti/pti.ko
        fi
    fi

    find ${D} -name stmcore-display-sti7106.ko | xargs -r rm # we don't have a 7106 chip
}

PACKAGESPLITFUNCS_append = " handle_driver_rename "

python handle_driver_rename () {
    d.setVar("RPROVIDES_fulan-dvb-modules-${MACHINE}", "fulan-dvb-modules")
    d.setVar("RREPLACES_fulan-dvb-modules-${MACHINE}", "fulan-dvb-modules")
    d.setVar("RCONFLICTS_fulan-dvb-modules-${MACHINE}", "fulan-dvb-modules")
}

FILES_${PN}-dev += "${includedir}"

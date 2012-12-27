PRODUCT_PROPERTY_OVERRIDES += \
       dalvik.vm.heapstartsize=5m \
       dalvik.vm.heapgrowthlimit=36m \
       dalvik.vm.heapsize=128m

$(call inherit-product, device/qcom/common/common.mk)

#LogSystem
LogSystem := LogKit
LogSystem += SystemAgent
LogSystem += qlogd
LogSystem += qlog-conf.xml
LogSystem += diag_mdlog

PRODUCT_PACKAGES += \
	com.invensense.android.hardware.xml \
	com.invensense.android.hardware \
	libgesture_manager_jni \
	libmpl_sys_jni \
	libmpl_ped_jni \
	libmllite \
	libmlplatform \
	libmplmpu \
	sensors.msm7627a \
	fat.img \
	fatbak.img \
	fatimgcheck \
	vold.emmc.fstab \
	vold.origin.fstab \
	vold.fat.fstab \
	akmdfs \
	FastBoot \
	RestoreAirplaneMode \
        hostapd_cli \
        hostapd \
        libecc \
        libiwnwai_asue \
        libsms4 \
        wpa_cli \
        wpa_supplicant \
        wpa_supplicant.conf \
        libwpa_client \
        abtfilt \
        fw-3 \
        bdata \
        athtcmd_ram \
        nullTestFlow \
        utf \
	cfg80211.ko\
	ath6kl_sdio.ko \
	athtestcmd \
	iw \
	iwconfig \
	iwlist \
	wlan_test.sh \
    fm_qsoc_patches \
	perfman_init \
	memopt_init


PRODUCT_PACKAGES += silent_profile
PRODUCT_PACKAGES += rmt_storage_recovery
PRODUCT_PACKAGES += nv_set
PRODUCT_PACKAGES += smlogserver

PRODUCT_PACKAGES += $(LogSystem)

PRODUCT_NAME := msm7627a
PRODUCT_DEVICE := msm7627a

#Bluetooth configuration files
PRODUCT_COPY_FILES += \
   system/bluetooth/data/main.le.conf:system/etc/bluetooth/main.conf \
   $(LOCAL_PATH)/persist.prop:$(PRODUCT_OUT)/persist/persist.prop

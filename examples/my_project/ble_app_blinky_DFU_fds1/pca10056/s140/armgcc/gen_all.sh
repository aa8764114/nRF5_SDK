#創建資料夾_all
mkdir _all

#將app複製到_all
cp ./_build/nrf52840_xxaa.hex ./_all/app.hex

#將bootloader複製到_all
cp /Users/motosawa/Documents/nRF5_SDK_17.1.0_ddde560/examples/dfu/secure_bootloader/pca10056_s140_ble/armgcc/_build/nrf52840_xxaa_s140.hex ./_all/bootloader.hex

#用bootloader在_all資料夾中產生bl-settings
nrfutil settings generate --family NRF52840 --application ./_all/app.hex  --application-version 3 --bootloader-version 2 --bl-settings-version 1 ./_all/bl-settings.hex

#把softdevice複製到_all
cp /Users/motosawa/Documents/nRF5_SDK_17.1.0_ddde560/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex ./_all/softdevice.hex

#將以上_all內的.hex檔案合併成all.hex
mergehex --merge ./_all/app.hex ./_all/bl-settings.hex ./_all/bootloader.hex ./_all/softdevice.hex --output ./_all/all.hex
#mergehex --merge ./_all/app.hex ./_all/bootloader.hex ./_all/softdevice.hex --output ./_all/all.hex


#清除開發版內的資料
nrfjprog -f nrf52 --eraseall

#刷入all.hex
nrfjprog -f nrf52 --program ./_all/all.hex --sectorerase


nrfjprog -f nrf52 --reset

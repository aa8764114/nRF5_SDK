#建立_zip資料夾
mkdir _zip

#將private.pem移入_zip
cp /Users/motosawa/Documents/nordic_key/private.pem ./_zip

#複製nrf52840_xxaa.hex到_zip中並改叫update.hex
cp ./_build/nrf52840_xxaa.hex ./_zip/update.hex

#用private.pem和update.hex產生update.zip
nrfutil pkg generate --application ./_zip/update.hex --application-version-string "1.1.0" --hw-version 52 --sd-req 0x100 --key-file ./_zip/private.pem ./_zip/update.zip

#將zip複製到桌面
cp ./_zip/update.zip /Users/motosawa/Desktop



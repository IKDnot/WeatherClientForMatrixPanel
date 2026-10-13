# ローカル秘密設定

Wi-Fi SSID／パスワード、天気APIキー、都市名は `include/DeviceSecrets.h` に設定します。このファイルは `.gitignore` 対象で、ソース管理へ追加されません。

初回セットアップでは、同じディレクトリにある `DeviceSecrets.example.h` を `DeviceSecrets.h` として複製し、各項目を実際の値に置き換えてください。

この分離は秘密情報の誤コミットを防ぐためのものです。ファームウェアを配布する場合、認証情報はバイナリから取り出される可能性があるため、デバイスの保護やESP32のSecure Boot／Flash Encryptionも検討してください。以前に公開・共有された認証情報は必要に応じて更新してください。

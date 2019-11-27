# Morphy Planning USB-IO on PIC16F1459

# Pinout

必要なピンは以下の通り：

- VDD/VUSB/VSS
- D+/D-
- MCLR/ICSPDAT/ISCPCLK
- RX/TX
- Other 10pins

ICSPDAT/ICSPCLKは共用にすれば12端子が使えるので、
これで8+4構成が作れるのでUSB-IO完全互換にできそう。

ICSPピンの設定はリセット直後ではなく少し遅らせる方がいいという話があったが、
ならばピン制御命令が来たら始めて出力ピンに設定するようにすればいいか。



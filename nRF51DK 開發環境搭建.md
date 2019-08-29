# nRF51DK 開發環境搭建
###### tags: `Intern`
## 必備軟體

### IDE
51推薦使用[Keil](https://blog.csdn.net/Mculover666/article/details/89469764)
52推薦使用[SES](https://www.segger.com/products/development-tools/embedded-studio/)

### [Command Line Tools](https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs)

包含 Jlink驅動、nrfjprog、mergehex等，將程式燒盡晶片用的軟體

安裝過程中不要更改任何項目，以下畫面選 select all
![Command Line Tools](https://i.imgur.com/jB4yJO2.png)

### [nRF5 SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs)
nRF51系列最新支援到SDK12.3.0

### SDK Plug In 安裝
使用 Keil 內建的 Keil pack Installer 即可

![Nordic Device Family Pack](https://i.imgur.com/mSrwcEL.png)
Nordic Device Family Pack
也就是 nRF MDK

![CMSIS](https://i.imgur.com/RGjG07F.png)
ARM CMSIS

**如果沒辦法使用 Pack Installer**

也可以單獨下載安裝
* **[CMSIS](https://github.com/ARM-software/CMSIS/releases)**

    隨便開個例子, Keil會確認缺少哪個版本
    nRF51DK會需要 4.5.0

* **[MDK](https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-MDK/Download#infotabs)**
    Keil 5 請下載 Pack, 3-clause BSD license

### [PuTTY](https://www.putty.org/)

用來看 serial 回傳的資料(UART的 TX, RX)
nRF51DK的設定, COM Port要設定成從裝置管理員看到的COM
![putty](https://i.imgur.com/38O3h2o.png)

### [SDK Documentation](http://developer.nordicsemi.com/nRF5_SDK/doc/)

線上的說明文件非常難用, 一定要用offline的

## Keil 使用教學

### 如何燒錄
![button](https://i.imgur.com/ulPRXW1.png)

編輯完C後, 按下 Build *左2* 編譯成 Hex 檔案

只有動到當下開啟的檔案, 用 Rebuild *左3* 就夠了

之後按下旁邊的 Download *右1* 就能燒錄進 MCU 中

### 如何開啟新專案
千萬別從空白的專案開始, 太花時間了

1. 從example中找到比較接近自己要使用的功能, 複製到同一層級的資料夾(ex. SDK\XXX\Projects\)

2. 在Project欄位中 nrf5xxx_xxac上右鍵點出 Options for Target...
![options](https://i.imgur.com/dOqWVcg.png)
3. 選擇 C/C++ 項目, 並在 include paths 中新增對應Library 的路徑
![paths](https://i.imgur.com/WXK27VM.png)
4. 在Project欄位中 對應的位置 加入對應的 Library
![add](https://i.imgur.com/RKOEBS2.png)
5. 在 sdk_config.h 中加入對應的 Enable
![config](https://i.imgur.com/fHCJIkM.png)

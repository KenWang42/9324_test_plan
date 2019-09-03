# SX9324 測試程式說明

IDE: Keil uvision 5

專案路徑: 
```.\projects\KANGYU\SX9324_Test_Plan\pca10028\blank\arm5_no_packs\uart_pca10028.uvprojx```

在 project 資料夾內已經放需要參考的官方範例, 如果要更詳細的範例請至以下路徑

```.\nRF5_SDK_12.3.0_d7731ad\examples\peripheral```

離線官方說明文件
```.\nRF5_SDK_12.3.0_d7731ad\nRF5_SDK_12.3.0_offline_doc\index.html```


## 測試流程

上電後, 先別插入產品, 等待初始化完成後再插入產品並輸入 R 來開始測試

1. 初始化各大功能
2. 讀取 Phase1 及 Phase2 的數值
3. 測試 LED 燈泡

在任何時候輸入 r 或 R, 會重新開始測試流程

## nRF51422 功能說明

TWI: SX9324使用的通訊協定, 藉此讀寫 Register 內的值

UART: nRF51422 與電腦連接的通訊協定, 藉此可以從鍵盤輸入, 也可以從MCU輸出到電腦

GPIO: nRF51422 對腳位輸出輸入的銅訊協定, 藉此可以控制 LED 的行為

以上三種功能的使用方法詳見官方在SDK內提供的範例, 以及說明文件

## 函式說明

### 1. ``` void SX9324_read_Reg(int Reg) ```
功能:

    讀取指定的 Register 數值

Args:

    (int(hex) Reg: 需要讀取的 Register, 需以 16 進位輸入


### 2. ```bool SX9324_init(void) ```
功能:

    初始化SX9324

    1. 讀取 IrqSrc(0x00) 重置 NIRQ

    2. 在各別 Register 內寫入自訂的值(藉由SX9324EVK設定, 需要注意不能寫入 0x00)

    3. 在 RegStat2(0x03) 內寫入 0x0F, 完成補正

Returns:
    
    bool: true, 初始化成功

### 3. ```void SX9324_set_phase(int phase)```

功能:
    
    設定 SX9324 讀取的 Phase
    
Args:
    
    int(hex) phase: 指定的 Phase, 以16進位輸入
    
                    參考 datasheet P.43 RegPhaseSel

### 4. ```void uart_event_handler(app_uart_evt_t * p_event)```

功能:
    
    設定 UART 事件的中斷

    在此測試中, 輸入 r 或是 R 都會觸發中斷 (重啟程式)
    
    輸入 w 或是 W 會亮起白燈
    
    輸入 o 或是 O 會亮起橘燈

    必須在 void uart_init() 內宣告此事件處理器

### 5. ```void uart_init()```

功能:
    
    初始化 UART 功能, 詳見SDK內提供的範例及說明文件

### 6. ```void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)```

功能:

    設定 TWI 事件的中斷

    在此測試中, 會將讀取到的值以 2補數 的形式印出
    
    並存入 PSENSOR_DIFF_VALUE[] 中

    參考 Datasheet P.43 RegAvgMsb

### 7. ```void twi_init()```

功能:
    
    初始化 TWI 功能, 詳見SDK內提供的範例及說明文件

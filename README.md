# Desktop_Social_Robot
 以 stm32 平台製作，可放置於桌面並具有互動功能的雙輪機器人。
 
 ## 目前實現功能
 * PID 控制演算法調整左右馬達電壓平衡機器人
 * 藍芽遙控
 * 超音波模組偵測到障礙物後退
 * 電池電壓不足 / 傾角過大 關閉馬達

 ## 未來規劃
 ### 串接 Ｍaix Dock (k210) 開發板 
 * 取代藍芽控制機器人
 * 實現手勢識別互動功能
 * 實現聲音控制互動功能
 ### 進一步的硬體調整
 * SLAM 演算法實現自主桌面環境探索
 * 語音助理
 * 物聯網家電控制

 ## 開發板
 * stm32 F103C8T6
 * 整合 TB6612FNG 馬達驅動
 * 整合 MPU6050 陀螺儀 / 加速度計

#### # 部分由開發板廠商提供與 stm32 專案產生的檔案沒有上傳

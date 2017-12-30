/**
        @file   SerialDataLogger.h
        @brief  SerialDataLoggerアプリケーションにデータを送るクラス
        @author 谷川　豊章
        @date   2016/08/19
*/


#ifndef DEF_SERIALDATALOGGER
#define DEF_SERIALDATALOGGER

#include <Arduino.h>


/**     SerialDataLoggerクラス
        SerialDataLoggerアプリケーションにプロットデータをシリアルで送るクラス
*/
class SerialDataLogger {
    float* dataArray;   ///< 送るプロットデータ
    int dataArrayLength;    ///< プロットデータのサイズ

    SerialDataLogger(const SerialDataLogger&);
    SerialDataLogger& operator=(const SerialDataLogger&);

    void SendByte(byte data) {
      if (data == 'e' || data == 's' || data == 'p') {
        Serial.write('e');
        Serial.write(~data);
      } else
        Serial.write(data);
    }

  public:

    /**     コンストラクタ
            クラスの初期化
            @param  seriesNumber        [in]送る系統の数
            @param  sendInterval_ms     [in]データを送る周期。デフォルト値を使用すれば、推奨値がセットされる。
    */
    SerialDataLogger(int seriesNumber) {

      if (seriesNumber < 0)
        seriesNumber = 0;

      ++seriesNumber;

      //データの保存場所を確保する
      dataArray = new float[seriesNumber];
      dataArrayLength = seriesNumber;

      for (int i = 0; i < dataArrayLength; i++) {
        dataArray[i] = 0.0f;
      }
    }


    /**     デストラクタ
    */
    ~SerialDataLogger() {
      delete[] dataArray;
    }


    /**     プロットするX軸座標の値をセット
            @param  [in]プロットする点のX軸座標の値
    */
    void SetX(float x) {
      dataArray[0] = x;
    }


    /**     プロットするY軸座標の値をセット
            @param  [in]系統の添え字
            @param  [in]プロットする点のY軸座標の値
    */
    void SetSeriesY(int index, float y) {
      ++index;
      if (0 < index && index < dataArrayLength) {
        dataArray[index] = y;
      }
    }


    /**     データを送る
            @attention  このメソッドを呼び出す前にSerialを初期化しておくこと。
    */
    void Send() {
      //send StartByte
      Serial.write('s');

      //send main byte
      for (int i = 0; i < dataArrayLength * 4; i++) {
        SendByte( ((char*)dataArray)[i] );
      }

      //send StopByte
      Serial.write('p');
    }

};


#endif

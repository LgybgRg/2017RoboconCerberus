/**
 *      @file   EncoderBoard.h
 *      @brief  ロータリーエンコーダ基盤から回転数を取得するクラス
 *      @author 谷川　豊章
 *      @date   2016/8/17
 */
 

#ifndef DEF_ENCODERBOARD_H
#define DEF_ENCODERBOARD_H

#include <arduino.h>
#include <Wire.h>

/**     EncoderBoardクラス
 *      エンコーダ基盤から回転数を取得するクラス
 */
class EncoderBoard{
    uint8_t address;   ///< エンコーダ基盤のI2Cアドレス
    
    long nowCount;  ///< 現在の回転数
    long offsetCount;   ///< 回転数のオフセット

    EncoderBoard(const EncoderBoard&);
    EncoderBoard& operator=(const EncoderBoard&);

public:

    /**     コンストラクタ
     *      @param  address         [in]使用するエンコーダ基盤のI2Cアドレス
     */
    EncoderBoard(uint8_t address) : address(address), nowCount(0), offsetCount(0) { }

    /**     ディストラクタ
     */
    virtual ~EncoderBoard(){ }

    /**     エンコーダ基盤の初期化
     */
    void Init(){
        Update();
    }

    enum Pullup{ PULLUP_DISABLE, PULLUP_ENABLE };
    /**     プルアップの設定
     *      エンコーダの信号線をプルアップするかを設定する。(初期状態はプルアップあり)
     *      @attention  このメソッドを呼び出す前にI2Cをマスターで初期化しておくこと。
     *      @param  mode    プルアップをするかどうか
     *      @retval 0   success
     *      @retval 1   data too long to fit in transmit buffer
     *      @retval 2   received NACK on transmit of address
     *      @retval 3   received NACK on transmit of data
     *      @retval 4   other error
     */
    byte SetPullup(Pullup mode){
        Wire.beginTransmission(address);
        Wire.write(mode);
        return Wire.endTransmission();
    }

    /**     データの更新
     *      I2Cで現在の回転数を読み出す。
     *      @attention  このメソッドを呼び出す前にI2Cをマスターで初期化しておくこと。
     *      @retval 0   success
     *      @retval 1   data too long to fit in transmit buffer
     *      @retval 2   received NACK on transmit of address
     *      @retval 3   received NACK on transmit of data
     *      @retval 4   other error
     */
    virtual void Update(){
        Wire.requestFrom(address, (uint8_t)4);    // request 4 bytes from slave device #2
    
        //現在のカウントを更新
        if(Wire.available() > 3){
            for(int i=0; i<4; i++){
                ((char*)&nowCount)[i] = Wire.read();
            }
        }
    
        //不必要なデータを読み出す
        while(Wire.available()){ 
            Wire.read();
        }
    }

    /**     回転数の初期化
     *      現在の回転数を0にセットする。
     *      @attention  この関数を呼び出す前にInit()を呼び出しておくこと。
     */
    void ResetCount(){
        offsetCount = nowCount;
    }

    /**     回転数の取得
     *      @attention  この関数を呼び出す前にInit()を呼び出しておくこと。
     *      @return     現在の回転数
     */
    long GetCount(){
        return nowCount - offsetCount;
    }
    
};

extern EncoderBoard enc_x,enc_y;

#endif

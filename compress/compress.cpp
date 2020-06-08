// compress.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include "zipHeader.h"
#include "decode.h"
#include "WriteData.h"
#include <direct.h>
#include "encode.h"

int main()
{
    char infile[] = "C:/Users/Bee/Desktop/【1 縦売り 夏】在庫状況.xlsx"; //読み込むファイルの指定 【2020年1月11日更新】2020-発注到着
    char mfile[255] = "C:/Users/Bee/Desktop/test"; //テスト書き出し　26文字
    char foln[255] = "C:/Users/Bee/Desktop/test";
    //char stockfoln[255] = { 0 };
    int slash[5] = { 0 };
    int slashnum = 0;
     /*
    絶対パスで指定
    (『C:\\Users\\gagambo\\Documents』のパスは実在するが
    『テスト』というフォルダは存在しない)
    */
    if (_mkdir("C:\\Users\\Bee\\Desktop\\test") == 0) {
        printf("フォルダ作成に成功しました。\n");
    }
    else {
        printf("フォルダ作成に失敗しました。\n");
    }
    /*
    相対パスで指定
    (『folder』というフォルダは存在しない)
    */
    /*if (_mkdir("folder") == 0) {
        printf("フォルダ作成に成功しました。\n");
    }
    else {
        printf("フォルダ作成に失敗しました。\n");
    }*/
    /*『newfolder』というフォルダが存在しない場合失敗する(この場合存在しない)*/
    /*if (_mkdir("newfolder\\test") == 0) {
        printf("フォルダ作成に成功しました。\n");
    }
    else {
        printf("フォルダ作成に失敗しました。\n");
    }*/

    _Post_ _Notnull_ FILE* cdf;

    std::ifstream fr(infile, std::ios::in | std::ios::binary);

    UINT64 pos = 0;
    UINT32 readlength = 0;
    /*ヘッダーデータの読み込み　位置取得*/
    ReadZipHead* Hr = new ReadZipHead(&fr);
    
    while (!fr.eof()) {
        pos = Hr->readLocal(pos);

        std::cout << "読み込み位置：" << pos<< std::endl;
        if (pos) {
            if (Hr->LH) {
                if (Hr->LH->method == 8) {
                    if (Hr->LH->nonsize > 0) {
                        std::cout << "圧縮データ" << std::endl;

                        DeflateDecode* dec = new DeflateDecode(&fr);
                        pos = dec->dataread(pos, Hr->LH->nonsize);

                        /*
                        encoding* enc = new encoding;//圧縮 オブジェクト生成
                        enc->compress(dec->ReadV, dec->readlen);//圧縮
                        delete enc;//削除
                        */

                        std::cout << "データ長：" << dec->readlen << std::endl;
                        //デスクトップパス付け加え
                        int i = 0;
                        mfile[25] = '/';

                        while (i <= Hr->LH->filenameleng) {
                            mfile[26 + i] = Hr->LH->filename[i];
                            if (Hr->LH->filename[i] == '/') {
                                slash[slashnum] = i; slashnum++;
                            }
                            i++;
                        }

                        if (slashnum > 0) {
                            foln[25] = '/';

                            i = 0;
                            int j = 0;
                            while (j < slashnum) {
                                while (i < slash[j]) {
                                    foln[26 + i] = Hr->LH->filename[i];
                                    i++;
                                }
                                foln[26 + i] = '\0';
                                //フォルダー作成
                                if (_mkdir(foln) == 0) {
                                    printf("フォルダ作成に成功しました。\n");
                                }
                                else {
                                    printf("フォルダ作成に失敗しました。\n");
                                    std::cout << foln << std::endl;
                                }
                                j++;
                            }
                        }

                        std::cout << "ファイル名：" << mfile << std::endl;
                        if (dec->readlen > 0) {
                            //データ書き込み用

                            //_Post_ _Notnull_ FILE* cdf;
                            fopen_s(&cdf, mfile, "wb");
                            if (!cdf) {
                                printf("ファイルを開けませんでした");
                                return 0;
                            }

                            //ファイル書き込み
                            //DataWrite* fwrite = new DataWrite(cdf);

                            //fwrite->writedata(dec->ReadV, dec->readlen);
                            std::cout << "データ書き込み" << std::endl;
                            UINT32 i = 0;
                            while (i < dec->readlen) {
                                fwrite(&dec->ReadV[i], sizeof(char), 1, cdf);
                                i++;
                            }

                            //delete fwrite;

                            if (cdf)
                                fclose(cdf);
                        }
                        slashnum = 0;
                        delete dec;
                        Hr->LH = nullptr;
                    }
                    else {
                        //圧縮前データ長　0

                        //デスクトップパス付け加え
                        int i = 0;
                        mfile[25] = '/';

                        while (i <= Hr->LH->filenameleng) {
                            mfile[26 + i] = Hr->LH->filename[i];
                            if (Hr->LH->filename[i] == '/') {
                                slash[slashnum] = i; slashnum++;
                            }
                            i++;
                        }

                        if (slashnum > 0) {
                            foln[25] = '/';

                            i = 0;
                            int j = 0;
                            while (j < slashnum) {
                                while (i < slash[j]) {
                                    foln[26 + i] = Hr->LH->filename[i];
                                    i++;
                                }
                                foln[26 + i] = '\0';
                                //フォルダー作成
                                if (_mkdir(foln) == 0) {
                                    printf("フォルダ作成に成功しました。\n");
                                }
                                else {
                                    printf("フォルダ作成に失敗しました。\n");
                                    std::cout << foln << std::endl;
                                }
                                j++;
                            }
                            slashnum = 0;
                        }
                    }
                    
                }
                else if (Hr->LH->method == 0) {
                    std::cout << "圧縮なし" << std::endl;

                    //ファイル名//デスクトップパス付け加え
                    int i = 0;
                    mfile[25] = '/';

                    while (i <= Hr->LH->filenameleng) {
                        mfile[26 + i] = Hr->LH->filename[i];
                        if (Hr->LH->filename[i] == '/') {
                            slash[slashnum] = i; slashnum++;
                        }
                        i++;
                    }

                    if (slashnum > 0) {
                        foln[25] = '/';

                        i = 0;
                        int j = 0;
                        while (j < slashnum) {
                            while (i < slash[j]) {
                                foln[26 + i] = Hr->LH->filename[i];
                                i++;
                            }
                            foln[26 + i] = '\0';
                            //フォルダー作成
                            if (_mkdir(foln) == 0) {
                                printf("フォルダ作成に成功しました。\n");
                            }
                            else {
                                printf("フォルダ作成に失敗しました。\n");
                            }
                            j++;
                        }
                        slashnum = 0;
                    }

                    _Post_ _Notnull_ FILE* cdf;
                    fopen_s(&cdf, mfile, "wb");
                    if (!cdf) {
                        printf("ファイルを開けませんでした");
                        return 0;
                    }

                    //ファイル書き込み
                    DataWrite* fwrite = new DataWrite(cdf);

                    std::cout << "データ書き込み　"<<Hr->LH->nonsize << std::endl;
                    pos=fwrite->nocompressWrite(&fr, Hr->LH->nonsize,pos,cdf);

                    if (cdf)
                        fclose(cdf);

                    delete fwrite;
                }
            }
        }
        else {
            std::cout << "ローカル終了" << std::endl;
        }
    }
    if (fr)
        fr.close();
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します

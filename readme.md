# loas-tools
Tool for LATM/LOAS File dumped from MPEG2-TS    (LATM/LOAS version of FakeAACWAV.)  

# Options
## -F (FakeMode)
 Convert to FakeWave from LATM/LOAS File demuxed by DGIndexNV. 

## -D (DecodeMode)
 Convert to LATM/LOAS File from FakeWave(from FakeMode)

# つまりこれなに
LATM/LOASファイルを可逆カット編集のために疑似Wave化する物．  

# 使い方の例

1. loas-tools -F hoge.latm
2. 出力されたwavをAviSynthなどでカット編集後出力
3. loas-tools　-D hoge.wav

# Tips
--overwriteを有効にすると強制的に上書きすることが出来ます．  
コマンドラインのパースの関係上，入力ファイルは最後にしないと不幸になります．  
# ビット深度について
--bitdepth は疑似Waveのビット深度を切り替える物で，対応できるファイルサイズのビットレートに影響があります(デフォルトは24Bit)．  
所謂FakeAACWAVの1/2モードと同様の事が出来ますが，本来のWaveファイルと等しいビットレートまでしか扱えなくなることに注意してください．  
loas-toolsでは，2ch48KHzのWaveファイルとして出力するため，  
8Bitでは768kbps，16Bitでは1536kbps，24Bitでは2304kbpsが対応できる最大値となります．  
（厳密に言うと1フレームが1024サンプル相当(2048/4096/6144Bytes)のサイズを超えてはなりません）  
基本的には8で問題ないと思われます．が，例外としてARIB TR-B39において22.2chのLATMの最大ビットレートが1920kbpsと規定されているため，この場合は強制的に24bitとして扱うことにしています．
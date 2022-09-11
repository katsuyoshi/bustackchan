# ﾊﾞｽﾀｯｸﾁｬﾝ (BUSTACK-chan)

![BUSTACK-chan](https://i.gyazo.com/a0adf6c8071c3130c2bce93c617262e6.png)

[ｽﾀｯｸﾁｬﾝ](https://protopedia.net/prototype/2345)にインスパイアされて作りました。

ﾊﾞｽﾀｯｸﾁｬﾝ という名前の思いつきからのスタートでした。

protopediaのページも併せてご覧ください。

https://protopedia.net/prototype/3251


## 材料

|材料|数量|備考|
|:--|--:|:--|
|[M5Stack Basic]()|1||
|[M5Stack用NeoPixel互換 LEDテープ 10 cm](https://www.switch-science.com/catalog/5208/)|1||
|[M5Stack用GROVE互換ケーブル 10 cm](https://www.switch-science.com/catalog/5213/)|1||
|[ｽﾀｯｸﾁｬﾝの基板]()|1|ｽﾀｯｸﾁｬﾝの公開されているGithubページに[データ](https://github.com/meganetaaan/stack-chan/tree/main/schematics/m5-pantilt/gerber)があります。[nyaru lab](https://booth.pm/ja/items/4094998)さんが販売している様です。|
|[リチウムイオンポリマー電池](https://www.switch-science.com/catalog/3118/)|1||
|[M5Stack用2 x 15ピンヘッダ](https://www.switch-science.com/catalog/3654/)|1||
|その他ピンヘッダ、コンデンサ、抵抗など||[ｽﾀｯｸﾁｬﾝの基板組み立て](https://github.com/meganetaaan/stack-chan/blob/main/schematics/README_ja.md)説明のPWMサーボの項を参考にしてください。|

## 作り方

- 3Dプリンターでボディ、ヘッド、窓、シャーシ、タイヤを印刷します。
  |パーツ|備考|
  |:--|:--|
  |[ボディ](/models/body&nbsp;v19.gcode)|加工が必要です。|
  |[ヘッド](/models/head&nbsp;v4.gcode)||
  |[窓](/models/window&nbsp;v2.gcode)|左右と後部の3つが組みになっています。|
  |[シャーシ](/models/chassis3&nbsp;v6.gcode)||
  |[タイヤ](/models/tire&nbsp;v3.gcode)|4本組みになってます。|

- ポディでM5StackのUSB-Cコネクターと電源ボタンの部分が塞がれているので、[発泡スチロールカッター](https://getnavi.jp/zakka/464317/)などでコネクターが差し込みできる様にボディにスリットを入れます。(*1)

- ボディに窓をはめる際は平やすりなどで窓枠の下の方を少々削って調整する必要があります。左右も若干削った方が良いかもしれません。窓をボディにはめ、ピッタリの場合はそのままで問題ありませんが、緩い場合は接着剤などを使ってください。

- 竹串か竹籤(φ2.5mm)を52mmの長さで切り2本用意します。

- シャーシの穴の空いているところに竹串を通し両端にタイヤを差し込みます。入りずらい場合は竹串の先端を少し削ってください。

- ｽﾀｯｸﾁｬﾝ基板を組み立てます。オリジナル[ｽﾀｯｸﾁｬﾝの基板組み立て](https://github.com/meganetaaan/stack-chan/blob/main/schematics/README_ja.md)説明のPWMサーボの項を参考に組み立てます。

- 基板をM5Stackに付けます。

- LEDテープをシャーシに付けます。(*2) マッチ箱くらいの物を貼り付けその側面にLEDテープを貼り付けます。

- リチウムイオンポリマー電池をその上に貼り付けます。

- GROVE互換ケーブルでLEDテープと基板を接続します。基板側はピッチが合わないのでケーブルを切断しコネクターをつける加工をし、J3ピンヘッダーにGND側を合わせて差し込みます。

- リチウムイオンポリマー電池を基板と接続します。

- M5Stackをシャーシーの先頭部に差し込みます。この時ボタンが上になる様にします。

- ボディをシャーシーに被せます。

- ヘッドをボディの穴に合わせて差し込みます。

- プログラムを書き込みます。PlatformIOを使っています。
  - 使用しているライブラリーはplatformio.iniをご覧ください。




*1 将来的にはモデルに反映したいと思っています。  
*2 将来的にはモデルに取り付け用の仕掛けをつけたいと思います。


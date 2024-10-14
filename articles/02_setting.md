# 2.環境構築

本書の読み進め方について解説します。

## 2.1 使用言語

PHPを使用します。
srcフォルダ以下にDocker環境がありますので必要に応じて使用してください。

### 混同のDocker環境を使用する場合

```bash
cd src
# コンテナ起動
docker compose up or make up
# コンテナ内に接続
docker compose exec app or make exec-app
```

### SDK

https://packagist.org/packages/symbol-blockchain-community/

composerを使ってパッケージを取得します。


### リファレンス
今のところなし（2024/07/20現在）

## 2.2 サンプルソースコード

src以下にPHPのサンプルコードを配置しています。

### 出力値確認

var_dupmまたはechoを使用しています。

### アカウント
#### Alice
本書では主にAliceアカウントを中心として解説します。
3章で作成したAliceをその後の章でも引き続き使いますので、十分なXYMを送信した状態でお読みください。

#### Bob
Aliceとの送受信用のアカウントとして各章で必要に応じて作成します。その他、マルチシグの章などでCarolなどを使用します。

### 手数料
本書で紹介するトランザクションの手数料乗数は100でトランザクションを作成します。


## 2.3 事前準備
ノード一覧より任意のノードのページをChromeブラウザなどで開きます。本書ではテストネットを前提として解説しています。

- テストネット
    - https://symbolnodes.org/nodes_testnet/
- メインネット
    - https://symbolnodes.org/nodes/

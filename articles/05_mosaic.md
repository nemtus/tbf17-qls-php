# モザイク

本章ではモザイクの設定とその生成方法について解説します。
Symbolではトークンのことをモザイクと表現します。

> Wikipediaによると、トークンとは「紀元前8000年頃から紀元前3000年までのメソポタミアの地層から出土する直径が1cm前後の粘土で作られたさまざまな形状の物体」のことを指します。一方でモザイクとは「小片を寄せあわせ埋め込んで、絵（図像）や模様を表す装飾美術の技法。石、陶磁器（モザイクタイル）、有色無色のガラス、貝殻、木などが使用され、建築物の床や壁面、あるいは工芸品の装飾のために施される。」とあります。SymbolにおいてモザイクとはSymbolが作りなすエコシステムの様相を表すさまざまな構成要素、と考えることができます。

## モザイク生成

モザイク生成には作成するモザイクを定義します。

```php
$f = MosaicFlags::NONE;
$f += MosaicFlags::SUPPLY_MUTABLE; // 供給量変更可能
// $f += MosaicFlags::TRANSFERABLE; // 第三者への譲渡可否
$f += MosaicFlags::RESTRICTABLE; //制限設定の可否
$f += MosaicFlags::REVOKABLE; //発行者からの還収可否
$flags = new MosaicFlags($f);

$mosaicId = IdGenerator::generateMosaicId($aliceKey->address);
// 桁数のチェック（15桁なら先頭に0を付ける）
$hexMosaicId = strtoupper(dechex($mosaicId['id']));
if (strlen($hexMosaicId) === 15) {
    $hexMosaicId = '0' . $hexMosaicId;
}
echo $hexMosaicId . PHP_EOL;

```
222EF7A74ED6A71C
```

// モザイク定義
$mosaicDefTx = new EmbeddedMosaicDefinitionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey, // 署名者公開鍵
  id: new MosaicId($mosaicId['id']), // モザイクID
  divisibility: 2, // 分割可能性
  duration: new BlockDuration(0), //duration:有効期限
  nonce: new MosaicNonce($mosaicId['nonce']),
  flags: $flags,
);
```
※AggregateTransaction の Inner Transaction クラスは全て Embedded がつきます。

MosaicFlagsは以下の通りです。

```js
MosaicFlags {
  supplyMutable: false,
  transferable: false,
  restrictable: false,
  revokable: false
}
```
数量変更、第三者への譲渡、モザイクグローバル制限の適用、発行者からの還収の可否について指定します。
この項目は後で変更することはできません。

#### divisibility:可分性

可分性は小数点第何位まで数量の単位とするかを決めます。データは整数値として保持されます。

divisibility:0 = 1
divisibility:1 = 1.0
divisibility:2 = 1.00

#### duration:有効期限

0を指定した場合、無期限に使用することができます。
モザイク有効期限を設定した場合、期限が切れた後も消滅することはなくデータとしては残ります。
アカウント1つにつき1000までしか所有することはできませんのでご注意ください。


次に数量を変更します
```php
//モザイク変更
$mosaicChangeTx = new EmbeddedMosaicSupplyChangeTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey, // 署名者公開鍵
  mosaicId: new UnresolvedMosaicId($mosaicId['id']),
  delta: new Amount(10000),
  action: new MosaicSupplyChangeAction(MosaicSupplyChangeAction::INCREASE),
);
```
※AggregateTransaction の Inner Transaction クラスは全て Embedded がつきます。

supplyMutable:falseの場合、全モザイクが発行者にある場合だけ数量の変更が可能です。
divisibility > 0 の場合は、最小単位を1として整数値で定義してください。
（divisibility:2 で 1.00 作成したい場合は100と指定）

MosaicSupplyChangeActionは以下の通りです。
```js
{0: 'DECREASE', 1: 'INCREASE'}
```
増やしたい場合はIncreaseを指定します。
上記2つのトランザクションをまとめてアグリゲートトランザクションを作成します。

```php
// マークルハッシュの算出
$embeddedTransactions = [$mosaicDefTx, $mosaicChangeTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートTx作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100);  // 手数料

// 署名
$sig = $aliceKey->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

/**
 * アナウンス
 */
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();
$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```

アグリゲートトランザクションの特徴として、まだ存在していないモザイクの数量を変更しようとしている点に注目してください。
配列化した時に、矛盾点がなければ1つのブロック内で問題なく処理することができます。


### 確認
モザイク作成したアカウントが持つモザイク情報を確認します。

```php
// 3.3 アカウント情報の確認 - 所有モザイク一覧の取得 を事前に実施する
$accountApiInstance = new AccountRoutesApi($client, $config);
$mosaicApiInstance = new MosaicRoutesApi($client, $config);

$account = $accountApiInstance->getAccountInfo($aliceKey->address);
foreach($account->getAccount()->getMosaics() as $mosaic) {
  $mocaisInfo = $mosaicApiInstance->getMosaic($mosaic->getId());
  echo "\n===モザイク情報===" . PHP_EOL;
  var_dump($mocaisInfo);
}
```
###### 出力例
```php
===モザイク情報===
object(SymbolRestClient\Model\MosaicInfoDTO)#131 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(2) {
    ["id"]=>
    string(24) "669E4D9884E82060AFBD9C2E"
    ["mosaic"]=>
    object(SymbolRestClient\Model\MosaicDTO)#121 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(9) {
        ["version"]=>
        int(1)
        ["id"]=>
        string(16) "12679808DC2A1493"
        ["supply"]=>
        string(5) "10000"
        ["start_height"]=>
        string(7) "1596556"
        ["owner_address"]=>
        string(48) "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0"
        ["revision"]=>
        int(1)
        ["flags"]=>
        int(13)
        ["divisibility"]=>
        int(2)
        ["duration"]=>
        string(1) "0"
      }
    }
  }
}
```

## モザイク送信

作成したモザイクを送信します。
よく、ブロックチェーンに初めて触れる方は、
モザイク送信について「クライアント端末に保存されたモザイクを別のクライアント端末へ送信」することとイメージされている人がいますが、
モザイク情報はすべてのノードで常に共有・同期化されており、送信先に未知のモザイク情報を届けることではありません。
正確にはブロックチェーンへ「トランザクションを送信」することにより、アカウント間でのトークン残量を組み替える操作のことを言います。

```php
// 受信アカウント作成
$bobKey = $facade->createAccount(PrivateKey::random());
$bobAddress = $bobKey->address;

$tx = new TransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  recipientAddress: $bobAddress,  // 受信者アドレス
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId("0x12679808DC2A1493"),  //5.1で作成したモザイクID
      amount: new Amount(100) //過分性が2のため、100を指定することで送信量が1モザイクとなる
    )
  ],
  message: "\0モザイク送信",
  deadline: new Timestamp($facade->now()->addHours(2)),
);
$facade->setMaxFee($tx, 100); // 手数料

// 署名とアナウンス
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}

```

##### 送信モザイクリスト

複数のモザイクを一度に送信できます。
XYMを送信するには以下のモザイクIDを指定します。
- メインネット：6BED913FA20223F8
- テストネット：72C0212E67A08BCE

#### 送信量
小数点もすべて整数にして指定します。
XYMは可分性6なので、1XYM=1000000で指定します。

### 送信確認

```php
$txInfo = $apiInstance->getConfirmedTransaction(
  $hash  // 送信トランザクションのハッシュ
);
```
###### 出力例
```php
object(SymbolRestClient\Model\TransactionInfoDTO)#127 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(3) {
    ["id"]=>
    string(24) "669E5AAB527B051AC20A7DA6"
    ["meta"]=>
    object(SymbolRestClient\Model\TransactionInfoDTOMeta)#120 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(8) {
        ["height"]=>
        string(7) "1596649"
        ["hash"]=>
        string(64) "2781E0DD5346405AC7B5CEC0DF6C8A52D164C86F9D75BA024ED8824E6B516F2F"
        ["merkle_component_hash"]=>
        string(64) "2781E0DD5346405AC7B5CEC0DF6C8A52D164C86F9D75BA024ED8824E6B516F2F"
        ["index"]=>
        int(0)
        ["timestamp"]=>
        string(11) "54403463949"
        ["fee_multiplier"]=>
        int(100)
        ["aggregate_hash"]=>
        NULL
        ["aggregate_id"]=>
        NULL
      }
    }
    ["transaction"]=>
    object(SymbolRestClient\Model\TransactionInfoDTOTransaction)#138 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(58) {
        ["size"]=>
        int(211)
        ["signature"]=>
        string(128) "6467909E4563D3BDF83400FD955D0F5CCE3CAAC7E9D9CB35037CDF4DE4BE2857\
                    CEAB9ED120BB38882076B8DC13E9F11E270A05821663B71D38E1696A5CA60C06"
        ["signer_public_key"]=>
        string(64) "25189135BF2307DCBCD1657A34ABC3FD\
                    EEC04A126D4572876BCA4F514DB5AC9B"
        ["version"]=>
        int(1)
        ["network"]=>
        int(152)
        ["type"]=>
        int(16724)
        ["max_fee"]=>
        string(5) "21100"
        ["deadline"]=>
        string(11) "54410638989"
        ["linked_public_key"]=>
        NULL
        ["link_action"]=>
        NULL
        ["start_epoch"]=>
        NULL
        ["end_epoch"]=>
        NULL
        ["transactions_hash"]=>
        NULL
        ["cosignatures"]=>
        NULL
        ["transactions"]=>
        NULL
        ["mosaic_id"]=>
        NULL
        ["amount"]=>
        NULL
        ["duration"]=>
        NULL
        ["hash"]=>
        NULL
        ["recipient_address"]=>
        string(48) "98387B89DFD6F53C8FC960A509392E527278E1C71CDAAF74"
        ["secret"]=>
        NULL
        ["hash_algorithm"]=>
        NULL
        ["proof"]=>
        NULL
        ["target_address"]=>
        NULL
        ["scoped_metadata_key"]=>
        NULL
        ["value_size_delta"]=>
        NULL
        ["value_size"]=>
        NULL
        ["value"]=>
        NULL
        ["target_mosaic_id"]=>
        NULL
        ["target_namespace_id"]=>
        NULL
        ["id"]=>
        NULL
        ["nonce"]=>
        NULL
        ["flags"]=>
        NULL
        ["divisibility"]=>
        NULL
        ["delta"]=>
        NULL
        ["action"]=>
        NULL
        ["source_address"]=>
        NULL
        ["parent_id"]=>
        NULL
        ["registration_type"]=>
        NULL
        ["name"]=>
        NULL
        ["namespace_id"]=>
        NULL
        ["address"]=>
        NULL
        ["alias_action"]=>
        NULL
        ["min_removal_delta"]=>
        NULL
        ["min_approval_delta"]=>
        NULL
        ["address_additions"]=>
        NULL
        ["address_deletions"]=>
        NULL
        ["restriction_flags"]=>
        NULL
        ["restriction_additions"]=>
        NULL
        ["restriction_deletions"]=>
        NULL
        ["reference_mosaic_id"]=>
        NULL
        ["restriction_key"]=>
        NULL
        ["previous_restriction_value"]=>
        NULL
        ["new_restriction_value"]=>
        NULL
        ["previous_restriction_type"]=>
        NULL
        ["new_restriction_type"]=>
        NULL
        ["mosaics"]=>
        array(2) {
          [0]=>
          object(SymbolRestClient\Model\UnresolvedMosaic)#139 (2) {
            ["openAPINullablesSetToNull":protected]=>
            array(0) {
            }
            ["container":protected]=>
            array(2) {
              ["id"]=>
              string(16) "12679808DC2A1493"
              ["amount"]=>
              string(3) "100"
            }
          }
          [1]=>
          object(SymbolRestClient\Model\UnresolvedMosaic)#140 (2) {
            ["openAPINullablesSetToNull":protected]=>
            array(0) {
            }
            ["container":protected]=>
            array(2) {
              ["id"]=>
              string(16) "72C0212E67A08BCE"
              ["amount"]=>
              string(7) "1000000"
            }
          }
        }
        ["message"]=>
        string(38) "00E383A2E382B6E382A4E382AFE98081E4BFA1"
      }
    }
  }
}
```
TransferTransactionのmosaicsに2種類のモザイクが送信されていることが確認できます。また、TransactionInfoに承認されたブロックの情報が記載されています。

## 現場で使えるヒント

### 所有証明

前章でトランザクションによる存在証明について説明しました。
アカウントの作成した送信指示が消せない形で残せるので、絶対につじつまの合う台帳を作ることができます。
すべてのアカウントの「絶対に消せない送信指示」の蓄積結果として、各アカウントは自分のモザイク所有を証明することができます。
（本ドキュメントでは所有を「自分の意思で手放すことができる状態」とします。少し話題がそれますが、法律的にはデジタルデータに所有権が認められていないのも、一度知ってしまったデータは自分の意志では忘れたことを他人に証明することができない点に注目すると「手放すことができる状態」の意味に納得がいくかもしれません。ブロックチェーンによりそのデータの放棄を明確に示すことができるのですが、詳しくは法律の専門の方にお任せします。）

#### NFT(non fungible token)

発行枚数を1に限定し、supplyMutableをfalseに設定することで、1つだけしか存在しないトークンを発行できます。
モザイクは作成したアカウントアドレスを改ざんできない情報として保有しているので、
そのアカウントの送信トランザクションをメタ情報として利用できます。
7章で説明するメタデータをモザイクに登録する方法もありますが、その方法は登録アカウントとモザイク作成者の連署によって更新可能なことにご注意ください。

NFTの実現方法はいろいろありますが、その一例の処理概要を以下に例示します（実行するためにはnonceやフラグ情報を適切に設定してください）。
```php
$f = MosaicFlags::NONE;
// $f += MosaicFlags::SUPPLY_MUTABLE; // 供給量変更可能
$f += MosaicFlags::TRANSFERABLE; // 第三者への譲渡可否
$f += MosaicFlags::RESTRICTABLE; //制限設定の可否
$f += MosaicFlags::REVOKABLE; //発行者からの還収可否
$flags = new MosaicFlags($f);

$mosaicId = IdGenerator::generateMosaicId($aliceKey->address);

// モザイク定義
$mosaicDefTx = new EmbeddedMosaicDefinitionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey, // 署名者公開鍵
  id: new MosaicId($mosaicId['id']), // モザイクID
  divisibility: 0, // 分割可能性
  duration: new BlockDuration(0), //duration:有効期限
  nonce: new MosaicNonce($mosaicId['nonce']),
  flags: $flags,
);

//モザイク変更
$mosaicChangeTx = new EmbeddedMosaicSupplyChangeTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey, // 署名者公開鍵
  mosaicId: new UnresolvedMosaicId($mosaicId['id']),
  delta: new Amount(1),
  action: new MosaicSupplyChangeAction(MosaicSupplyChangeAction::INCREASE),
);

//NFTデータ
$nftTx = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  recipientAddress: $bobAddress,  // 受信者アドレス
  message: "\0NFT送信", //NFTデータ実態
);

// マークルハッシュの算出
$embeddedTransactions = [$mosaicDefTx, $mosaicChangeTx, $nftTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// モザイクの生成とNFTデータをアグリゲートしてブロックに登録
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100);  // 手数料

// 署名
$sig = $aliceKey->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

/**
 * アナウンス
 */
try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```
※AggregateTransaction の Inner Transaction クラスは全て Embedded がつくので EmbeddedTransferTransactionV1 を使用します。

モザイク生成時のブロック高と作成アカウントがモザイク情報に含まれているので同ブロック内のトランザクションを検索することにより、
紐づけられたNFTデータを取得することができます。

##### 注意事項
モザイクの作成者が全数量を所有している場合、供給量を変更することが可能です。
またトランザクションに分割してデータを記録した場合、改ざんできませんがデータの追記は可能です。
NFTを運用する場合はモザイク作成者の秘密鍵を厳重に管理・あるいは破棄するなど、適切な運用にご注意ください。


#### 回収可能なポイント運用

transferableをfalseに設定することで転売が制限されるため、資金決済法の影響を受けにくいポイントを定義することができます。
またrevokableをtrueに設定することで、ユーザ側が秘密鍵を管理しなくても使用分を回収できるような中央管理型のポイント運用を行うことができます。

```php
$f = MosaicFlags::NONE;
$f += MosaicFlags::SUPPLY_MUTABLE; // 供給量変更可能
// $f += MosaicFlags::TRANSFERABLE; // 第三者への譲渡可否
$f += MosaicFlags::RESTRICTABLE; //制限設定の可否
$f += MosaicFlags::REVOKABLE; //発行者からの還収可否
$flags = new MosaicFlags($f);
```

トランザクションは以下のように記述します。

```js
$revocationTx = new MosaicSupplyRevocationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  mosaic: new UnresolvedMosaic(
    mosaicId: new UnresolvedMosaicId("0x12679808DC2A1493"),  //5.1で作成したモザイクID
    amount: new Amount(100) //過分性が2のため、100を指定することで送信量が1モザイクとなる
  ),
  sourceAddress: new UnresolvedAddress(
    "TDZ46RYMP6XTRQLOGI3AWULOHV56LBUE7M43MCI"
  ), //回収ターゲット
);
$facade->setMaxFee($revocationTx, 100); // 手数料

```





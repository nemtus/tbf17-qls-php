# ロック

Symbolブロックチェーンにはハッシュロックとシークレットロックの２種類のロック機構があります。

## ハッシュロック

ハッシュロックは後でアナウンスされる予定のトランザクションを事前にハッシュ値で登録しておくことで、
該当トランザクションがアナウンスされた場合に、そのトランザクションをAPIノード上で処理せずにロックさせて、署名が集まってから処理を行うことができます。
アカウントが所有するモザイクを操作できないようにロックするわけではなく、ロックされるのはハッシュ値の対象となるトランザクションとなります。
ハッシュロックにかかる費用は10XYM、有効期限は最大約48時間です。ロックしたトランザクションが承認されれば10XYMは返却されます。

### アグリゲートボンデッドトランザクションの作成

```php
$bobKey = $facade->createAccount(
    new PrivateKey(
        "ED949592C90CA58A16CB5BEC303DB011A48373063DDB0C4CFD6DFD01F********"
    )
);
$bobAddress = $bobKey->address;

$namespaceIds = IdGenerator::generateNamespacePath('symbol.xym');
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);

// アグリゲートTxに含めるTxを作成
$tx1 = new EmbeddedTransferTransactionV1(
  signerPublicKey: $aliceKey->publicKey,
  recipientAddress: $bobAddress,
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
      amount: new Amount(1000000) // 金額(1XYM)
    )
  ],
  message: "",  //メッセージなし
);

$tx2 = new EmbeddedTransferTransactionV1(
  signerPublicKey: $bobKey->publicKey,
  recipientAddress: $aliceKey->address,
  message: "\0thank you!",
);

// マークルハッシュの算出
$embeddedTransactions = [$tx1, $tx2];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートボンデットTx作成
$aggregateTx = new AggregateBondedTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 1);  // 手数料

// 署名
$sig = $aliceKey->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);
echo 'アグリゲートボンデットTxHash' . PHP_EOL;
echo $facade->hashTransaction($aggregateTx) . PHP_EOL;

/**
 * ハッシュロック
 */
$hashLockTx = new HashLockTransactionV1(
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  network: new NetworkType(NetworkType::TESTNET),
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  duration: new BlockDuration(480), // 有効期限
  hash: new Hash256($facade->hashTransaction($aggregateTx)), // ペイロードのハッシュ
  mosaic: new UnresolvedMosaic(
    mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
    amount: new Amount(10 * 1000000) // 金額(10XYM)
  )
);
$facade->setMaxFee($hashLockTx, 100);  // 手数料

// 署名
$hashLockSig = $aliceKey->signTransaction($hashLockTx);
$hashLockJsonPayload = $facade->attachSignature($hashLockTx, $hashLockSig);
```

tx1,tx2の2つのトランザクションをaggregateArrayで配列にする時に、送信元アカウントの公開鍵を指定します。
公開鍵はアカウントの章を参考に事前にAPIで取得しておきましょう。
配列化されたトランザクションはブロック承認時にその順序で整合性を検証されます。
例えば、tx1でNFTをAliceからBobへ送信した後、tx2でBobからCarolへ同じNFTを送信することは可能ですが、tx2,tx1の順序でアグリゲートトランザクションを通知するとエラーになります。
また、アグリゲートトランザクションの中に1つでも整合性の合わないトランザクションが存在していると、アグリゲートトランザクション全体がエラーとなってチェーンに承認されることはありません。

### ハッシュロックトランザクションの作成と署名、アナウンス
```php

//ハッシュロックTX作成
$hashLockTx = new HashLockTransactionV1(
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  network: new NetworkType(NetworkType::TESTNET),
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  duration: new BlockDuration(480), // 有効期限
  hash: new Hash256($facade->hashTransaction($aggregateTx)), // ペイロードのハッシュ
  mosaic: new UnresolvedMosaic(
    mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
    amount: new Amount(10 * 1000000) // 金額(10XYM)
  )
);
$facade->setMaxFee($hashLockTx, 100);  // 手数料

// 署名
$hashLockSig = $aliceKey->signTransaction($hashLockTx);
$hashLockJsonPayload = $facade->attachSignature($hashLockTx, $hashLockSig);

/**
 * ハッシュロックをアナウンス
 */
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();
$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($hashLockJsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```

### アグリゲートボンデッドトランザクションのアナウンス

エクスプローラーなどで確認した後、ボンデッドトランザクションをネットワークにアナウンスします。
```php
try {
  $result = $apiInstance->announcePartialTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}

```


### 連署
ロックされたトランザクションを指定されたアカウント(Bob)で連署します。

```php
$txInfo = $apiInstance->getPartialTransaction(
    $facade->hashTransaction($aggregateTx)
);

// 連署者の連署
$txInfo = $apiInstance->getPartialTransaction(
    $facade->hashTransaction($aggregateTx)
);

// // 連署者の連署
$signTxHash = new Hash256($txInfo->getMeta()->getHash());
$signature = new Signature($bobKey->keyPair->sign($signTxHash->binaryData));
$body = [
    'parentHash' => $signTxHash->__toString(),
    'signature' => $signature->__toString(),
    'signerPublicKey' => $bobKey->publicKey->__toString(),
    'version' => '0'
];

print_r($body);

//アナウンス
try {
  $result = $apiInstance->announceCosignatureTransaction($body);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```

### 注意点
ハッシュロックトランザクションは起案者(トランザクションを作成し最初に署名するアカウント)に限らず、誰が作成してアナウンスしても大丈夫ですが、
アグリゲートトランザクションにそのアカウントがsignerとなるトランザクションを含めるようにしてください。
モザイク送信無し＆メッセージ無しのダミートランザクションでも問題ありません（パフォーマンスに影響が出るための仕様とのことです）
また、ハッシュロックトランザクションが承認された直後にボンデッドトランザクションをアナウンスした場合、
ハッシュロックの承認がネットワーク全体に伝播する前にボンデッドトランザクションを受け取ってしまうノードが出てくる可能性があります。
そのような状態を防ぐために、ボンデッドトランザクションはハッシュロックトランザクションが承認された後しばらく待ってからアナウンスするようにしてください。

## シークレットロック・シークレットプルーフ

シークレットロックは事前に共通パスワードを作成しておき、指定モザイクをロックします。
受信者が有効期限内にパスワードの所有を証明することができればロックされたモザイクを受け取ることができる仕組みです。

ここではAliceが1XYMをロックしてBobが解除することで受信する方法を説明します。

ロック解除にBob側からトランザクションをアナウンスする必要があるのでFAUCETで10XYMほど受信しておきます。

```
//FAUCET URL出力
echo "https://testnet.symbol.tools/?recipient=" . $bobAddress . "&amount=10";
```

### シークレットロック

ロック・解除にかかわる共通暗号を作成します。

PHPではSHA3-256ハッシュ関数を提供するライブラリをインストールします。

```bash
composer require symfony/polyfill-php70
```

```php
require 'vendor/autoload.php';

use Symfony\Polyfill\Php70\Php70;

$proof = random_bytes(20); // 解除用キーワード
$secret = hash('sha3-256', $proof, true); // ロック用キーワード

echo "secret: " . bin2hex($secret) . PHP_EOL;
echo "proof: " . bin2hex($proof) . PHP_EOL;
```

###### 出力例
```
secret: 40eb770de739fa22617e8f2212b1fe821107e97f32a2c288264e0ccb644610f1
proof: 1f62d5bdfbffb657d5c4a67d5d2f5617aec14c43
```

トランザクションを作成・署名・アナウンスします
```php
$lockTx = new SecretLockTransactionV1(
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  network: new NetworkType(NetworkType::TESTNET),
  mosaic: new UnresolvedMosaic(
    mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
    amount: new Amount(1000000) // ロックするモザイク
  ),
  duration: new BlockDuration(480), //ロック期間
  hashAlgorithm: new LockHashAlgorithm(LockHashAlgorithm::SHA3_256), // ハッシュアルゴリズム
  secret: new Hash256($secret), // ロック用キーワード
  recipientAddress: $bobAddress, // 解除時の転送先：Bob
);
$facade->setMaxFee($lockTx, 100);  // 手数料

// 署名
$lockSig = $aliceKey->signTransaction($lockTx);
$payload = $facade->attachSignature($lockTx, $lockSig);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```

LockHashAlgorithmは以下の通りです。
```js
{0: 'Op_Sha3_256', 1: 'Op_Hash_160', 2: 'Op_Hash_256'}
```

ロック時に解除先を指定するのでBob以外のアカウントが解除しても転送先（Bob）を変更することはできません。
ロック期間は最長で365日(ブロック数を日換算)までです。

承認されたトランザクションを確認します。
```php
$secretAipInstance = new SecretLockRoutesApi($client, $config);
$resutl = $secretAipInstance->searchSecretLock(secret: $secret);
```
###### 出力例
```
{
  "data": [
    {
      "id": "66A4247084E82060AFC6705C",
      "lock": {
        "version": 1,
        "ownerAddress": "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
        "mosaicId": "72C0212E67A08BCE",
        "amount": "1000000",
        "endHeight": "1607702",
        "status": 0,
        "hashAlgorithm": 0,
        "secret": 
          "A8E4F52ADDA0AFCD413D86A51589711CF045F144EEE56FC9CE96095D6AB79E9E",
        "recipientAddress": "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26",
        "compositeHash": 
          "57541680B0E2DFAE504A6937AAB6A65A7008C25F905FCA68A24C21165314023C"
      }
    }
  ],
  "pagination": {
    "pageNumber": 1,
    "pageSize": 10
  }
}
```
ロックしたAliceがownerAddress、受信予定のBobがrecipientAddressに記録されています。
secret情報が公開されていて、これに対応するproofをBobがネットワークに通知します。


### シークレットプルーフ

解除用キーワードを使用してロック解除します。
Bobは事前に解除用キーワードを入手しておく必要があります。

```php
$proofTx = new SecretProofTransactionV1(
  signerPublicKey: $bobKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  network: new NetworkType(NetworkType::TESTNET),
  hashAlgorithm: new LockHashAlgorithm(LockHashAlgorithm::SHA3_256), // ハッシュアルゴリズム
  secret: new Hash256($secret), // ロック用キーワード
  recipientAddress: $bobAddress, // 解除時の転送先：Alice
  proof: $proof, // 解除用キーワード
);
$facade->setMaxFee($proofTx, 100);  // 手数料

// 署名
$proofSig = $bobKey->signTransaction($proofTx);
$payload = $facade->attachSignature($proofTx, $proofSig);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
```

承認結果を確認します。
```js
$txInfo = $apiInstance->getConfirmedTransaction(
    $facade->hashTransaction($proofTx)
);
echo $txInfo;
```
###### 出力例
```
{
  "id": "66A429D1527B051AC20AE9B3",
  "meta": {
    "height": "1607263",
    "hash": 
      "91387B92117ACE7A6BB5596720DAEC6FEA89E42076BD40320381CE1A86C0D57D",
    "merkleComponentHash": 
      "91387B92117ACE7A6BB5596720DAEC6FEA89E42076BD40320381CE1A86C0D57D",
    "index": 1,
    "timestamp": "54784174665",
    "feeMultiplier": 100
  },
  "transaction": {
    "size": 207,
    "signature": 
      "52CFCE339A361AED998B12EC7B4976542F5F413694BD6E9395DB37C980977839\
        FE2ABEBF577C78CFCA065F0E9C2D72228859B830B75FEBC64551396FAE1D7B00",
    "signerPublicKey": 
      "D47E477DA7CAE6127779523270F91BD000D7D0E06DA56192FE911460DC39081C",
    "version": 1,
    "network": 152,
    "type": 16978,
    "maxFee": "20700",
    "deadline": "54791351985",
    "recipientAddress": "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26",
    "secret": 
      "2F63B181A3C0A9C549F13492D9A8A3D6851B911F813A4C4D70539A0BE55D277D",
    "hashAlgorithm": 0,
    "proof": "86D75E3321D5EE227E14C3CFD67E378EB3F3FD64"
  }
}
```

SecretProofTransactionにはモザイクの受信量の情報は含まれていません。
ブロック生成時に作成されるレシートで受信量を確認します。
レシートタイプ:LockSecret_Completed でBob宛のレシートを検索してみます。

```php
$receiptApiInstance = new ReceiptRoutesApi($client, $config);
$result = $receiptApiInstance->searchReceipts(
  receipt_type: new ReceiptType(ReceiptType::LOCK_SECRET_COMPLETED),
  target_address:$bobAddress
);
echo 'レシート' . PHP_EOL;
echo $result . PHP_EOL;
```
###### 出力例
```
{
  "data": [
    {
      "id": "66A428AC527B051AC20AE98E",
      "meta": {
        "timestamp": "54783880862"
      },
      "statement": {
        "height": "1607253",
        "source": {
          "primaryId": 2,
          "secondaryId": 0
        },
        "receipts": [
          {
            "version": 1,
            "type": 8786,
            "mosaicId": "72C0212E67A08BCE",
            "amount": "1000000",
            "targetAddress": 
              "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26"
          }
        ]
      }
    },
    {
      "id": "66A428D2527B051AC20AE996",
      "meta": {
        "timestamp": "54783918766"
      },
      "statement": {
        "height": "1607254",
        "source": {
          "primaryId": 2,
          "secondaryId": 0
        },
        "receipts": [
          {
            "version": 1,
            "type": 8786,
            "mosaicId": "72C0212E67A08BCE",
            "amount": "1000000",
            "targetAddress": 
              "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26"
          }
        ]
      }
    },
    {
      "id": "66A42992527B051AC20AE9AC",
      "meta": {
        "timestamp": "54784111700"
      },
      "statement": {
        "height": "1607261",
        "source": {
          "primaryId": 2,
          "secondaryId": 0
        },
        "receipts": [
          {
            "version": 1,
            "type": 8786,
            "mosaicId": "72C0212E67A08BCE",
            "amount": "1000000",
            "targetAddress": 
              "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26"
          }
        ]
      }
    },
    {
      "id": "66A429D1527B051AC20AE9B7",
      "meta": {
        "timestamp": "54784174665"
      },
      "statement": {
        "height": "1607263",
        "source": {
          "primaryId": 2,
          "secondaryId": 0
        },
        "receipts": [
          {
            "version": 1,
            "type": 8786,
            "mosaicId": "72C0212E67A08BCE",
            "amount": "1000000",
            "targetAddress": 
              "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26"
          }
        ]
      }
    },
    {
      "id": "66A42B8F527B051AC20AE9D6",
      "meta": {
        "timestamp": "54784619386"
      },
      "statement": {
        "height": "1607275",
        "source": {
          "primaryId": 2,
          "secondaryId": 0
        },
        "receipts": [
          {
            "version": 1,
            "type": 8786,
            "mosaicId": "72C0212E67A08BCE",
            "amount": "1000000",
            "targetAddress": 
              "98665DDAC8CCF6EE40B1D50800DC8C6C27B314988A1FDB26"
          }
        ]
      }
    }
  ],
  "pagination": {
    "pageNumber": 1,
    "pageSize": 10
  }
}
```

ReceiptTypeは以下の通りです。

```js
{
  4685: 'Mosaic_Rental_Fee',
  4942: 'Namespace_Rental_Fee',
  8515: 'Harvest_Fee',
  8776: 'LockHash_Completed',
  8786: 'LockSecret_Completed',
  9032: 'LockHash_Expired',
  9042: 'LockSecret_Expired',
  12616: 'LockHash_Created',
  12626: 'LockSecret_Created',
  16717: 'Mosaic_Expired',
  16718: 'Namespace_Expired',
  16974: 'Namespace_Deleted',
  20803: 'Inflation',
  57667: 'Transaction_Group',
  61763: 'Address_Alias_Resolution',
  62019: 'Mosaic_Alias_Resolution'
}

8786: 'LockSecret_Completed' :ロック解除完了
9042: 'LockSecret_Expired'　：ロック期限切れ
```

## 現場で使えるヒント


### 手数料代払い

一般的にブロックチェーンはトランザクション送信に手数料を必要とします。
そのため、ブロックチェーンを利用しようとするユーザは事前に手数料を取引所から入手しておく必要があります。
このユーザが企業である場合はその管理方法も加えてさらにハードルの高い問題となります。
アグリゲートトランザクションを使用することでハッシュロック費用とネットワーク手数料をサービス提供者が代理で負担することができます。

### タイマー送信

シークレットロックは指定ブロック数を経過すると元のアカウントへ払い戻されます。
この原理を利用して、シークレットロックしたアカウントにたいしてロック分の費用をサービス提供者が充足しておけば、
期限が過ぎた後ユーザ側がロック分のトークン所有量が増加することになります。
一方で、期限が過ぎる前にシークレット証明トランザクションをアナウンスすると、送信が完了し、サービス提供者に充当戻るためキャンセル扱いとなります。

### アトミックスワップ
シークレットロックを使用して、他のチェーンとのトークン・モザイクの交換を行うことができます。
他のチェーンではハッシュタイムロックコントラクト(HTLC)と呼ばれているためハッシュロックと間違えないようにご注意ください。



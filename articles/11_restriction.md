# 11.制限

アカウントに対する制限とモザイクのグローバル制限についての方法を紹介します。
本章では、既存アカウントの権限を制限してしまうので、使い捨てのアカウントを新規に作成してお試しください。

```php
//使い捨てアカウントCarolの生成
$carolKey = $facade->createAccount(PrivateKey::random());

echo "https://testnet.symbol.tools/?recipient=" . $carolKey->address . "&amount=20" . PHP_EOL;
```
## 11.1 アカウント制限

### 指定アドレスからの受信制限・指定アドレスへの送信制限
```php

$bobKey = $facade->createAccount(new PrivateKey('7CBA79757479402DDCDE6577F938CDE6FD9035ACADC1E343AE155EFA679D462A') );
$bobAddress = $bobKey->address;
echo 'Bob' . PHP_EOL;
echo 'Address: ' . $bobAddress . PHP_EOL;

// 制限設定
$f = AccountRestrictionFlags::ADDRESS; // アドレス制限
$f += AccountRestrictionFlags::BLOCK; // ブロック
$flags = new AccountRestrictionFlags($f);

// アドレス制限設定Tx作成
$tx = new AccountAddressRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  restrictionFlags: $flags, // 制限フラグ
  restrictionAdditions:[
    $bobAddress
  ],  // 設定アドレス
  restrictionDeletions:[] // 削除アドレス
);
$facade->setMaxFee($tx, 100);

// 署名
$sig = $carolKey->signTransaction($tx);
$jsonPayload = $facade->attachSignature($tx, $sig);

try {
  $result = $apiInstance->announceTransaction($jsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
```

restrictionFlags は v2 の AddressRestrictionFlag に相当します。 AddressRestrictionFlag との対応は以下の通りです。

- AllowIncomingAddress：指定アドレスからのみ受信許可
  - symbolSdk.symbol.AccountRestrictionFlags.ADDRESS
- AllowOutgoingAddress：指定アドレス宛のみ送信許可
  - symbolSdk.symbol.AccountRestrictionFlags.ADDRESS + symbolSdk.symbol.AccountRestrictionFlags.OUTGOING
- BlockIncomingAddress：指定アドレスからの受信受拒否
  - symbolSdk.symbol.AccountRestrictionFlags.ADDRESS + symbolSdk.symbol.AccountRestrictionFlags.BLOCK
- BlockOutgoingAddress：指定アドレス宛への送信禁止
  - symbolSdk.symbol.AccountRestrictionFlags.ADDRESS + symbolSdk.symbol.AccountRestrictionFlags.BLOCK + symbolSdk.symbol.AccountRestrictionFlags.OUTGOING

### 指定モザイクの受信制限
```php
$f = AccountRestrictionFlags::MOSAIC_ID; // モザイク制限
$f += AccountRestrictionFlags::BLOCK; // ブロック
$flags = new AccountRestrictionFlags($f);

$tx = new AccountMosaicRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  restrictionFlags: $flags, // 制限フラグ
  restrictionAdditions:[
    new UnresolvedMosaicId($namespaceId)
  ],  // 設定モザイク
  restrictionDeletions:[] // 削除モザイク
);
$facade->setMaxFee($tx, 1000);

// 署名
$sig = $carolKey->signTransaction($tx);
$jsonPayload = $facade->attachSignature($tx, $sig);

try {
  $result = $apiInstance->announceTransaction($jsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
```

アカウント制限と同様、 restrictionFlags は v2 の MosaicRestrictionFlag に相当します。 MosaicRestrictionFlag との対応は以下の通りです。

- AllowMosaic：指定モザイクを含むトランザクションのみ受信許可
  - symbolSdk.symbol.AccountRestrictionFlags.MOSAIC_ID
- BlockMosaic：指定モザイクを含むトランザクションを受信拒否
  -symbolSdk.symbol.AccountRestrictionFlags.MOSAIC_ID + symbolSdk.symbol.AccountRestrictionFlags.BLOCK
モザイク送信の制限機能はありません。また、後述するモザイクのふるまいを制限するグローバルモザイク制限と混同しないようにご注意ください。

### 指定トランザクションの送信制限

```php
$f = AccountRestrictionFlags::TRANSACTION_TYPE; // トランザクション制限
$f += AccountRestrictionFlags::OUTGOING; // 送信
$flags = new AccountRestrictionFlags($f);

$tx = new AccountOperationRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  restrictionFlags: $flags, // 制限フラグ
  restrictionAdditions:[
    new TransactionType(TransactionType::ACCOUNT_OPERATION_RESTRICTION)
  ],  // 設定トランザクション
  restrictionDeletions:[] // 削除トランザクション
);
$facade->setMaxFee($tx, 100);

// 署名
$sig = $carolKey->signTransaction($tx);
$jsonPayload = $facade->attachSignature($tx, $sig);

try {
  $result = $apiInstance->announceTransaction($jsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
```

アカウント制限やモザイク制限と同様、restrictionFlags は v2 の OperationRestrictionFlag に相当します。 OperationRestrictionFlag との対応は以下の通りです。

- AllowOutgoingTransactionType：指定トランザクションの送信のみ許可
  - symbolSdk.symbol.AccountRestrictionFlags.TRANSACTION_TYPE + symbolSdk.symbol.AccountRestrictionFlags.OUTGOING
- BlockOutgoingTransactionType：指定トランザクションの送信を禁止
 - symbolSdk.symbol.AccountRestrictionFlags.TRANSACTION_TYPE + symbolSdk.symbol.AccountRestrictionFlags.OUTGOING + symbolSdk.symbol.AccountRestrictionFlags.BLOCK

トランザクション受信の制限機能はありません。指定できるオペレーションは以下の通りです。

TransactionTypeについては以下の通りです。
```js
{16705: 'AGGREGATE_COMPLETE', 16707: 'VOTING_KEY_LINK', 16708: 'ACCOUNT_METADATA', 16712: 'HASH_LOCK', 16716: 'ACCOUNT_KEY_LINK', 16717: 'MOSAIC_DEFINITION', 16718: 'NAMESPACE_REGISTRATION', 16720: 'ACCOUNT_ADDRESS_RESTRICTION', 16721: 'MOSAIC_GLOBAL_RESTRICTION', 16722: 'SECRET_LOCK', 16724: 'TRANSFER', 16725: 'MULTISIG_ACCOUNT_MODIFICATION', 16961: 'AGGREGATE_BONDED', 16963: 'VRF_KEY_LINK', 16964: 'MOSAIC_METADATA', 16972: 'NODE_KEY_LINK', 16973: 'MOSAIC_SUPPLY_CHANGE', 16974: 'ADDRESS_ALIAS', 16976: 'ACCOUNT_MOSAIC_RESTRICTION', 16977: 'MOSAIC_ADDRESS_RESTRICTION', 16978: 'SECRET_PROOF', 17220: 'NAMESPACE_METADATA', 17229: 'MOSAIC_SUPPLY_REVOCATION', 17230: 'MOSAIC_ALIAS'}
```

##### 注意事項
17232: 'ACCOUNT_OPERATION_RESTRICTION' の制限は許可されていません。
つまり、AllowOutgoingTransactionTypeを指定する場合は、ACCOUNT_OPERATION_RESTRICTIONを必ず含める必要があり、
BlockOutgoingTransactionTypeを指定する場合は、ACCOUNT_OPERATION_RESTRICTIONを含めることはできません。


### 確認

設定した制限情報を確認します

```js
$restrictionAipInstance = new RestrictionAccountRoutesApi($client, $config);
$res = $restrictionAipInstance->getAccountRestrictions($carolKey->address);
echo $res . PHP_EOL;
```
###### 出力例
```
{
    "accountRestrictions": {
        "version": 1,
        "address": "98070C726161DEDAC6642EB41583016DC936053BA43E050B",
        "restrictions": [
            {
                "restrictionFlags": 32769,
                "values": [
                    "98B00F753CF2564075CC94721E3BCFC4B99E38E8A3DCBBC7"
                ]
            },
            {
                "restrictionFlags": 32770,
                "values": [
                    "72C0212E67A08BCE"
                ]
            },
            {
                "restrictionFlags": 16388,
                "values": [
                    "17232"
                ]
            }
        ]
    }
}
```

## 11.2 グローバルモザイク制限

グローバルモザイク制限はモザイクに対して送信可能な条件を設定します。
その後、各アカウントに対してグローバルモザイク制限専用の数値メタデータを付与します。
送信アカウント・受信アカウントの両方が条件を満たした場合のみ、該当モザイクを送信することができます。


### グローバル制限機能つきモザイクの作成
restrictableをtrueにしてCarolでモザイクを作成します。

```php

// モザイクフラグ設定
$f = MosaicFlags::NONE;
$f += MosaicFlags::SUPPLY_MUTABLE; // 供給量変更可能
$f += MosaicFlags::TRANSFERABLE; // 第三者への譲渡可否
$f += MosaicFlags::RESTRICTABLE; //制限設定の可否
$f += MosaicFlags::REVOKABLE; //発行者からの還収可否
$flags = new MosaicFlags($f);

$mosaicId = IdGenerator::generateMosaicId($carolKey->address);

// モザイク定義
$mosaicDefTx = new EmbeddedMosaicDefinitionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey, // 署名者公開鍵
  id: new MosaicId($mosaicId['id']), // モザイクID
  divisibility: 2, // 分割可能性
  duration: new BlockDuration(0), //duration:有効期限
  nonce: new MosaicNonce($mosaicId['nonce']),
  flags: $flags,
);

// モザイク変更
$mosaicChangeTx = new EmbeddedMosaicSupplyChangeTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey, // 署名者公開鍵
  mosaicId: new UnresolvedMosaicId($mosaicId['id']),
  delta: new Amount(10000),
  action: new MosaicSupplyChangeAction(MosaicSupplyChangeAction::INCREASE),
);

// キーの値と設定
$keyId = Metadata::metadataGenerateKey("KYC"); // restrictionKey

// グローバルモザイク制限
$mosaicGlobalResTx = new EmbeddedMosaicGlobalRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  mosaicId: new UnresolvedMosaicId($mosaicId['id']),
  restrictionKey: $keyId,
  newRestrictionValue: 1,
  newRestrictionType: new MosaicRestrictionType(MosaicRestrictionType::EQ),
);
// 更新する場合は以下も設定する必要あり
//   - mosaicGlobalResTx.previousRestrictionValue
//   - mosaicGlobalResTx.previousRestrictionType

// マークルハッシュの算出
$embeddedTransactions = [$mosaicDefTx, $mosaicChangeTx, $mosaicGlobalResTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートTx作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100);  // 手数料

// 署名
$sig = $carolKey->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
```

MosaicRestrictionTypeについては以下の通りです。

```js
{0: 'NONE', 1: 'EQ', 2: 'NE', 3: 'LT', 4: 'LE', 5: 'GT', 6: 'GE'}
```

| 演算子  | 略称  | 英語  |
|---|---|---|
| =  | EQ  | equal to  |
| !=  | NE  | not equal to  |
| <  | LT  | less than  |
| <=  | LE  | less than or equal to  |
| >  | GT  | greater than  |
| <=  | GE  | greater than or equal to  |


### アカウントへのモザイク制限適用

Carol,Bobに対してグローバル制限モザイクに対しての適格情報を追加します。  
送信・受信についてかかる制限なので、すでに所有しているモザイクについての制限はありません。  
送信を成功させるためには、送信者・受信者双方が条件をクリアしている必要があります。  
モザイク作成者の秘密鍵があればどのアカウントに対しても承諾の署名を必要とせずに制限をつけることができます。  

```php
// carolに適用
$carolMosaicAddressResTx = new MosaicAddressRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  mosaicId: new UnresolvedMosaicId('0x10FE6A79F72DB356'),
  restrictionKey: $keyId,
  previousRestrictionValue: -1, // 以前のリストリクション値がなく、新規に値を設定する場合
  newRestrictionValue: 1,
  targetAddress: $carolKey->address,
);
$facade->setMaxFee($carolMosaicAddressResTx, 100);


// 署名
$sig = $carolKey->signTransaction($carolMosaicAddressResTx);
$jsonPayload = $facade->attachSignature($carolMosaicAddressResTx, $sig);

try {
  $result = $apiInstance->announceTransaction($jsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}

// bobに適用
$bobMosaicAddressResTx = new MosaicAddressRestrictionTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  mosaicId: new UnresolvedMosaicId('0x10FE6A79F72DB356'),
  restrictionKey: $keyId,
  previousRestrictionValue: -1, // 以前のリストリクション値がなく、新規に値を設定する場合
  newRestrictionValue: 1,
  targetAddress: $bobKey->address,
);
$facade->setMaxFee($bobMosaicAddressResTx, 100);

// 署名
$sig = $carolKey->signTransaction($bobMosaicAddressResTx);
$jsonPayload = $facade->attachSignature($bobMosaicAddressResTx, $sig);

try {
  $result = $apiInstance->announceTransaction($jsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
```

### 制限状態確認

ノードに問い合わせて制限状態を確認します。

```php
$restrictionAipInstance = new RestrictionMosaicRoutesApi($client, $config);
$res = $restrictionAipInstance->searchMosaicRestrictions(
  mosaic_id: '10FE6A79F72DB356'
);
echo 'MosaicRestrictions' . PHP_EOL;
echo $res . PHP_EOL;
```

###### 出力例
```js
{
    "data": [
        {
            "id": "66A5B8E584E82060AFC8F4DD",
            "mosaicRestrictionEntry": {
                "version": 1,
                "compositeHash": "D2F056476C118C5CB1C15978B23603D404CE91629BA2DB82A3D91CEA9D6E9422",
                "entryType": 1,
                "mosaicId": "10FE6A79F72DB356",
                "restrictions": [
                    {
                        "key": "9300605567124626807",
                        "restriction": {
                            "referenceMosaicId": "0000000000000000",
                            "restrictionValue": "1",
                            "restrictionType": 1
                        }
                    }
                ]
            }
        },
        {
            "id": "66A5C05484E82060AFC900C0",
            "mosaicRestrictionEntry": {
                "version": 1,
                "compositeHash": "0E562A82F14DB98C1831B570F3D96FE7939AED5AF110FE54EC995DCE8408629F",
                "entryType": 0,
                "mosaicId": "10FE6A79F72DB356",
                "restrictions": [
                    {
                        "key": "9300605567124626807"
                    }
                ]
            }
        },
        {
            "id": "66A5C0F384E82060AFC90244",
            "mosaicRestrictionEntry": {
                "version": 1,
                "compositeHash": "164CE57733CEEFE0D8E46B05750C1209B04E96696F46204676B631A63BE005A9",
                "entryType": 0,
                "mosaicId": "10FE6A79F72DB356",
                "restrictions": [
                    {
                        "key": "9300605567124626807"
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

### 送信確認

実際にモザイクを送信してみて、制限状態を確認します。

```php
// 成功（CarolからBobに送信）
$tx = new TransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  recipientAddress: $bobKey->address, // 受信者アドレス
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId('0x51E212A3D485C85F'),  // モザイクID
      amount: new Amount(1) // 金額
    )
  ],
  message: '',
);
$facade->setMaxFee($tx, 100);  // 手数料
$sig = $carolKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);
try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
echo 'TxHash' . PHP_EOL;
echo $facade->hashTransaction($tx) . PHP_EOL;

// 失敗（CarolからDaveに送信）
$daveKey = $facade->createAccount(PrivateKey::random());

$tx = new TransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)), // 有効期限
  recipientAddress: $daveKey->address, // 受信者アドレス
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId('0x51E212A3D485C85F'),  // モザイクID
      amount: new Amount(1) // 金額
    )
  ],
  message: '',
);
$facade->setMaxFee($tx, 100);  // 手数料
$sig = $carolKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);
try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ', $e->getMessage(), PHP_EOL;
}
echo 'TxHash' . PHP_EOL;
echo $facade->hashTransaction($tx) . PHP_EOL;
```

失敗した場合以下のようなエラーステータスになります。

```js
{"hash":"C7E0EAF9941D9030E055BE87F0F79805582CA1713680BA546837042EA3DEDBCD","code":"Failure_RestrictionMosaic_Account_Unauthorized","deadline":"54911200734","group":"failed"}
```

## 11.3 現場で使えるヒント

ブロックチェーンの社会実装などを考えたときに、法律や信頼性の見地から
一つの役割のみを持たせたいアカウント、関係ないアカウントを巻き込みたくないと思うことがあります。
そんな場合にアカウント制限とグローバルモザイク制限を使いこなすことで、
モザイクのふるまいを柔軟にコントロールすることができます。

### アカウントバーン

AllowIncomingAddressによって指定アドレスからのみ受信可能にしておいて、
XYMを全量送信すると、秘密鍵を持っていても自力では操作困難なアカウントを明示的に作成することができます。
（最小手数料を0に設定したノードによって承認されることもあり、その可能性はゼロではありません）

### モザイクロック
譲渡不可設定のモザイクを配布し、配布者側のアカウントで受け取り拒否を行うとモザイクをロックさせることができます。

### 所属証明
モザイクの章で所有の証明について説明しました。グローバルモザイク制限を活用することで、
KYCが済んだアカウント間でのみ所有・流通させることが可能なモザイクを作り、所有者のみが所属できる独自経済圏を構築することが可能です。



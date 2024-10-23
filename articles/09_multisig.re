= 9.マルチシグ化

アカウントのマルチシグ化について説明します。

=== 注意事項

一つのマルチシグアカウントに登録できる連署者の数は25個です。一つのアカウントは最大25個のマルチシグの連署者になれます。マルチシグは最大3階層まで構成できます。本書では1階層のマルチシグのみ解説します。

=== 9.0 アカウントの準備

この章のサンプルソースコードで使用するアカウントを作成し、それぞれの秘密鍵を出力しておきます。本章でマルチシグ化したアカウントBobは、Carolの秘密鍵を紛失すると使えなくなってしまうのでご注意ください。

//emlist[][php]{
$bobKey = $facade->createAccount(PrivateKey::random());
$carolKey1 = $facade->createAccount(PrivateKey::random());
$carolKey2 = $facade->createAccount(PrivateKey::random());
$carolKey3 = $facade->createAccount(PrivateKey::random());
$carolKey4 = $facade->createAccount(PrivateKey::random());
$carolKey5 = $facade->createAccount(PrivateKey::random());

echo "===秘密鍵と公開鍵の導出===" . PHP_EOL;
echo $bobKey->keyPair->privateKey() . PHP_EOL;
echo $carolKey1->keyPair->privateKey() . PHP_EOL;
echo $carolKey2->keyPair->privateKey() . PHP_EOL;
echo $carolKey3->keyPair->privateKey() . PHP_EOL;
echo $carolKey4->keyPair->privateKey() . PHP_EOL;
echo $carolKey5->keyPair->privateKey() . PHP_EOL;
//}

テストネットの場合はFAUCETでネットワーク手数料分をbobとcarol1に補給しておきます。

 * Faucet

//beginchild

 * https://testnet.symbol.tools/

//endchild

===== URL出力

//emlist[][php]{
echo "https://testnet.symbol.tools/?recipient="
 . $bobKey->address
 . "&amount=20" . PHP_EOL;
echo "https://testnet.symbol.tools/?recipient="
 . $carolKey1->address
 . "&amount=20" . PHP_EOL;
//}

== 9.1 マルチシグの登録

Symbolではマルチシグアカウントを新規に作成するのではなく、既存アカウントについて連署者を指定してマルチシグ化します。マルチシグ化には連署者に指定されたアカウントの承諾署名(オプトイン)が必要なため、アグリゲートトランザクションを使用します。

//emlist[][php]{
$multisigTx =  new EmbeddedMultisigAccountModificationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,  // マルチシグ化したいアカウントの公開鍵を指定
  minApprovalDelta: 3, // minApproval:承認のために必要な最小署名者数増分
  minRemovalDelta: 3, // minRemoval:除名のために必要な最小署名者数増分
  addressAdditions: [
    $carolKey1->address,
    $carolKey2->address,
    $carolKey3->address,
    $carolKey4->address,
  ],
  addressDeletions: [] // 除名対象アドレスリスト
);

// マークルハッシュの算出
$embeddedTransactions = [$multisigTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートトランザクションの作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,  // マルチシグ化したいアカウントの公開鍵を指定
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 4);  // 手数料

// マルチシグ化したいアカウントによる署名
$sig = $bobKey->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

// 追加・除外対象として指定したアカウントによる連署
$coSig1 = $facade->cosignTransaction($carolKey1->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig1);
$coSig2 = $facade->cosignTransaction($carolKey2->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig2);
$coSig3 = $facade->cosignTransaction($carolKey3->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig3);
$coSig4 = $facade->cosignTransaction($carolKey4->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig4);

// アナウンス
$payload = ["payload" => strtoupper(bin2hex($aggregateTx->serialize()))];

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

== 9.2 確認

=== マルチシグ化したアカウントの確認

//emlist[][js]{
$multisigApiInstance = new MultisigRoutesApi($client, $config);
$multisigInfo = $multisigApiInstance->getAccountMultisig($bobKey->address);
echo "===マルチシグ情報===" . PHP_EOL;
echo $multisigInfo . PHP_EOL;
//}

====== 出力例

//emlist{
{
    "multisig": {
        "version": 1,
        "accountAddress": "98D215F1BB0566C26847D2612F94FD6769384D8C0DE3FAAB",
        "minApproval": 3,
        "minRemoval": 3,
        "cosignatoryAddresses": [
            "98402F5B7B45F3F5BC986FEA1DACE7AA4FEC48143371433C",
            "98429F73132C64A4676BD26B47CC574D68B3FC5E4C3B48A0",
            "9844ACF30F35DED7514DDD8651E718949786143ACE087EA1",
            "98F36944663F9EE70BC30AC39FDEDF6800356B5ED4CAA9B7"
        ],
        "multisigAddresses": []
    }
}
//}

cosignatoryAddressesが連署者として登録されていることがわかります。また、minApproval:3 によりトランザクションが成立するために必要な署名数3 minRemoval: 3により連署者を取り外すために必要な署名者数は3であることがわかります。

=== 連署者アカウントの確認

//emlist[][php]{
$multisigInfo = $multisigApiInstance->getAccountMultisig($carolKey1->address);
echo "===連署者1のマルチシグ情報===" . PHP_EOL;
echo $multisigInfo . PHP_EOL;
//}

====== 出力例

//emlist{
{
    "multisig": {
        "version": 1,
        "accountAddress": "98402F5B7B45F3F5BC986FEA1DACE7AA4FEC48143371433C",
        "minApproval": 0,
        "minRemoval": 0,
        "cosignatoryAddresses": [],
        "multisigAddresses": [
            "98D215F1BB0566C26847D2612F94FD6769384D8C0DE3FAAB"
        ]
    }
}
//}

multisigAddresses に対して連署する権利を持っていることが分かります。

== 9.3 マルチシグ署名

マルチシグ化したアカウントからモザイクを送信します。

=== アグリゲートコンプリートトランザクションで送信

アグリゲートコンプリートトランザクションの場合、ノードにアナウンスする前に連署者の署名を全て集めてからトランザクションを作成します。

//emlist[][php]{
$tx = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,  //マルチシグ化したアカウントの公開鍵
  recipientAddress: $aliceKey->address,
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
      amount: new Amount(1000000) // 金額(1XYM)
    )
  ],
  message: "\0test"
);

// マークルハッシュの算出
$embeddedTransactions = [$tx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートトランザクションの作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey1->publicKey,  // マルチシグ化したいアカウントの公開鍵を指定
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 2);  // 手数料

// 起案者アカウントによる署名
$sig = $carolKey1->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

// 追加・除外対象として指定したアカウントによる連署
$coSig1 = $facade->cosignTransaction($carolKey2->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig1);
$coSig2 = $facade->cosignTransaction($carolKey3->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig2);

// アナウンス
$payload = ["payload" => strtoupper(bin2hex($aggregateTx->serialize()))];

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

=== アグリゲートボンデッドトランザクションで送信

アグリゲートボンデッドトランザクションの場合は連署者を指定せずにアナウンスできます。事前にハッシュロックでトランザクションを留め置きしておくことを宣言しておき、連署者がネットワーク上に留め置きされたトランザクションに追加署名することで完成となります。

//emlist[][php]{
// アグリゲートTxに含めるTxを作成
$tx = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,
  recipientAddress: $aliceKey->address,
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId($namespaceId), // モザイクID
      amount: new Amount(1000000) // 金額(1XYM)
    )
  ],
  message: "\0test"
);

// マークルハッシュの算出
$embeddedTransactions = [$tx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートボンデッドTx作成
$aggregateTx = new AggregateBondedTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey1->publicKey,  // 起案者アカウントの公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 2);  // 手数料

// 署名
$sig = $carolKey1->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

// ハッシュロックTx作成
$hashLockTx = new HashLockTransactionV1(
  signerPublicKey: $carolKey1->publicKey,  // 署名者公開鍵
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
$hashLockSig = $carolKey1->signTransaction($hashLockTx);
$hashLockJsonPayload = $facade->attachSignature($hashLockTx, $hashLockSig);

// ハッシュロックをアナウンス
try {
  $result = $apiInstance->announceTransaction($hashLockJsonPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

//emlist[][php]{
//ハッシュロックの承認を確認した後、ボンデッドTXをアナウンス
try {
  $result = $apiInstance->announcePartialTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

ボンデッドトランザクションがノードに取り込まれるとパーシャル署名状態となるので、8.ロックで紹介した連署を使用して、マルチシグアカウントで連署します。連署をサポートするウォレットで承認することもできます。

連署例）

//emlist[][php]{
/**
 * 連署
 */
// トランザクションの取得
$txInfo = $apiInstance->getPartialTransaction(
    $facade->hashTransaction($aggregateTx)
);


/**
 * carolKey2の連署
 */
$signTxHash = new Hash256($txInfo->getMeta()->getHash());
$signature = new Signature($carolKey2->keyPair->sign($signTxHash->binaryData));
$body = [
    'parentHash' => $signTxHash->__toString(),
    'signature' => $signature->__toString(),
    'signerPublicKey' => $carolKey2->publicKey->__toString(),
    'version' => '0'
];

//アナウンス
try {
  $result = $apiInstance->announceCosignatureTransaction($body);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
echo 'TxHash' . PHP_EOL;
echo $signTxHash->__toString() . PHP_EOL;

sleep(1);

/**
 * carolKey3の連署
 */
$signature = new Signature($carolKey3->keyPair->sign($signTxHash->binaryData));
$body = [
    'parentHash' => $signTxHash->__toString(),
    'signature' => $signature->__toString(),
    'signerPublicKey' => $carolKey3->publicKey->__toString(),
    'version' => '0'
];

//アナウンス
try {
  $result = $apiInstance->announceCosignatureTransaction($body);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
echo 'TxHash' . PHP_EOL;
echo $signTxHash->__toString() . PHP_EOL;
//}

== 9.4 マルチシグ送信の確認

マルチシグで行った送信トランザクションの結果を確認してみます。

//emlist[][php]{
$aggregateHash = $facade->hashTransaction($aggregateTx);
$txInfo = $apiInstance->getConfirmedTransaction($aggregateHash);
console.log(txInfo);
//}

====== 出力例

//emlist{
{
  "id": "66A4BF0F527B051AC20AF327",
  "meta": {
    "height": "1608331",
    "hash": 
      "6122124B72EEE31237F0CBF1A9A133E3D94AAB0C15B8582CCBA7187082E5DAD4",
    "merkleComponentHash": 
      "593B38945C4FF97085686285A409797ADA0EC3C5B167150F898C624E8C98E8AE",
    "index": 0,
    "timestamp": "54822380100",
    "feeMultiplier": 100
  },
  "transaction": {
    "size": 480,
    "signature": 
      "B899E6F18C51927E35126D87803AD560C29FBD9F38A403F6DF7DDAF7E44A9E83\
        CF83555DC3B6D90F93A288DB8083145FB9EDE4F2B4E247ED65A460EC63F53108",
    "signerPublicKey": 
      "8D1516F8F7C8352680C19F578238D8C258A281E9FD66A72A384C10776FD42DE1",
    "version": 2,
    "network": 152,
    "type": 16961,
    "maxFee": "48000",
    "deadline": "54829501706",
    "transactionsHash": 
      "CC4EEE544A2376EF9F7951627971E339638DBEAC4102D9173B4ECD814E6E0EFB",
    "cosignatures": [
      {
        "signature": 
          "3E52A9D398BDD97022F7A69CEA0F38EC0A208E1726372879DD4A3A64209AB551\
            7F7E769B89D4C05BD920325AE461A597E6A4FF04DA82C32008E8086D11670908",
        "version": "0",
        "signerPublicKey": 
          "4AA192AE9F4FD262E489BCA3B79490FB39F61D1CD7206D50F856891CAA3CCAF7"
      },
      {
        "signature": 
          "5B1AA912AEBDDAAAA7DD4EDAD03A29B5A9FEF6CAA52E9DA1961AA6CB0A1CCAD5\
            059DACBCD6380425912F982AD31BD08479BB95EC88C5A8110B389FAFF2DB0A08",
        "version": "0",
        "signerPublicKey": 
          "D1C49DF9CA73E0BAA204EFDF88F714548FD40B2E1D9A8EE31FE98AD7E3C877FD"
      }
    ],
    "transactions": [
      {
        "id": "66A4BF0F527B051AC20AF328",
        "meta": {
          "height": "1608331",
          "aggregateHash": 
            "6122124B72EEE31237F0CBF1A9A133E3D94AAB0C15B8582CCBA7187082E5DAD4",
          "aggregateId": "66A4BF0F527B051AC20AF327",
          "index": 0,
          "timestamp": "54822380100",
          "feeMultiplier": 100
        },
        "transaction": {
          "signerPublicKey": 
            "AD4D43687EFDE2A15CF316FCCB209F80DC07498090725FE9C9C2A239E7B38E39",
          "version": 1,
          "network": 152,
          "type": 16724,
          "recipientAddress": 
            "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
          "mosaics": [
            {
              "id": "E74B99BA41F4AFEE",
              "amount": "1000000"
            }
          ],
          "message": "0074657374"
        }
      }
    ]
  }
}
//}

 * マルチシグアカウント

//beginchild

 * Bob

//beginchild

 * txInfo.transaction.transactions[0].transaction.signerPublicKey

//beginchild

 * AD4D43687EFDE2A15CF316FCCB209F80DC07498090725FE9C9C2A239E7B38E39

//endchild

//endchild

//endchild

 * 起案者アカウント

//beginchild

 * Carol1

//beginchild

 * txInfo.transaction.signerPublicKey

//beginchild

 * 8D1516F8F7C8352680C19F578238D8C258A281E9FD66A72A384C10776FD42DE1

//endchild

//endchild

//endchild

 * 連署者アカウント

//beginchild

 * Carol2

//beginchild

 * xInfo.transaction.cosignatures[0].signerPublicKey

//beginchild

 * 4AA192AE9F4FD262E489BCA3B79490FB39F61D1CD7206D50F856891CAA3CCAF7

//endchild

//endchild
 * Carol3

//beginchild

 * xInfo.transaction.cosignatures[1].signerPublicKey

//beginchild

 * D1C49DF9CA73E0BAA204EFDF88F714548FD40B2E1D9A8EE31FE98AD7E3C877FD

//endchild

//endchild

//endchild

== 9.5 マルチシグ構成変更

=== マルチシグ構成の縮小

連署者を減らすには除名対象アドレスに指定するとともに最小署名者数を連署者数が超えてしまわないように調整してトランザクションをアナウンスします。除名対象者を連署者に含む必要はありません。

//emlist[][php]{
$multisigTx = new EmbeddedMultisigAccountModificationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,  // マルチシグ化したいアカウントの公開鍵を指定
  minApprovalDelta: -1, // minApproval:承認のために必要な最小署名者数増分
  minRemovalDelta: -1, // minRemoval:除名のために必要な最小署名者数増分
  addressAdditions: [], //追加対象アドレスリスト
  addressDeletions: [
    $carolKey3->address,
  ] // 除名対象アドレスリスト
);

// マークルハッシュの算出
$embeddedTransactions = [$multisigTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートトランザクションの作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey1->publicKey,  // 起案者アカウントの公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 2);  // 手数料

// 起案者アカウントによる署名
$sig = $carolKey1->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

// 連署者アカウントによる連署
$coSig1 = $facade->cosignTransaction($carolKey2->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig1);
$coSig4 = $facade->cosignTransaction($carolKey4->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig4);

// アナウンス
$payload = ["payload" => strtoupper(bin2hex($aggregateTx->serialize()))];

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

=== 連署者構成の差替え

連署者を差し替えるには、追加対象アドレスと除名対象アドレスを指定します。新たに追加指定するアカウントの連署は必ず必要です。

//emlist[][php]{
$multisigTx = new EmbeddedMultisigAccountModificationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,  // 構成変更したいマルチシグアカウントの公開鍵を指定
  minApprovalDelta: 0, // minApproval:承認のために必要な最小署名者数増分
  minRemovalDelta: 0, // minRemoval:除名のために必要な最小署名者数増分
  addressAdditions: [
    $carolKey5->address,
  ],
  addressDeletions: [
    $carolKey4->address,
  ] // 除名対象アドレスリスト
);

// マークルハッシュの算出
$embeddedTransactions = [$multisigTx];
$merkleHash = $facade->hashEmbeddedTransactions($embeddedTransactions);

// アグリゲートトランザクションの作成
$aggregateTx = new AggregateCompleteTransactionV2(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $carolKey1->publicKey,  // 起案者アカウントの公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  transactionsHash: $merkleHash,
  transactions: $embeddedTransactions
);
$facade->setMaxFee($aggregateTx, 100, 2);  // 手数料

// 起案者アカウントによる署名
$sig = $carolKey1->signTransaction($aggregateTx);
$payload = $facade->attachSignature($aggregateTx, $sig);

// 連署者アカウントによる連署
$coSig2 = $facade->cosignTransaction($carolKey2->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig2);
$coSig5 = $facade->cosignTransaction($carolKey5->keyPair, $aggregateTx);
array_push($aggregateTx->cosignatures, $coSig5);

// アナウンス
$payload = ["payload" => strtoupper(bin2hex($aggregateTx->serialize()))];

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

== 9.6 現場で使えるヒント

=== 多要素認証

秘密鍵の管理を複数の端末に分散させることができます。セキュリティ用の鍵を用意しておけば、紛失・漏洩時にも安全に回復することができます。また、マルチシグの安全運用については盗難時と紛失時の2パターンを検討しておく必要があるのでご注意ください。- 盗難時：ほかにも秘密鍵を使える人がいる。- 紛失時：だれもその秘密鍵を使えなくなる。

=== アカウントの所有

マルチシグ化したアカウントの秘密鍵は無効化し、マルチシグを解除しない限りたとえ秘密鍵を知っていたとしてもモザイク送信などはできなくなります。モザイクの章で説明した通り、所有を「自分の意思で手放すことができる状態」だとすると、マルチシグ化したアカウントがもつモザイク等の所有者は連署者になります。また、Symbolではマルチシグの構成変更が可能ですのでアカウントの所有を他の連署者に安全に移転することができます。

=== ワークフロー

Symbolではマルチシグを3階層まで構成することができます(マルチレベルマルチシグ)。マルチレベルマルチシグを用いることで、バックアップ鍵を不正に持ち出して連署を完成させたり、承認者と監査役だけで署名を完成させるといったことを防ぐことができます。これによって、ブロックチェーン上にトランザクションが存在することが現実社会のワークフローなどの条件を満たした証拠として提示することができます。

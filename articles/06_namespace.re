= ネームスペース

Symbolブロックチェーンではネームスペースをレンタルしてアドレスやモザイクに視認性の高い単語をリンクさせることができます。ネームスペースは最大64文字、利用可能な文字は a, b, c, …, z, 0, 1, 2, …, 9, _ , - です。

== 手数料の計算

ネームスペースのレンタルにはネットワーク手数料とは別にレンタル手数料が発生します。ネットワークの活性度に比例して価格が変動しますので、取得前に確認するようにしてください。

ルートネームスペースを365日レンタルする場合の手数料を計算します。

//emlist[][php]{
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();

$networkApiInstance = new NetworkRoutesApi($client, $config);
$rootNsperBlock = $networkApiInstance->getRentalFees()
    ->getEffectiveRootNamespaceRentalFeePerBlock();
$rentalDays = 365;
$rentalBlock = ($rentalDays * 24 * 60 * 60) / 30;
$rootNsRenatalFeeTotal = $rentalBlock * $rootNsperBlock;
echo "rentalBlock: " . $rentalBlock . PHP_EOL;
echo "Root Namespace Rental Fee: " . $rootNsRenatalFeeTotal . PHP_EOL;
//}

====== 出力例

//emlist{
rentalBlock: 1051200
Root Namespace Rental Fee: 210240000 //約210XYM
//}

期間はブロック数で指定します。1ブロックを30秒として計算しました。最低で30日分はレンタルする必要があります（最大で1825日分）。

サブネームスペースの取得手数料を計算します。

//emlist[][php]{
$childNamespaceRentalFee = $networkApiInstance->getRentalFees()
    ->getEffectiveChildNamespaceRentalFee();
echo "Child Namespace Rental Fee: " . $childNamespaceRentalFee . PHP_EOL;
//}

====== 出力例

//emlist[][js]{
Child Namespace Rental Fee: 10000000 //10XYM
//}

サブネームスペースに期間指定はありません。ルートネームスペースをレンタルしている限り使用できます。

== レンタル

ルートネームスペースをレンタルします(例:xembook)

//emlist[][php]{
$name = "xembook";
$tx = new NamespaceRegistrationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  duration: new BlockDuration(86400), // 有効期限
  id: new NamespaceId(IdGenerator::generateNamespaceId($name)), //必須
  name: $name,
);
$facade->setMaxFee($tx, 100);
// 署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

// アナウンス
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
//}

サブネームスペースをレンタルします(例:xembook.tomato)

//emlist[][php]{
$parnetNameId = IdGenerator::generateNamespaceId("xembook"); //ルートネームスペース名
$name = "tomato"; //サブネームスペース名

// Tx作成
$tx = new NamespaceRegistrationTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,  // 署名者公開鍵
  deadline: new Timestamp($facade->now()->addHours(2)),
  duration: new BlockDuration(86400), // 有効期限
  parentId: new NamespaceId($parnetNameId),
  id: new NamespaceId(IdGenerator::generateNamespaceId($name, $parnetNameId)),
  registrationType: new NamespaceRegistrationType(
    NamespaceRegistrationType::CHILD
  ),
  name: $name,
);
$facade->setMaxFee($tx, 200);

// 署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

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
//}

2階層目のサブネームスペースを作成したい場合は例えば、xembook.tomato.morningを定義したい場合は以下のようにします。

//emlist[][php]{
$rootName = IdGenerator::generateNamespaceId("xembook"); //ルートネームスペース名
$parnetNameId = IdGenerator::generateNamespaceId("tomato", $rootName); // 紐づけたい1階層目のサブネームスペース
$name = "morning"; //サブネームスペース名
// 以下はサブネームスペース作成と同じ
//}

=== 有効期限の計算

レンタル済みルートネームスペースの有効期限を計算します。

//emlist[][php]{
$namespaceIds = IdGenerator::generateNamespacePath("xembook"); // ルートネームスペース
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);

$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();
$namespaceApiInstance = new NamespaceRoutesApi($client, $config);
try {
  $nsInfo = $namespaceApiInstance->getNamespace(substr($namespaceId, 2));
  // echo $nsInfo['namespace']. PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}

$chainApiInstance = new ChainRoutesApi($client, $config);
try {
  $chainInfo = $chainApiInstance->getChainInfo(substr($namespaceId, 2));
  // echo $chainInfo . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
$lastHeight = (int)$chainInfo['height'];

$blockApiInstance = new BlockRoutesApi($client, $config);
try {
  $lastBlock = $blockApiInstance->getBlockByHeight($lastHeight);
  // echo $lastBlock . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
$remainHeight = (int)$nsInfo['namespace']['end_height'] - $lastHeight;

$endDate = Carbon::createFromTimestampMs((int)$lastBlock['block']['timestamp'] +
    $remainHeight * 30000 + $epochAdjustment * 1000);
echo "End Date: " . $endDate . PHP_EOL;
//}

ネームスペース情報の終了ブロックを取得し、現在のブロック高から差し引いた残ブロック数に30秒(平均ブロック生成間隔)を掛け合わせた日時を出力します。テストネットでは設定した有効期限よりも1日程度更新期限が猶予されます。メインネットはこの値が30日となっていますのでご留意ください

====== 出力例

//emlist{
End Date: 2024-09-22 04:02:26
//}

== リンク

=== アカウントへのリンク

//emlist[][php]{
$namespaceId = IdGenerator::generateNamespaceId("xembook"); // ルートネームスペース
$address = $aliceKey->address;

//Tx作成
$tx = new AddressAliasTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  namespaceId: new NamespaceId($namespaceId),
  address: new Address($address),
  aliasAction: new AliasAction(AliasAction::LINK),
);
$facade->setMaxFee($tx, 100);

//署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

リンク先のアドレスは自分が所有していなくても問題ありません。

=== モザイクへリンク

//emlist[][php]{
$namespaceIds = IdGenerator::generateNamespacePath("xembook.tomato"); // ルートネームスペース
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);
$mosaicId = new MosaicId("0x12679808DC2xxxx");

//Tx作成
$tx = new MosaicAliasTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  namespaceId: new NamespaceId($namespaceId),
  mosaicId: $mosaicId,
  aliasAction: new AliasAction(AliasAction::LINK),
);
$facade->setMaxFee($tx, 100);

//署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

モザイクを作成したアドレスと同一の場合のみリンクできるようです。

== 未解決で使用

送信先にUnresolvedAccountとして指定して、アドレスを特定しないままトランザクションを署名・アナウンスします。 チェーン側で解決されたアカウントに対しての送信が実施されます。

v3 ではネームスペースを直接指定できないため、アドレスを特定しないまま操作する場合はデータを加工する必要があります。

//emlist[][php]{

// UnresolvedAccount 導出
$namespaceId = IdGenerator::generateNamespaceId("xembook"); // ルートネームスペース
$address = Address::fromNamespaceId(
    new NamespaceId($namespaceId),
    $facade->network->identifier
);

// Tx作成
$tx = new TransferTransactionV1(
  signerPublicKey: $aliceKey->publicKey,
  network: new NetworkType($networkType),
  deadline: new Timestamp($facade->now()->addHours(2)),
  recipientAddress: new UnresolvedAddress($unresolvedAccount),
  message: ''
);
$facade->setMaxFee($tx, 100);

//署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}

//}

送信モザイクにUnresolvedMosaicとして指定して、モザイクIDを特定しないままトランザクションを署名・アナウンスします。

//emlist[][php]{
$namespaceIds = IdGenerator::generateNamespacePath("xembook.tomato"); // ルートネームスペース
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);

$tx = new TransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  recipientAddress: $aliceKey->address,
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId($namespaceId),
      amount: new Amount(100)
    ),
  ],
);
$facade->setMaxFee($tx, 100);

//署名
$sig = $aliceKey->signTransaction($tx);
$payload = $facade->attachSignature($tx, $sig);

$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

XYMをネームスペースで使用する場合は以下のように指定します。

//emlist[][php]{
$namespaceIds = IdGenerator::generateNamespacePath("symbol.xym");
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);
var_dump($namespaceId);
//}

//emlist[][php]{
object(SymbolSdk\Symbol\Models\NamespaceId)#101 (2) {
  ["size"]=>
  int(8)
  ["value"]=>
  int(-1780160202445377554)
}
//}

== 参照

アドレスへリンクしたネームスペースの参照します

//emlist[][php]{
$namespaceId = new NamespaceId(IdGenerator::generateNamespaceId("xembook"));
$namespaceInfo = $namespaceApiInstance->getNamespace(substr($namespaceId, 2));
var_dump($namespaceInfo);
//}

====== 出力例

//emlist[][php]{
object(SymbolRestClient\Model\NamespaceInfoDTO)#124 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(3) {
    ["id"]=>
    string(24) "66A048C3527B051AC20A9E78"
    ["meta"]=>
    object(SymbolRestClient\Model\NamespaceMetaDTO)#129 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(2) {
        ["active"]=>
        bool(true)
        ["index"]=>
        int(1)
      }
    }
    ["namespace"]=>
    object(SymbolRestClient\Model\NamespaceDTO)#120 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(11) {
        ["version"]=>
        int(1)
        ["registration_type"]=>
        int(0)
        ["depth"]=>
        int(1)
        ["level0"]=>
        string(16) "E6707B3A003BDDD3"
        ["level1"]=>
        NULL
        ["level2"]=>
        NULL
        ["alias"]=>
        object(SymbolRestClient\Model\AliasDTO)#132 (2) {
          ["openAPINullablesSetToNull":protected]=>
          array(0) {
          }
          ["container":protected]=>
          array(3) {
            ["type"]=>
            int(2)
            ["mosaic_id"]=>
            NULL
            ["address"]=>
            string(48) "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0"
          }
        }
        ["parent_id"]=>
        string(16) "0000000000000000"
        ["owner_address"]=>
        string(48) "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0"
        ["start_height"]=>
        string(7) "1597929"
        ["end_height"]=>
        string(7) "1773609"
      }
    }
  }
}
//}

AliasTypeは以下の通りです。

//emlist[][js]{
{0: 'None', 1: 'Mosaic', 2: 'Address'}
//}

NamespaceRegistrationTypeは以下の通りです。

//emlist[][js]{
{0: 'RootNamespace', 1: 'SubNamespace'}
//}

モザイクへリンクしたネームスペースを参照します。

//emlist[][php]{
$namespaceIds = IdGenerator::generateNamespacePath("xembook.tomato");
$namespaceId = new NamespaceId($namespaceIds[count($namespaceIds) - 1]);
$namespaceInfo = $namespaceApiInstance->getNamespace(substr($namespaceId, 2));
var_dump($namespaceInfo);
//}

====== 出力例

//emlist[][php]{
object(SymbolRestClient\Model\NamespaceInfoDTO)#104 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(3) {
    ["id"]=>
    string(24) "66A048C3527B051AC20A9E7D"
    ["meta"]=>
    object(SymbolRestClient\Model\NamespaceMetaDTO)#133 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(2) {
        ["active"]=>
        bool(true)
        ["index"]=>
        int(1)
      }
    }
    ["namespace"]=>
    object(SymbolRestClient\Model\NamespaceDTO)#128 (2) {
      ["openAPINullablesSetToNull":protected]=>
      array(0) {
      }
      ["container":protected]=>
      array(11) {
        ["version"]=>
        int(1)
        ["registration_type"]=>
        int(1)
        ["depth"]=>
        int(2)
        ["level0"]=>
        string(16) "E6707B3A003BDDD3"
        ["level1"]=>
        string(16) "9EFE1CF171B6C81E"
        ["level2"]=>
        NULL
        ["alias"]=>
        object(SymbolRestClient\Model\AliasDTO)#136 (2) {
          ["openAPINullablesSetToNull":protected]=>
          array(0) {
          }
          ["container":protected]=>
          array(3) {
            ["type"]=>
            int(1)
            ["mosaic_id"]=>
            string(16) "12679808DC2A1493"
            ["address"]=>
            NULL
          }
        }
        ["parent_id"]=>
        string(16) "E6707B3A003BDDD3"
        ["owner_address"]=>
        string(48) "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0"
        ["start_height"]=>
        string(7) "1597929"
        ["end_height"]=>
        string(7) "1773609"
      }
    }
  }
}
//}

=== 逆引き

アドレスに紐づけられたネームスペースを全て調べます。

//emlist[][php]{
$addresses = ["addresses"=> ["TBIL6D6RURP45YQRWV6Q7YVWIIPLQGLZQFHWFEQ"]];
$accountNames = $namespaceApiInstance->getAccountsNames($addresses);
var_dump($accountNames);
//}

モザイクに紐づけられたネームスペースを全て調べます。

//emlist[][php]{
$mosaicIds = ["mosaicIds"=> ["72C0212E67A08BCE"]];
$mosaicNames = $namespaceApiInstance->getMosaicsNames($mosaicIds);
var_dump($mosaicNames);
//}

=== レシートの参照

トランザクションに使用されたネームスペースをブロックチェーン側がどう解決したかを確認します。

アドレスの場合

//emlist[][php]{
$receiptApiInstance = new ReceiptRoutesApi($client, $config);

$state = $receiptApiInstance->searchAddressResolutionStatements(
  height: 1600481
);
echo $state;
//}

====== 出力例

//emlist{
{
    "data": [
        {
            "id": "66A07563527B051AC20AA1FE",
            "meta": {
                "timestamp": "54541377011"
            },
            "statement": {
                "height": "1600481",
                "unresolved": "99D3DD3B003A7B70E6000000000000000000000000000000",
                "resolutionEntries": [
                    {
                        "source": {
                            "primaryId": 1,
                            "secondaryId": 0
                        },
                        "resolved": "98E521BD0F024F58E670A023BF3A14F3\
                                      BECAF0280396BED0"
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
//}

//emlist{
http://sym-test-03.opening-line.jp:3000/statements/resolutions/address?height=1600481
//}

モザイクの場合

//emlist[][php]{
$state = $receiptApiInstance->searchMosaicResolutionStatements(
  height: 1601155
);
echo $state;
//}

//emlist[][php]{
object(stdClass)#142 (2) {
  ["data"]=>
  array(1) {
    [0]=>
    object(stdClass)#139 (3) {
      ["statement"]=>
      object(stdClass)#143 (3) {
        ["height"]=>
        string(7) "1600481"
        ["unresolved"]=>
        string(48) "99D3DD3B003A7B70E6000000000000000000000000000000"
        ["resolutionEntries"]=>
        array(1) {
          [0]=>
          object(stdClass)#144 (2) {
            ["source"]=>
            object(stdClass)#140 (2) {
              ["primaryId"]=>
              int(1)
              ["secondaryId"]=>
              int(0)
            }
            ["resolved"]=>
            string(48) "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0"
          }
        }
      }
      ["id"]=>
      string(24) "66A07563527B051AC20AA1FE"
      ["meta"]=>
      object(stdClass)#95 (1) {
        ["timestamp"]=>
        string(11) "54541377011"
      }
    }
  }
  ["pagination"]=>
  object(stdClass)#124 (2) {
    ["pageNumber"]=>
    int(1)
    ["pageSize"]=>
    int(10)
  }
}
{
    "data": [
        {
            "id": "66A0D267527B051AC20AA8AB",
            "meta": {
                "timestamp": "54565187345"
            },
            "statement": {
                "height": "1601155",
                "unresolved": {},
                "resolutionEntries": [
                    {
                        "source": {
                            "primaryId": 1,
                            "secondaryId": 0
                        },
                        "resolved": {}
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
//}

==== 注意事項

ネームスペースはレンタル制のため、過去のトランザクションで使用したネームスペースのリンク先と 現在のネームスペースのリンク先が異なる可能性があります。 過去のデータを参照する際などに、その時どのアカウントにリンクしていたかなどを知りたい場合は 必ずレシートを参照するようにしてください。

== 現場で使えるヒント

=== 外部ドメインとの相互リンク

ネームスペースは重複取得がプロトコル上制限されているため、 インターネットドメインや実世界で周知されている商標名と同一のネームスペースを取得し、 外部(公式サイトや印刷物など)からネームスペース存在の認知を公表することで、 Symbol上のアカウントのブランド価値を構築することができます (法的な効力については調整が必要です)。 外部ドメイン側のハッキングあるいは、Symbol側でのネームスペース更新忘れにはご注意ください。

@<i>{ネームスペースを取得するアカウントについての注意}ネームスペースはレンタル期限という概念をもつ機能です。 今のところ、取得したネームスペースは放棄か延長の選択肢しかありません。 運用譲渡などが発生する可能性のあるシステムでネームスペース活用を検討する場合は マルチシグ化(9章)したアカウントでネームスペースを取得することをおすすめします。

==== ネームスペースを取得するアカウントについての注意

ネームスペースはレンタル期限という概念をもつ機能です。今のところ、取得したネームスペースは放棄か延長の選択肢しかありません。運用譲渡などが発生する可能性のあるシステムでネームスペース活用を検討する場合はマルチシグ化(9章)したアカウントでネームスペースを取得することをおすすめします。

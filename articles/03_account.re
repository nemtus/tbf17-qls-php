= 3.アカウント

アカウントは秘密鍵に紐づく情報が記録されたデータ構造体です。アカウントと関連づいた秘密鍵を使って署名することでのみブロックチェーンのデータを更新することができます。

== 3.1 アカウント生成

アカウントには秘密鍵と公開鍵をセットにしたキーペア、アドレスなどの情報が含まれています。まずはランダムにアカウントを作成して、それらの情報を確認してみましょう。

=== 新規生成

以下の手順で秘密鍵を作成し、秘密鍵より公開鍵を導出します。

//emlist[][php]{
$aliceKey = $facade->createAccount(PrivateKey::random());
var_dump($aliceKey);
//}

====== 出力例

//emlist[][php]{
object(SymbolSdk\Symbol\SymbolAccount)#29 (4) {
  ["keyPair"]=>
  object(SymbolSdk\Symbol\KeyPair)#27 (2) {
    ["_privateKey":"SymbolSdk\Symbol\KeyPair":private]=>
    object(SymbolSdk\CryptoTypes\PrivateKey)#39 (1) {
      ["binaryData"]=>
      string(32) "a �
���J}�i�h�H�E1=���|��nB�"
    }
    ["_keyPair":"SymbolSdk\Symbol\KeyPair":private]=>
    array(2) {
      ["publicKey"]=>
      string(32) "v������z�|���f�*���%�43�1�S�"
      ["secretKey"]=>
      string(64) "a �
���J}�i�h�H�E1=���|��nB�v������z�|���f�*���%�43�1�S�"
    }
  }
  ["_facade":protected]=>
  object(SymbolSdk\Facade\SymbolFacade)#35 (1) {
    ["network"]=>
    object(SymbolSdk\Symbol\Network)#43 (8) {
      ["generationHashSeed"]=>
      object(SymbolSdk\CryptoTypes\Hash256)#41 (1) {
        ["binaryData"]=>
        string(32) "I���'j����#I�̣�0.z�T��"yIO�e�"
      }
      ["name"]=>
      string(7) "testnet"
      ["identifier"]=>
      int(152)
      ["datetimeConverter"]=>
      object(SymbolSdk\Network\NetworkTimestampDatetimeConverter)#37 (2) {
        ["epoch"]=>
        object(DateTime)#42 (3) {
          ["date"]=>
          string(26) "2022-10-31 21:07:47.000000"
          ["timezone_type"]=>
          int(2)
          ["timezone"]=>
          string(1) "Z"
        }
        ["timeUnits"]=>
        int(1)
      }
      ["_addressHasher":protected]=>
      string(8) "sha3-256"
      ["_createAddress":protected]=>
      object(Closure)#38 (2) {
        ["this"]=>
        *RECURSION*
        ["parameter"]=>
        array(2) {
          ["$addressWithoutChecksum"]=>
          string(10) "<required>"
          ["$checksum"]=>
          string(10) "<required>"
        }
      }
      ["_addressClass":protected]=>
      object(ReflectionClass)#21 (1) {
        ["name"]=>
        string(24) "SymbolSdk\Symbol\Address"
      }
      ["networkTimestampClass"]=>
      object(ReflectionClass)#40 (1) {
        ["name"]=>
        string(33) "SymbolSdk\Symbol\NetworkTimestamp"
      }
    }
  }
  ["publicKey"]=>
  object(SymbolSdk\Symbol\Models\PublicKey)#65 (1) {
    ["binaryData"]=>
    string(32) "v������z�|���f�*���%�43�1�S�"
  }
  ["address"]=>
  object(SymbolSdk\Symbol\Models\UnresolvedAddress)#66 (1) {
    ["binaryData"]=>
    string(24) "��@b����3
���-��CM�3p�"
  }
}
//}

=== 秘密鍵と公開鍵の導出

//emlist[][php]{
echo  $aliceKey->publicKey. PHP_EOL;
echo  $aliceKey->keyPair->privateKey(). PHP_EOL;
//}

//emlist{
> 85B1B06DD5EE2A611325287705FA909442969B3C7FF47672B1EC34E9C*******
> EA648F781721F5094D77149886F18F6B2C2F621B2A3FB8CFA1E382B9E011DFF3
//}

==== 注意事項

秘密鍵を紛失するとそのアカウントに紐づけられたデータを操作することが出来なくなります。また、他人は知らないという秘密鍵の性質を利用してデータ操作の署名を行うので、秘密鍵を他人に教えてはいけません。組織のなかで秘密鍵を譲り受けて運用を続けるといった行為も控えましょう。 一般的な Web サービスでは「アカウント ID」に対してパスワードが割り振られるため、パスワードの変更が可能ですが、ブロックチェーンではパスワードにあたる秘密鍵に対して一意に決まる ID(アドレス)が割り振られるため、アカウントに紐づく秘密鍵を変更するということはできません。

=== アドレスの導出

//emlist[][php]{
$aliceRawAddress = $aliceKey->address;
echo $aliceRawAddress . PHP_EOL;
//}

//emlist{
> TCTEAYWL2X5PQMYKZD26SLPWUZBRX3KNWMZXBFY
//}

これらがブロックチェーンを操作するための最も基本的な情報となります。また、秘密鍵からアカウントを生成したり、公開鍵やアドレスのみを扱うクラスの生成方法も確認しておきましょう。

=== 秘密鍵からアカウント生成

//emlist[][php]{

$aliceKey = $facade->createAccount(
    new PrivateKey(
        '85B1B06DD5EE2A611325287705FA909442969B3C7FF47672B1EC34E9C*******'
    )
);

$aliceRawAddress = $aliceKey->address;
echo $aliceRawAddress . PHP_EOL;
//}

//emlist{
>TCTEAYWL2X5PQMYKZD26SLPWUZBRX3KNWMZXBFY
//}

=== 公開鍵クラスの生成

//emlist[][php]{
$alicePublicAccount = $facade->createPublicAccount(
    new PublicKey(
        'EA648F781721F5094D77149886F18F6B2C2F621B2A3FB8CFA1E382B9E011DFF3'
    )
);

var_dump($alicePublicAccount->address);
echo substr($alicePublicAccount->publicKey, 2, 66) . PHP_EOL;
//}

====== 出力例

//emlist[][php]{
object(SymbolSdk\Symbol\Models\UnresolvedAddress)#68 (1) {
  ["binaryData"]=>
  string(24) "�o��dOv|����h!峯oZ4~\�"
}
EA648F781721F5094D77149886F18F6B2C2F621B2A3FB8CFA1E382B9E011DFF3
//}

=== アドレスクラスの生成

//emlist[][php]{
$aliceAddress = new Address('TDNX2EJJKC3C4QOGURQZLQS6KJLQACTGWYZFKTQ');
echo $aliceAddress->__tostring() . PHP_EOL;

//}

====== 出力例

//emlist[][php]{
TDSSDPIPAJHVRZTQUAR36OQU6O7MV4BIAOLL5UA
//}

== 3.2 アカウントへの送信

アカウントを作成しただけでは、ブロックチェーンにデータを送信することはできません。パブリックブロックチェーンはリソースを有効活用するためにデータ送信時に手数料を要求します。Symbol ブロックチェーンでは、この手数料を XYM という共通トークンで支払うことになります。アカウントを生成したら、この後の章から説明するトランザクションを実行するために必要な手数料を送信しておきます。

=== フォーセットから送信

テストネットではフォーセット（蛇口）サービスから検証用のXYMを入手することができます。メインネットの場合は取引所などでXYMを購入するか、投げ銭サービス(QUEST)などを利用して寄付を募りましょう。

テストネット- FAUCET(蛇口) - https://testnet.symbol.tools/

メインネット- QUEST - https://quest-bc.com/

=== エクスプローラーで確認

フォーセットから作成したアカウントへ送信が成功したらエクスプローラーで確認してみましょう。

 * テストネット

//beginchild

 * https://testnet.symbol.fyi/

//endchild

 * メインネット

//beginchild

 * https://symbol.fyi/

//endchild

== 3.3 アカウント情報の確認

ノードに保存されているアカウント情報を取得します。

=== 所有モザイク一覧の取得

//emlist[][php]{
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new \GuzzleHttp\Client();
$accountApiInstance = new AccountRoutesApi($client, $config);

$account = $accountApiInstance->getAccountInfo($aliceAddress);

echo "\n===アカウント情報の確認===" . PHP_EOL;
echo $account . PHP_EOL;
//}

====== 出力例

//emlist[][php]{
{
    "id": "669B74CB84E82060AFB8C21B",
    "account": {
        "version": 1,
        "address": "98DB7D112950B62E41C6A46195C25E5257000A66B632554E",
        "addressHeight": "1591329",
        "publicKey":
            "0000000000000000000000000000000000000000000000000000000000000000",
        "publicKeyHeight": "0",
        "accountType": 0,
        "supplementalPublicKeys": {},
        "activityBuckets": [],
        "mosaics": [
            {
                "id": "72C0212E67A08BCE",
                "amount": "1000000000"
            }
        ],
        "importance": "0",
        "importanceHeight": "0"
    }
}
//}

==== publicKey

クライアント側で作成しただけで、ブロックチェーンでまだ利用されていないアカウント情報は記録されていません。宛先として指定されて受信することで初めてアカウント情報が記録され、署名したトランザクションを送信することで公開鍵の情報が記録されます。そのため、publicKeyは現在@<tt>{00000...}表記となっています。

== 3.4 現場で使えるヒント

=== 暗号化と署名

アカウントとして生成した秘密鍵や公開鍵は、そのまま従来の暗号化や電子署名として活用することができます。信頼性に問題点があるアプリケーションを使用する必要がある場合も、個人間（エンドツーエンド）でデータの秘匿性・正当性を検証することができます。

==== 事前準備：対話のためのBobアカウントを生成

//emlist[][php]{
$bobKey = new KeyPair(PrivateKey::random());
//}

==== 暗号化

Aliceの秘密鍵・Bobの公開鍵で暗号化し、Aliceの公開鍵・Bobの秘密鍵で復号します（AES-GCM形式）。

//emlist[][php]{
$bobKey = new KeyPair(PrivateKey::random());

$message = "Hello Symbol!";
$encryptedMessage = $aliceKey->messageEncoder()
    ->encode(
        $bobKey->publicKey(),
        $message
);
$encryptedMessage = $aliceMesgEncoder->encode(
    $bobKey->publicKey(),
    $message
);
echo strtoupper(bin2hex($encryptedMessage)) . PHP_EOL;
//}

//emlist{
> "0105949FB22EF11566D1E7F3EED230CE73D93D71BFD1AAE19D78315FD4F80"
."28D1D16AC223A8A7E9D5AAB"
//}

==== 復号

//emlist[][php]{
$bobMsgEncoder = new MessageEncoder($bobKey);
$decryptMessageData = $bobMsgEncoder->tryDecode(
    $aliceKey->keyPair->publicKey(),
    $encryptedMessage
);
var_dump($decryptMessageData);
if($decryptMessageData['isDecoded']){
    echo "\nDecoded message: " . PHP_EOL;
    echo $decryptMessageData["message"] . PHP_EOL;
}else{
    echo "\nFailed to decode message" . PHP_EOL;
}
//}

//emlist{
array(2) {
  ["isDecoded"]=>
  bool(true)
  ["message"]=>
  string(13) "Hello Symbol!"
}
> "Hello Symol!"
//}

==== 署名

Aliceの秘密鍵でメッセージを署名し、Aliceの公開鍵と署名でメッセージを検証します。

//emlist[][php]{
$payload = "Hellow Symbol!";
$signature = $aliceKey->keyPair->sign($payload);
echo "\n===署名===" . PHP_EOL;
echo $signature . PHP_EOL;
//}

//emlist{
> "0x1D37BA4A84CF6161A3659038040BD446AC531F116083847E73795DD7188A512A"
."E03BF5326991A81A5D6A0308662242BD77E767AF8F9B3CCC401CCE765AF17C03"
//}

==== 検証

//emlist[][php]{
$v = new Verifier($aliceKey->keyPair->publicKey());
$isVerified = $v->verify($payload, $signature);
echo "alice verified: " . PHP_EOL;
var_dump($isVerified);

$bobKey = new KeyPair(PrivateKey::random());
$v = new Verifier($bobKey->publicKey());
$isVerified = $v->verify($payload, $signature);
echo "bob verified: " . PHP_EOL;
var_dump($isVerified);
//}

//emlist{
alice verified:
bool(true)
bob verified:
bool(false)
//}

ブロックチェーンを使用しない署名は何度も再利用される可能性があることにご注意ください。

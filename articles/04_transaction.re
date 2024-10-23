= トランザクション

ブロックチェーン上のデータ更新はトランザクションをネットワークにアナウンスすることによって行います。

== トランザクションのライフサイクル

トランザクションを作成してから、改ざんが困難なデータとなるまでを順に説明します。

 * トランザクション作成

//beginchild

 * ブロックチェーンが受理できるフォーマットでトランザクションを作成します。

//endchild

 * 署名

//beginchild

 * アカウントの秘密鍵でトランザクションを署名します。

//endchild

 * アナウンス

//beginchild

 * 任意のノードに署名済みトランザクションを通知します。

//endchild

 * 未承認トランザクション

//beginchild

 * ノードに受理されたトランザクションは、未承認トランザクションとして全ノードに伝播します

//beginchild

 * トランザクションに設定した最大手数料が、各ノード毎に設定されている最低手数料を満たさない場合はそのノードへは伝播しません。

//endchild

//endchild
 * 承認済みトランザクション

//beginchild

 * 約30秒に1度ごとに生成されるブロックに未承認トランザクションが取り込まれると、承認済みトランザクションとなります。

//endchild

 * ロールバック

//beginchild

 * ノード間の合意に達することができずロールバックされたブロックに含まれていたトランザクションは、未承認トランザクションに差し戻されます。

//beginchild

 * 有効期限切れや、キャッシュからあふれたトランザクションは切り捨てられます。

//endchild

//endchild
 * ファイナライズ

//beginchild

 * 投票ノードによるファイナライズプロセスによりブロックが確定するとトランザクションはロールバック不可なデータとして扱うことができます。

//endchild

=== ブロックとは

ブロックは約30秒ごとに生成され、高い手数料を支払ったトランザクションから優先に取り込まれ、ブロック単位で他のノードと同期します。同期に失敗するとロールバックして、ネットワークが全体で合意が取れるまでこの作業を繰り返します。

== トランザクション作成

まずは最も基本的な転送トランザクションを作成してみます。

=== Bobへの転送トランザクション

送信先のBobアドレスを作成しておきます。

//emlist[][php]{
$bobKey = $facade->createAccount(PrivateKey::random());
echo $bobKey->address . PHP_EOL;
//}

//emlist{
> TDCTNJQT27NMIMGAQYWBUG6MCLBXYI2OOPVZ76I
//}

トランザクションを作成します。

//emlist[][php]{
// aliceの秘密鍵からアカウント生成
$aliceKey = $facade->createAccount(new PrivateKey($alicePrivateKey));

$messageData = "\0hello, symbol!";
$tx = new TransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  deadline: new Timestamp($facade->now()->addHours(2)),
  recipientAddress: $bob->address,
  mosaics: [
    new UnresolvedMosaic(
      mosaicId: new UnresolvedMosaicId('0x72C0212E67A08BCE'),
      amount: new Amount(1000000)
    )
  ],
  message: $messageData
);

$facade->setMaxFee($transferTransaction, 100);  // 手数料
var_dump($tx);
//}

//emlist[][php]{
object(SymbolSdk\Symbol\Models\TransferTransactionV1)#71 (14) {
  ["recipientAddress"]=>
  object(SymbolSdk\Symbol\Models\UnresolvedAddress)#68 (1) {
    ["binaryData"]=>
    string(24) "��!�OX�p�#�:���(���"
  }
  ["mosaics"]=>
  array(1) {
    [0]=>
    object(SymbolSdk\Symbol\Models\UnresolvedMosaic)#76 (2) {
      ["mosaicId"]=>
      object(SymbolSdk\Symbol\Models\UnresolvedMosaicId)#77 (2) {
        ["size"]=>
        int(8)
        ["value"]=>
        int(8268645399043017678)
      }
      ["amount"]=>
      object(SymbolSdk\Symbol\Models\Amount)#78 (2) {
        ["size"]=>
        int(8)
        ["value"]=>
        int(1000000)
      }
    }
  }
  ["message"]=>
  string(15) "hello, symbol!"
  ["transferTransactionBodyReserved_1":
    "SymbolSdk\Symbol\Models\TransferTransactionV1":private]=>
  int(0)
  ["transferTransactionBodyReserved_2":
    "SymbolSdk\Symbol\Models\TransferTransactionV1":private]=>
  int(0)
  ["signature"]=>
  object(SymbolSdk\Symbol\Models\Signature)#80 (1) {
    ["binaryData"]=>
    string(64) ""
  }
  ["signerPublicKey"]=>
  object(SymbolSdk\Symbol\Models\PublicKey)#73 (1) {
    ["binaryData"]=>
    string(32) "%�5�#ܼ�ez4�����JmEr�k�OQM���"
  }
  ["version"]=>
  int(1)
  ["network"]=>
  object(SymbolSdk\Symbol\Models\NetworkType)#75 (1) {
    ["value"]=>
    int(152)
  }
  ["type"]=>
  object(SymbolSdk\Symbol\Models\TransactionType)#79 (1) {
    ["value"]=>
    int(16724)
  }
  ["fee"]=>
  object(SymbolSdk\Symbol\Models\Amount)#81 (2) {
    ["size"]=>
    int(8)
    ["value"]=>
    int(19100)
  }
  ["deadline"]=>
  object(SymbolSdk\Symbol\Models\Timestamp)#72 (2) {
    ["size"]=>
    int(8)
    ["value"]=>
    int(54392061979)
  }
  ["verifiableEntityHeaderReserved_1":
    "SymbolSdk\Symbol\Models\Transaction":private]=>
  int(0)
  ["entityBodyReserved_1":
    "SymbolSdk\Symbol\Models\Transaction":private]=>
  int(0)
}
//}

各設定項目について説明します。

==== 有効期限

sdkではデフォルトで2時間後に設定されます。最大6時間まで指定可能です。

//emlist[][php]{
new Timestamp($facade->now()->addHours(2))
//}

==== メッセージ

トランザクションに最大1023バイトのメッセージを添付することができます。バイナリデータであってもrawdataとして送信することが可能です。

===== 空メッセージ

//emlist[][php]{
$messageData = "";?
//}

===== 平文メッセージ

エクスプローラーなどで表示するためには先頭に\0を付加する必要があります。

//emlist[][php]{
$messageData = "\0hello, symbol!";
//}

===== 暗号文メッセージ

MessageEncoder を使用して暗号化すると、自動で暗号文メッセージを表すメッセージタイプ 0x01 が付加されます。

//emlist[][php]{
$aliceMesgEncoder = new MessageEncoder($aliceKey);
$encryptedMessage = $aliceMesgEncoder->encode($bobKey->publicKey(), $message);
//}

==== 最大手数料

ネットワーク手数料については、常に少し多めに払っておけば問題はないのですが、最低限の知識は持っておく必要があります。アカウントはトランザクションを作成するときに、ここまでは手数料として払ってもいいという最大手数料を指定します。一方で、ノードはその時々で最も高い手数料となるトランザクションのみブロックにまとめて収穫しようとします。つまり、多く払ってもいいというトランザクションが他に多く存在すると承認されるまでの時間が長くなります。逆に、より少なく払いたいというトランザクションが多く存在し、その総額が大きい場合は、設定した最大額に満たない手数料額で送信が実現します。

トランザクションサイズ x feeMultiprilerというもので決定されます。176バイトだった場合 maxFee を100で設定すると 17600μXYM = 0.0176XYMを手数料として支払うことを許容します。feeMultiprier = 100として指定する方法とmaxFee = 17600 として指定する方法があります。

===== feeMultiprier = 100として指定する方法

//emlist[][php]{
$facade->setMaxFee($transferTransaction, 100);
//}

== 署名とアナウンス

作成したトランザクションを秘密鍵で署名して、任意のノードを通じてアナウンスします。

=== 署名

//emlist[][js]{
$signature = $aliceKey->signTransaction($transferTransaction);
$payload = $facade->attachSignature($transferTransaction, $signature);
var_dump($payload);
//}

====== 出力例

//emlist{
array(1) {
  ["payload"]=>
  string(382) "BF0000000000000071B2D3DA6B83E6904DEB1EE5AE259D6F51B71E742B3F4D5B"
  ."9EC394BADC844A1574F9418F0349D825AAD836100F39F0AB87EC0BF19BE8E21B1AB94D0AC11B7"
  ."80625189135BF2307DCBCD1657A34ABC3FDEEC04A126D4572876BCA4F514DB5AC9B0000000001"
  ."9854419C4A0000000000000CB455A40C00000098D02DDEA92E7186B5321831A58554E7B6EB584"
  ."54B08161E0F00010000000000CE8BA0672E21C07240420F00000000000068656C6C6F2C207379"
  ."6D626F6C21"
}
//}

=== アナウンス

//emlist[][php]{
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();
$apiInstance = new TransactionRoutesApi($client, $config);

try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: '
    . $e->getMessage()
    . PHP_EOL;
}
//}

※ IDEを使っている人は@<tt>{$apiInstance->announceTransaction($payload)}ここのpayloadの指定でwarningが表示されるかもしれませんが気にしなくて大丈夫です。

//emlist{
{
    "message": "packet 9 was pushed to the network via \/transactions"
}
//}

上記のスクリプトのように @<tt>{packet n was pushed to the network} というレスポンスがあれば、トランザクションはノードに受理されたことになります。これはトランザクションのフォーマット等に異常が無かった程度の意味しかありません。Symbolではノードの応答速度を極限に高めるため、トランザクションの内容を検証するまえに受信結果の応答を返し接続を切断します。レスポンス値はこの情報を受け取ったにすぎません。フォーマットに異常があった場合は以下のようなメッセージ応答があります。

===== アナウンスに失敗した場合の応答例

//emlist{
Uncaught Error: {
  "statusCode":409,
  "statusMessage":"Unknown Error",
  "body":
    "{\"code\":\"InvalidArgument\",\"message\":\"payload has an invalid format\"}"
}
//}

== 確認

=== ステータスの確認

ノードに受理されたトランザクションのステータスを確認

//emlist[][php]{
$hash = $facade->hashTransaction($transferTransaction);
$txStatusApi = new TransactionStatusRoutesApi($client, $config);

try {
  $txStatus = $txStatusApi->getTransactionStatus($hash);
  var_dump($txStatus);
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

====== 出力例

//emlist[][php]{
object(SymbolRestClient\Model\TransactionStatusDTO)#111 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(5) {
    ["group"]=>
    string(11) "unconfirmed"
    ["code"]=>
    string(7) "Success"
    ["hash"]=>
    string(64) "D944914B44D1E210CF9FE7E7BA9C27ACBDB9B8C76E4844CE7E530B564189C323"
    ["deadline"]=>
    string(11) "54299100360"
    ["height"]=>
    string(1) "0"
  }
}
//}

承認されると @<tt>{group: "confirmed"}となっています。

受理されたものの、エラーが発生していた場合は以下のような出力となります。トランザクションを書き直して再度アナウンスしてみてください。

//emlist[][php]{
object(SymbolRestClient\Model\TransactionStatusDTO)#111 (2) {
  ["openAPINullablesSetToNull":protected]=>
  array(0) {
  }
  ["container":protected]=>
  array(5) {
    ["group"]=>
    string(6) "failed"
    ["code"]=>
    string(33) "Failure_Core_Insufficient_Balance"
    ["hash"]=>
    string(64) "1DC2808BC3BBAD6F8C9ECDC1C5F0523D681E875CA414944D73CCCB592E7775CF"
    ["deadline"]=>
    string(11) "54299158295"
    ["height"]=>
    NULL
  }
}
//}

以下のようにResourceNotFoundエラーが発生した場合はトランザクションが受理されていません。

//emlist{
response:
{
  "code":"ResourceNotFound",
  "message":"no resource exists with id" 
    . "'066A063559C2D4EF2EA3222A9BA46193520915DE24EA4B3E601910" 
    . "(truncated...)",
}
//}

考えられる可能性としては、トランザクションで指定した最大手数料が、ノードで設定された最低手数料に満たない場合や、アグリゲートトランザクションとしてアナウンスすることが求められているトランザクションを単体のトランザクションでアナウンスした場合に発生するようです。

=== 承認確認

トランザクションがブロックに承認されるまでに30秒程度かかります。

==== エクスプローラーで確認

signedTx.hash で取得できるハッシュ値を使ってエクスプローラーで検索してみましょう。

//emlist[][js]{
$hash = $facade->hashTransaction($transferTransaction);
echo $hash . PHP_EOL;
//}

//emlist{
> "BAD83010F0A707A8ED0AF9D5BFDDBF8226878F6921CE75C7E75BBE50F869870B"
//}

 * メインネット　

//beginchild

 * @<href>{https://symbol.fyi/transactions/BAD83010F0A707A8ED0AF9D5BFDDBF8226878F6921CE75C7E75BBE50F869870B,https://symbol.fyi/transactions/}@<embed>{|html|<br />}BAD83010F0A707A8ED0AF9D5BFDDBF8226878F6921CE75C7E75BBE50F869870B

//endchild

 * テストネット　

//beginchild

 * @<href>{https://testnet.symbol.fyi/transactions/BAD83010F0A707A8ED0AF9D5BFDDBF8226878F6921CE75C7E75BBE50F869870B,https://testnet.symbol.fyi/transactions/}@<embed>{|html|<br />}BAD83010F0A707A8ED0AF9D5BFDDBF8226878F6921CE75C7E75BBE50F869870B

//endchild

==== SDKで確認

//emlist[][php]{
try {
  $result = $apiInstance->getConfirmedTransaction($hash);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

====== 出力例

//emlist[][php]{
{
    "id": "669CABEA527B051AC20A62DF",
    "meta": {
        "height": "1593561",
        "hash": "7F6E7537D731307CCB6F9DDF62E737326071038851939623FF1AB56C83D2FBDF",
        "merkleComponentHash": 
            "7F6E7537D731307CCB6F9DDF62E737326071038851939623FF1AB56C83D2FBDF",
        "index": 0,
        "timestamp": "54293190821",
        "feeMultiplier": 100
    },
    "transaction": {
        "size": 191,
        "signature": 
            "08BEC8BC4B00733F6E029F92F2B2A736BFAFD748D818BEB98DFCF9FAC36AC682"
            . "3A6A53A4F16FF1B6A49B0ECAA99FD31B0BD460A876EAD0C1A5D1990356A65105",
        "signerPublicKey":
            "25189135BF2307DCBCD1657A34ABC3FDEEC04A126D4572876BCA4F514DB5AC9B",
        "version": 1,
        "network": 152,
        "type": 16724,
        "maxFee": "19100",
        "deadline": "54300378357",
        "recipientAddress": "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
        "mosaics": [
            {
                "id": "72C0212E67A08BCE",
                "amount": "1000000"
            }
        ],
        "message": "0068656C6C6F2C2073796D626F6C21"
    }
}
//}

===== 注意点

トランザクションはブロックで承認されたとしても、ロールバックが発生するとトランザクションの承認が取り消される場合があります。ブロックが承認された後、数ブロックの承認が進むと、ロールバックの発生する確率は減少していきます。また、Votingノードの投票で実施されるファイナライズブロックを待つことで、記録されたデータは確実なものとなります。

===== スクリプト例

トランザクションをアナウンスした後は以下のようなスクリプトを流すと、チェーンの状態を把握しやすくて便利です。※途中sleepを入れているのはアナウンスより先にステータスを確認しに行ってしまいエラーを返す可能性があるためです。

//emlist[][php]{
sleep(2);
$hash = $facade->hashTransaction($transferTransaction);
echo "\n===トランザクションハッシュ===" . PHP_EOL;
echo $hash . PHP_EOL;

$txStatusApi = new TransactionStatusRoutesApi($client, $config);

try {
  $txStatus = $txStatusApi->getTransactionStatus($hash);
  echo "\n===ステータスの確認===" . PHP_EOL;
  var_dump($txStatus);
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}

sleep(1);
/**
 * 承認確認
 */
try {
  $result = $apiInstance->getConfirmedTransaction($hash);
  echo "\n===承認確認===" . PHP_EOL;
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

== トランザクション履歴

Aliceが送受信したトランザクション履歴を一覧で取得します。

//emlist[][php]{
try {
  $result = $apiInstance->searchConfirmedTransactions(
    address: $aliceKey->address,
    embedded: "true",
  );
  echo "\n===トランザクション履歴===" . PHP_EOL;
  var_dump($result);
} catch (\Throwable $th) {
  echo 'Exception when calling TransactionRoutesApi->searchConfirmedTransactions: ', 
    . $th->getMessage(),
    . PHP_EOL;
}
//}

※ IDEなどを使うと@<tt>{embedded}はbool型を期待するのでwarningが表示されるかもしれませんが、ここでは文字列の@<tt>{true}を渡します。

====== 出力例

//emlist[][json]{
{
    "data": [
        {
            "id": "669C9D0B527B051AC20A61DF",
            "meta": {
                "height": "1593459",
                "hash": 
                    "5FDA8DB2C16B4C0C3F6C22E6856B2A8\
                      CFAC5001020F7ED3151069D879A0BE29D",
                "merkleComponentHash": 
                    "5FDA8DB2C16B4C0C3F6C22E6856B2A8\
                      CFAC5001020F7ED3151069D879A0BE29D",
                "index": 0,
                "timestamp": "54289384247",
                "feeMultiplier": 534
            },
            "transaction": {
                "size": 187,
                "signature": 
                    "E9E4E49235FC46D892B77DDC37CB42D8\
                      27556860D1C609DEF171A4EC698C8574\
                      4FBD579641E0765CA6B2D9E4E12DCEFF\
                      9E6C9473CF5948CFC17BA7A566444F0A",
                "signerPublicKey": 
                    "81EA7C15E7EC06261C9F654F54EAC474\
                      8CFCF00E09A8FE47779ACD14A7602004",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "100000",
                "deadline": "54296567552",
                "recipientAddress":
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "500000000"
                    }
                ],
                "message": "00476F6F64204C75636B21"
            }
        },
        {
            "id": "669C9D31527B051AC20A61E3",
            "meta": {
                "height": "1593460",
                "hash": 
                    "A39F0A79CD87D4247B00C2397875803A\
                      D2103542B6B3C7F485F80A4A4D613746",
                "merkleComponentHash": 
                    "A39F0A79CD87D4247B00C2397875803A\
                      D2103542B6B3C7F485F80A4A4D613746",
                "index": 0,
                "timestamp": "54289422373",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "3F989538EEEBA80AB3394475B8BA93B4\
                      0B75277B4D12D9003998E20CFBE66721\
                      E38B606184A5DA704257B8D766FAEE28\
                      3901EB0A3945B97C8D18AFB7D83CA104",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54296609500",
                "recipientAddress": 
                    "988EF2B2B89F6BC742C3902F8FCD66CF3B096A3B970DCE0D",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669C9D7A527B051AC20A61E9",
            "meta": {
                "height": "1593462",
                "hash": 
                    "963BFB1866067979D6105053DB85BA19\
                      0DF22E0ABE39732596361B8788EDA664",
                "merkleComponentHash": 
                    "963BFB1866067979D6105053DB85BA19\
                      0DF22E0ABE39732596361B8788EDA664",
                "index": 0,
                "timestamp": "54289494625",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "71B2D3DA6B83E6904DEB1EE5AE259D6F\
                      51B71E742B3F4D5B9EC394BADC844A15\
                      74F9418F0349D825AAD836100F39F0AB\
                      87EC0BF19BE8E21B1AB94D0AC11B7806",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54296687628",
                "recipientAddress": 
                    "98D02DDEA92E7186B5321831A58554E7B6EB58454B08161E",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA1CC527B051AC20A6227",
            "meta": {
                "height": "1593492",
                "hash": 
                    "5B1974CD5B0958F5A716151EA677A28C\
                      20B348920D65E8ECAFEA5C52726E6CD4",
                "merkleComponentHash": 
                    "5B1974CD5B0958F5A716151EA677A28C\
                      20B348920D65E8ECAFEA5C52726E6CD4",
                "index": 0,
                "timestamp": "54290601924",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "2ABCC254980A9C662D3A435149E52891\
                      FFCC7576AF19F9CEE25053E34A3543EA\
                      216A79D3884DFC7770993D32B68EB65A\
                      CAFC7A677BBFF363EA0432D725DF060C",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54297798137",
                "recipientAddress": 
                    "98A1947584CC5BF18F0C31C76BEB6785639E710DF8D0C65A",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA3A1527B051AC20A6243",
            "meta": {
                "height": "1593505",
                "hash": 
                    "45AE0769D78F2CF7CBD86657B3E0B0E1\
                      AAE63D30EA53708B032C00A1A22D0BC0",
                "merkleComponentHash": 
                    "45AE0769D78F2CF7CBD86657B3E0B0E1\
                      AAE63D30EA53708B032C00A1A22D0BC0",
                "index": 0,
                "timestamp": "54291069705",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "F484662FCF3E8C6A5AED6CBF14570919\
                      1F766F75496C712B4096B1A3B1CE1DF9\
                      B19C7E6B4EAED093B5C6649C91315F4A\
                      DC2F05537389BD6B773C888D02192106",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54298265506",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA651527B051AC20A6272",
            "meta": {
                "height": "1593524",
                "hash": 
                    "A5C897E3FACFEBE21C0E7E5AFC34B03C\
                      B69FFE7401E9EB526A518B1CD9909B30",
                "merkleComponentHash": 
                    "A5C897E3FACFEBE21C0E7E5AFC34B03C\
                      B69FFE7401E9EB526A518B1CD9909B30",
                "index": 0,
                "timestamp": "54291758600",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "4A299DCB86E3394778EDAAF08784692C\
                      62E69FBC26E6C65CD097885558F4B686\
                      9740BC4713CD91B45C42A10965303BED\
                      2C2515B35EB9533CAAA10D96978E100B",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54298910859",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA651527B051AC20A6273",
            "meta": {
                "height": "1593524",
                "hash": 
                    "03A4710884D72B960574DFD389ED0F24\
                      84A4FF3F25C07ABA5E5645D57830B415",
                "merkleComponentHash": 
                    "03A4710884D72B960574DFD389ED0F24\
                      84A4FF3F25C07ABA5E5645D57830B415",
                "index": 1,
                "timestamp": "54291758600",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "CEF6B254B5891E8A003930F41C52315D\
                      8A42D51EF04874E20EA63618FF8B3EF5\
                      F5CFA6F8D6100CCCFB34F9F3C5255AEB\
                      8CB40CBD8481B50B46A9FC59DC1ADE08",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54298936740",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA685527B051AC20A6277",
            "meta": {
                "height": "1593525",
                "hash": 
                    "24F6DDD9A53198948EBDC864C5CC8072\
                      D08ECA320F2803B45C78A84B12AAED87",
                "merkleComponentHash":
                    "24F6DDD9A53198948EBDC864C5CC8072\
                      D08ECA320F2803B45C78A84B12AAED87",
                "index": 0,
                "timestamp": "54291809907",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "3CAE1F2908409365391D87C48B5C8C3C\
                      5C11CD920C7CCA8A8C738FB781401308\
                      11AAECA54AA0A235E2956C3B18672801\
                      56EF3EEA0609826B617C2CE934901E0F",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54299008654",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA709527B051AC20A627F",
            "meta": {
                "height": "1593528",
                "hash": 
                    "D944914B44D1E210CF9FE7E7BA9C27AC\
                      BDB9B8C76E4844CE7E530B564189C323",
                "merkleComponentHash": 
                    "D944914B44D1E210CF9FE7E7BA9C27AC\
                      BDB9B8C76E4844CE7E530B564189C323",
                "index": 0,
                "timestamp": "54291942199",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "EA54DC04B9E2039090FC73E4B19EC606\
                      3542FB41978C6C6A327C1DDA3F04678F\
                      50D4F76EF5D0DFC5EB38C002D3F39219\
                      C1278DF9BDD2F83954DA1C4A3001E709",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54299100360",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        },
        {
            "id": "669CA74F527B051AC20A6285",
            "meta": {
                "height": "1593530",
                "hash": 
                    "9E2E98FE88AEA9685E0B0A87E56C17F4\
                      36F429A452CC3A7B187AC37E2B26DB79",
                "merkleComponentHash": 
                    "9E2E98FE88AEA9685E0B0A87E56C17F4\
                      36F429A452CC3A7B187AC37E2B26DB79",
                "index": 0,
                "timestamp": "54292012549",
                "feeMultiplier": 100
            },
            "transaction": {
                "size": 191,
                "signature": 
                    "0A550C24171133AAE9349841E3EE919B\
                      C9B410CC94CB40C568031901CF0ABB29\
                      74F9DFE88068547A06044CB97F4A0315\
                      A648D0EEAB163C8A76E5AD2C086C320F",
                "signerPublicKey": 
                    "25189135BF2307DCBCD1657A34ABC3FD\
                      EEC04A126D4572876BCA4F514DB5AC9B",
                "version": 1,
                "network": 152,
                "type": 16724,
                "maxFee": "19100",
                "deadline": "54299208979",
                "recipientAddress": 
                    "98E521BD0F024F58E670A023BF3A14F3BECAF0280396BED0",
                "mosaics": [
                    {
                        "id": "72C0212E67A08BCE",
                        "amount": "1000000"
                    }
                ],
                "message": "0068656C6C6F2C2073796D626F6C21"
            }
        }
    ],
    "pagination": {
        "pageNumber": 1,
        "pageSize": 10
    }
}
//}

=== Txペイロード作成時のメッセージの差異

【未検証のため検証後執筆予定】

== アグリゲートトランザクション

Symbolでは複数のトランザクションを1ブロックにまとめてアナウンスすることができます。 最大で100件のトランザクションをまとめることができます（連署者が異なる場合は25アカウントまでを連署指定可能）。 以降の章で扱う内容にアグリゲートトランザクションへの理解が必要な機能が含まれますので、 本章ではアグリゲートトランザクションのうち、簡単なものだけを紹介します。

//emlist[][php]{
$bobKey = $facade->createAccount(PrivateKey::random());
$bobAddress = $bobKey->address;
$carolKey = $facade->createAccount(PrivateKey::random());
$carolAddress = $carolKey->address;

// アグリゲートTxに含めるTxを作成
$innerTx1 = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  recipientAddress: $carolAddress,
  message: "\0hello, carol!"
);

$innerTx2 = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  recipientAddress: $bobAddress,
  message: "\0hello, bob!"
);

// マークルハッシュの算出
$embeddedTransactions = [$innerTx1, $innerTx2];
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
// アナウンス
try {
  $result = $apiInstance->announceTransaction($payload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

まず、アグリゲートトランザクションに含めるトランザクションを作成します。このときDeadlineを指定する必要はありませんがnetworkは必ず指定してください。（指定しない場合Failure_Core_Wrong_Networkが発生します）リスト化するときに、生成したトランザクションにtoAggregateを追加して送信元アカウントの公開鍵を指定します。ちなみに送信元アカウントと署名アカウントが必ずしも一致するとは限りません。後の章での解説で「Bobの送信トランザクションをAliceが署名する」といった事が起こり得るためこのような書き方をします。これはSymbolブロックチェーンでトランザクションを扱ううえで最も重要な概念になります。なお、本章で扱うトランザクションは同じAliceですので、アグリゲートボンデッドトランザクションへの署名もAliceを指定します。

=== アグリゲートトランザクションにおける最大手数料

アグリゲートトランザクションも通常のトランザクション同様、最大手数料を直接指定する方法とfeeMultiprierで指定する方法があります。先の例では最大手数料を直接指定する方法を使用しました。ここではfeeMultiprierで指定する方法を紹介します。

//emlist[][php]{
$requiredCosignatures = 1; // 必要な連署者の数を指定
$calculatedCosignatures = $requiredCosignatures > count($aggregateTx->cosignatures)
    ? $requiredCosignatures
    : count($aggregateTx->cosignatures);
$sizePerCosignature = 8 + 32 + 64;
$calculatedSize = $aggregateTx->size() -
    count($aggregateTx->cosignatures) * $sizePerCosignature +
    $calculatedCosignatures * $sizePerCosignature;

$aggregateTx->fee = new Amount($calculatedSize * 100);  // 手数料

//}

== 現場で使えるヒント

存在証明アカウントの章でアカウントによるデータの署名と検証する方法について説明しました。 このデータをトランザクションに載せてブロックチェーンが承認することで、 アカウントがある時刻にあるデータの存在を認知したことを消すことができなくなります。 タイムスタンプの刻印された電子署名を利害関係者間で所有することと同じ意味があると考えることもできます。 （法律的な判断は他の方にお任せします）

ブロックチェーンは、この消せない「アカウントが認知したという事実」の存在をもって送信などのデータ更新を行います。 また、誰もがまだ知らないはずの事実を知っていたことの証明としてブロックチェーンを利用することもできます。 ここでは、その存在が証明されたデータをトランザクションに載せる２つの方法について説明します。

デジタルデータのハッシュ値(SHA256)出力方法ファイルの要約値をブロックチェーンに記録することでそのファイルの存在を証明することができます。

各OSごとのファイルのSHA256でハッシュ値を計算する方法は下記の通りです。

//emlist[][bash]{
#Windows
certutil -hashfile WINファイルパス SHA256
#Mac
shasum -a 256 MACファイルパス
#Linux
sha256sum Linuxファイルパス
//}

=== 大きなデータの分割

トランザクションのペイロードには1023バイトしか格納できないため、大きなデータは分割してペイロードに詰め込んでアグリゲートトランザクションにします。

//emlist[][php]{
$bigdata = "C00200000000000093B0B985101C1BDD1BC2BF30D72F35E34265B3F381ECA464"
    . "733E147A4F0A6B9353547E2E08189EF37E50D271BEB5F09B81CE5816BB34A153"
    . "7E4FD2268520AF630A0A0E5C72B0D5946C1EFEE7E5317C5985F106B739BB0BC0"
    . "00009A288417B3CD6D26000000000198414140770200000000002A769FB40000"
    . "1B3376B455CFAE2CCDA9C282BF8556D3E9C9C0DE18B0CBE6660ACCF86EB54AC5"
    . "85F1B001000000000000DB000000000000000E5C72B0D5946C1EFEE7E5317C59"
    . "6C4506B739BB0BC07E4F9A288417B3CD6D26000000000198544198205C1A4CE0"
    . "3435B3A896B1B2360E03633B9F36BF7F22338B00000000000000006665346535"
    . "3841393833444430383935303645394533424446434235313637433046394232"
    . "4438353445364630324138373645353037344236413033374146434142333033"
    . "3233384130363035334335334535423541383532383544363943413236423534"
    . "4238343032364244444331443133343139464435353438323930334242453038"
    . "A23432304100000000006800000000000000B2D4FD84B2B63A96AA37C35FC6E0"
    . "6C451CEC1FD19C8FFC8D93CCCA2B028D1E9D000000000198444198205C1A4CE0"
    . "3435B3A896B1B2360E03633B9F36BF7F2233BC089179EBBE01A8140014003538"
    . "3A96344434373631364336433635373237396800000000000000B2D4FD84B2B6"
    . "4441AA37C35FC6E0A2341CEC1FD19C8FFC8D93CCCA2B028D1E9D000000000198"
    . "9BEB98205C1A4CE06C45B3A896B1B2360E03633B9F36BF7F223345ECB996EDDB"
    . "0000140014003538343534443437363136433643363537323739000000000000"
    . "1E9DB2D4FD84B2B63A96AA37C35FC6E0A2341CEC1FD19C8FFC8D93CCCA2B028D"
    . "0E2D5A71EBA9C924EFA146897BE6C9BB3DACEFA26A07D687AC4A83C9B0308764"
    . "1DDAE952E9DDBC33312E2C8D021B4CC0435852C0756B1EBD983FCE221A981D02";

$payloads = [];
for ($i = 0; $i < strlen($bigdata) / 1023; $i++) {
    $payloads[] = substr($bigdata, $i * 1023, 1023);
}

print_r($payloads);
//}

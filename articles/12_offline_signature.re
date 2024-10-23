= オフライン署名

ロック機構の章で、アナウンスしたトランザクションをハッシュ値指定でロックして、複数の署名（オンライン署名）を集めるアグリゲートトランザクションを紹介しました。この章では、トランザクションを事前に署名を集めてノードにアナウンスするオフライン署名について説明します。

=== 手順

Aliceが起案者となりトランザクションを作成し、署名します。次にBobが署名してAliceに返します。最後にAliceがトランザクションを結合してネットワークにアナウンスします。

== トランザクション作成

//emlist[][php]{
$bobPrivateKey = 'B34C8DEEADF5FE608CB2FD245C9ECF8A70DAD7F7E66CB22614BAF90E******';
$bobKey = $facade->createAccount(new PrivateKey($bobPrivateKey));

// アグリゲートTxに含めるTxを作成
$innerTx1 = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $aliceKey->publicKey,
  recipientAddress: $bobKey->address,
  mosaics:[],
  message: "\0tx1",
);

$innerTx2 = new EmbeddedTransferTransactionV1(
  network: new NetworkType(NetworkType::TESTNET),
  signerPublicKey: $bobKey->publicKey,
  recipientAddress: $aliceKey->address,
  mosaics:[],
  message: "\0tx2",
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
  transactions: $embeddedTransactions,
);
$facade->setMaxFee($aggregateTx, 100, 2);

// 署名
$signedHash = $aliceKey->signTransaction($aggregateTx);
$signedPayload = $facade->attachSignature($aggregateTx, $signedHash);
echo "\n===payload===" . PHP_EOL;
echo $signedPayload['payload'] . PHP_EOL;
//}

====== 出力例

//emlist[][js]{
"5801000000000000C3D9A9649B042203011BF8D6EFC912562FED813FCAE4A986\
1D42C9B6C397E00DF54A8634A9AC8AAD06AD882BA4841E60AF8FE9F1D2521110\
B30B3099B83F630825189135BF2307DCBCD1657A34ABC3FDEEC04A126D457287\
6BCA4F514DB5AC9B00000000029841416086000000000000D1B2F5CE0C000000\
8BD1718CB31EF65217DD6263D65D44F02CCA55DA429EDE4641F4FF0C97ECABBE\
B000000000000000550000000000000025189135BF2307DCBCD1657A34ABC3FD\
EEC04A126D4572876BCA4F514DB5AC9B000000000198544198B4A75DD1ADA814\
4D60BD8107002C2FEB02DD2EFD5C788E05000000000000005C30747831000000\
5500000000000000A3378BE54307C0A814CDDEB2F9BEC1ACBEA44F298063A16A\
06B5C4ACE0AD28B4000000000198544198E521BD0F024F58E670A023BF3A14F3\
BECAF0280396BED005000000000000005C30747832000000"
//}

署名を行い、signedHash,signedPayloadを出力します。signedPayloadをBobに渡して署名を促します。

== Bobによる連署

Aliceから受け取ったsignedPayloadでトランザクションを復元します。

//emlist[][php]{
$tx = TransactionFactory::deserialize(hex2bin($signedPayload['payload'])); // バイナリデータにする
echo "\n===tx===" . PHP_EOL;
print_r($tx);
//}

====== 出力例

//emlist[][php]{
SymbolSdk\Symbol\Models\AggregateCompleteTransactionV2 Object
(
    [transactionsHash] => SymbolSdk\Symbol\Models\Hash256 Object
        (
            [binaryData] => ��q���R�bc�]D�,�U�B��FA��
                                                     �쫾
        )

    [transactions] => Array
        (
            [0] => SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1 Object
                (
                    [recipientAddress] => SymbolSdk\Symbol\Models\UnresolvedAddress Object
                        (
                            [binaryData] => ���]ѭ�M`��,/��.�\x�
                        )

                    [mosaics] => Array
                        (
                        )

                    [message] => \0tx1
                    [transferTransactionBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [transferTransactionBodyReserved_2:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => %�5�#ܼ�ez4�����JmEr�k�OQM���
                        )

                    [version] => 1
                    [network] => SymbolSdk\Symbol\Models\NetworkType Object
                        (
                            [value] => 152
                        )

                    [type] => SymbolSdk\Symbol\Models\TransactionType Object
                        (
                            [value] => 16724
                        )

                    [embeddedTransactionHeaderReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                    [entityBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                )

            [1] => SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1 Object
                (
                    [recipientAddress]
                        => SymbolSdk\Symbol\Models\UnresolvedAddress Object
                        (
                            [binaryData] => ��!�OX�p�#�:���(���
                        )

                    [mosaics] => Array
                        (
                        )

                    [message] => \0tx2
                    [transferTransactionBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [transferTransactionBodyReserved_2:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [signerPublicKey]
                        => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => �7��C���޲������O)�c�j�Ĭ�(�
                        )

                    [version] => 1
                    [network] => SymbolSdk\Symbol\Models\NetworkType Object
                        (
                            [value] => 152
                        )

                    [type] => SymbolSdk\Symbol\Models\TransactionType Object
                        (
                            [value] => 16724
                        )

                    [embeddedTransactionHeaderReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                    [entityBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                )

        )

    [cosignatures] => Array
        (
        )

    [aggregateTransactionHeaderReserved_1:
        SymbolSdk\Symbol\Models\AggregateCompleteTransactionV2:
        private] => 0
    [signature] => SymbolSdk\Symbol\Models\Signature Object
        (
            [binaryData] => �����FI�Q�Q��9#�{���k�-��Z�bb�
                                                          m�`�@u�^V�~cA'�74�Fv
        )

    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
        (
            [binaryData] => %�5�#ܼ�ez4�����JmEr�k�OQM���
        )

    [version] => 2
    [network] => SymbolSdk\Symbol\Models\NetworkType Object
        (
            [value] => 152
        )

    [type] => SymbolSdk\Symbol\Models\TransactionType Object
        (
            [value] => 16705
        )

    [fee] => SymbolSdk\Symbol\Models\Amount Object
        (
            [size] => 8
            [value] => 34400
        )

    [deadline] => SymbolSdk\Symbol\Models\Timestamp Object
        (
            [size] => 8
            [value] => 55012380245
        )

    [verifiableEntityHeaderReserved_1:
        SymbolSdk\Symbol\Models\Transaction:
        private] => 0
    [entityBodyReserved_1:
        SymbolSdk\Symbol\Models\Transaction:
        private] => 0
)
//}

念のため、Aliceがすでに署名したトランザクション（ペイロード）かどうかを検証します。

//emlist[][php]{
$signature = new Signature($tx->signature);
$res = $facade->verifyTransaction($tx, $signature);
var_dump($res);
//}

====== 出力例

//emlist{
true
//}

ペイロードがsigner、つまりAliceによって署名されたものであることが確認できました。次にBobが連署します。

//emlist[][php]{
$bobCosignature = $facade->cosignTransaction($bobKey->keyPair, $tx, true);
$bobSignedTxSignature = $bobCosignature->signature;
$bobSignedTxSignerPublicKey = $bobCosignature->signerPublicKey;
//}

CosignatureTransactionで署名を行い、bobSignedTxSignature,bobSignedTxSignerPublicKeyを出力しAliceに返却します。Bobが全ての署名を揃えられる場合は、Aliceに返却しなくてもBobがアナウンスすることも可能です。

== Aliceによるアナウンス

AliceはBobからbobSignedTxSignature,bobSignedTxSignerPublicKeyを受け取ります。また事前にAlice自身で作成したsignedPayloadを用意します。

//emlist[][php]{
$recreatedTx = TransactionFactory::deserialize(hex2bin($signedPayload['payload']));

// 連署者の署名を追加
$cosignature = new Cosignature();
$signTxHash = $facade->hashTransaction($aggregateTx);
$cosignature->parentHash = new Hash256($signTxHash);
$cosignature->version = 0;
$cosignature->signerPublicKey = $bobSignedTxSignerPublicKey;
$cosignature->signature = $bobSignedTxSignature;
array_push($recreatedTx->cosignatures, $cosignature);

$signedPayload = ["payload" => strtoupper(bin2hex($recreatedTx->serialize()))];
echo $signedPayload;

try {
  $result = $apiInstance->announceTransaction($signedPayload);
  echo $result . PHP_EOL;
} catch (Exception $e) {
  echo 'Exception when calling TransactionRoutesApi->announceTransaction: ',
    . $e->getMessage(),
    . PHP_EOL;
}
//}

== 現場で使えるヒント

=== マーケットプレイスレス

ボンデッドトランザクションと異なりハッシュロックのための手数料(10XYM)を気にする必要がありません。@<br>{}ペイロードを共有できる場が存在する場合、売り手は考えられるすべての買い手候補に対してペイロードを作成して交渉開始を待つことができます。（複数のトランザクションが個別に実行されないように、1つしか存在しない領収書NFTをアグリゲートトランザクションに混ぜ込むなどして排他制御をしてください）。この交渉に専用のマーケットプレイスを構築する必要はありません。SNSのタイムラインをマーケットプレイスにしたり、必要に応じて任意の時間や空間でワンタイムマーケットプレイスを展開することができます。

ただ、オフラインで署名を交換するため、なりすましのハッシュ署名要求には気を付けましょう。@<br>{}（必ず検証可能なペイロードからハッシュを生成して署名するようにしてください）

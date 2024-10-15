= 13.検証

ブロックチェーン上に記録されたさまざまな情報を検証します。ブロックチェーンへのデータ記録は全ノードの合意を持って行われますが、ブロックチェーンへの@<b>{データ参照}はノード単体からの情報取得であるため、信用できないノードの情報を元にして新たな取引を行いたい場合は、ノードから取得したデータに対して検証を行う必要があります。

== 13.1 トランザクションの検証

トランザクションがブロックヘッダーに含まれていることを検証します。この検証が成功すれば、トランザクションがブロックチェーンの合意によって承認されたものとみなすことができます。

=== 検証するペイロード

今回検証するトランザクションペイロードとそのトランザクションが記録されているとされるブロック高です。

//emlist{
payload =
  "2802000000000000A5151FD55D82351DD488DB5563DD328DA72B2AD25B513C1D\
    0F7F78AFF4D35BA094ABF505C74E6D6BE1FA19F3E5AC60A85E1A4EDC4AC07DEC\
    C0E56C59D5D24F0B69A31A837EB7DE323F08CA52495A57BA0A95B52D1BB54CEA\
    9A94C12A87B1CADB0000000002984141A0D70000000000000EEAD68105000000\
    62E78B6170628861B4FC4FCA75210352ACDBD2378AC0A447A3DCF63F969366BB\
    1801000000000000540000000000000069A31A837EB7DE323F08CA52495A57BA\
    0A95B52D1BB54CEA9A94C12A87B1CADB000000000198544198A8D76FEF838227\
    4D472EE377F2FF3393E5B62C08B4329D04000000000000000074783100000000\
    590000000000000069A31A837EB7DE323F08CA52495A57BA0A95B52D1BB54CEA\
    9A94C12A87B1CADB000000000198444198A8D76FEF8382274D472EE377F2FF33\
    93E5B62C08B4329D6668A0DE72812AAE05000500746573743100000000000000\
    590000000000000069A31A837EB7DE323F08CA52495A57BA0A95B52D1BB54CEA\
    9A94C12A87B1CADB000000000198444198A8D76FEF8382274D472EE377F2FF33\
    93E5B62C08B4329DBF85DADBFD54C48D05000500746573743200000000000000\
    0000000000000000662CEDF69962B1E0F1BF0C43A510DFB12190128B90F7FE9B\
    A48B1249E8E10DBEEDD3B8A0555B4237505E3E0822B74BCBED8AA3663022413A\
    FDA265BE1C55431ACAE3EA975AF6FD61DEFFA6A16CBA5174A16EF5553AE669D5\
    803A0FA9D1424600";
height = 686312;
//}

=== payload確認

トランザクションの内容を確認します。

//emlist[][php]{
$tx = TransactionFactory::deserialize(hex2bin($payload));
$hash = $facade->hashTransaction($tx);
echo "\n===payload確認===" . PHP_EOL;
echo $hash . PHP_EOL;
print_r($tx);
//}

====== 出力例

//emlist{
4A1C88BBFE6EB46111C2B02F7C7355DAE186E54132197C2CD6D51297846A1824
SymbolSdk\Symbol\Models\AggregateCompleteTransactionV2 Object
(
    [transactionsHash] => SymbolSdk\Symbol\Models\Hash256 Object
        (
            [binaryData] => b�apb�a��O�u!R���7���G���?��f�
        )

    [transactions] => Array
        (
            [0] => SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1 Object
                (
                    [recipientAddress] => SymbolSdk\Symbol\Models\UnresolvedAddress
                        Object
                        (
                            [binaryData] => ���o'MG.�w��3���2�
                        )

                    [mosaics] => Array
                        (
                        )

                    [message] => tx1
                    [transferTransactionBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [transferTransactionBodyReserved_2:
                        SymbolSdk\Symbol\Models\EmbeddedTransferTransactionV1:
                        private] => 0
                    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => i��~��2�RIZW�
��-LꚔ�*����
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

            [1] => SymbolSdk\Symbol\Models\EmbeddedAccountMetadataTransactionV1
                Object
                (
                    [targetAddress] => SymbolSdk\Symbol\Models\UnresolvedAddress
                        Object
                        (
                            [binaryData] => ���o'MG.�w��3���2�
                        )

                    [scopedMetadataKey] => -5896758431726933914
                    [valueSizeDelta] => 5
                    [value] => test1
                    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => i��~��2�RIZW�
��-LꚔ�*����
                        )

                    [version] => 1
                    [network] => SymbolSdk\Symbol\Models\NetworkType Object
                        (
                            [value] => 152
                        )

                    [type] => SymbolSdk\Symbol\Models\TransactionType Object
                        (
                            [value] => 16708
                        )

                    [embeddedTransactionHeaderReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                    [entityBodyReserved_1:
                        SymbolSdk\Symbol\Models\EmbeddedTransaction:
                        private] => 0
                )

            [2] => SymbolSdk\Symbol\Models\EmbeddedAccountMetadataTransactionV1
                Object
                (
                    [targetAddress] => SymbolSdk\Symbol\Models\UnresolvedAddress
                        Object
                        (
                            [binaryData] => ���o'MG.�w��3���2�
                        )

                    [scopedMetadataKey] => -8231360769634433601
                    [valueSizeDelta] => 5
                    [value] => test2
                    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => i��~��2�RIZW�
��-LꚔ�*����
                        )

                    [version] => 1
                    [network] => SymbolSdk\Symbol\Models\NetworkType Object
                        (
                            [value] => 152
                        )

                    [type] => SymbolSdk\Symbol\Models\TransactionType Object
                        (
                            [value] => 16708
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
            [0] => SymbolSdk\Symbol\Models\Cosignature Object
                (
                    [version] => 0
                    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
                        (
                            [binaryData] => f,���b���
�                                                    C�߱!��������I��
                        )

                    [signature] => SymbolSdk\Symbol\Models\Signature Object
                        (
                            [binaryData] => �Ӹ�U[B7P^"�K�튣f0"A:��e�UC���Z��a����l�Qt�n�U:�iՀ:��BF
                        )

                )

        )

    [aggregateTransactionHeaderReserved_1:
        SymbolSdk\Symbol\Models\AggregateCompleteTransactionV2:
        private] => 0
    [signature] => SymbolSdk\Symbol\Models\Signature Object
        (
            [binaryData] => ��]�5Ԉ�Uc�2��+*�[Q<x���[�����Nmk����`�^N�J�}���lY��O

        )

    [signerPublicKey] => SymbolSdk\Symbol\Models\PublicKey Object
        (
            [binaryData] => i��~��2�RIZW�
��-LꚔ�*����
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
            [value] => 55200
        )

    [deadline] => SymbolSdk\Symbol\Models\Timestamp Object
        (
            [size] => 8
            [value] => 23653181966
        )

    [verifiableEntityHeaderReserved_1:
        SymbolSdk\Symbol\Models\Transaction:
        private] => 0
    [entityBodyReserved_1:
        SymbolSdk\Symbol\Models\Transaction:
        private] => 0
)
//}

=== 署名者の検証

トランザクションがブロックに含まれていることが確認できれば自明ですが、@<br>{}念のため、アカウントの公開鍵でトランザクションの署名を検証しておきます。

//emlist[][php]{
$signature = new Signature($tx->signature);
$res = $facade->verifyTransaction($tx, $signature);
echo "\n===署名の検証===" . PHP_EOL;
var_dump($res);
console.log(res);
//}

//emlist{
true
//}

=== マークルコンポーネントハッシュの計算

トランザクションのハッシュ値には連署者の情報が含まれていません。一方でブロックヘッダーに格納されるマークルルートはトランザクションのハッシュに連署者の情報が含めたものが格納されます。そのためトランザクションがブロック内部に存在しているかどうかを検証する場合は、トランザクションハッシュをマークルコンポーネントハッシュに変換しておく必要があります。

//emlist[][php]{
$merkleComponentHash = $hash;

if (isset($tx->cosignatures) && count($tx->cosignatures) > 0) {
  $hasher = new MerkleHashBuilder();
  $hash = new Hash256($hash);
  $hasher->update($hash);
  foreach ($tx->cosignatures as $cosignature) {
    $hasher->update(new Hash256($cosignature->signerPublicKey));
  }
  $merkleComponentHash = $hasher->final();
}
echo strtoupper($merkleComponentHash) . PHP_EOL;
//}

====== 出力例

//emlist{
C61D17F89F5DEBC74A98A1321DB71EB7DC9111CDF1CF3C07C0E9A91FFE305AC3
//}

=== InBlockの検証

ノードからマークルツリーを取得し、先ほど計算したmerkleComponentHashからブロックヘッダーのマークルルートが導出できることを確認します。

//emlist[][php]{
$leafhash = new Hash256($merkleComponentHash);

// ノードから取得
$config = new Configuration();
$config->setHost($NODE_URL);
$client = new GuzzleHttp\Client();
$blockApiInstance = new BlockRoutesApi($client, $config);

$HRoot = $blockApiInstance->getBlockByHeight($height);
$HRootHash = new Hash256($HRoot["block"]["transactions_hash"]);

$merkleProof = $blockApiInstance->getMerkleTransaction($height, $leafhash);
$merklePath = $merkleProof["merkle_path"];

$resutl = Merkle::proveMerkle($leafhash, $merklePath, $HRootHash);
echo "===InBlockの検証===" . PHP_EOL;
var_dump($resutl);
//}

//emlist{
bool(true)
//}

トランザクションの情報がブロックヘッダーに含まれていることが確認できました。

== 13.2 ブロックヘッダーの検証

既知のブロックハッシュ値（例：ファイナライズブロック）から、検証中のブロックヘッダーまでたどれることを検証します。

=== normalブロックの検証

//emlist[][php]{
/**
 * Converts a given number to a reversed hex string of specified byte length.
 *
 * @param int|string $number The number to convert.
 * @param int $bytes The byte length of the resulting hex string.
 * @return string The reversed hex string.
 */
function reverseHex($number, $bytes = 1) {
  // 10進数を16進数に変換し、必要に応じてゼロパディング
  $hex = str_pad(dechex($number), $bytes * 2, "0", STR_PAD_LEFT);
  // 16進数の文字列をバイナリデータに変換
  $bin = hex2bin($hex);
  // バイナリデータを逆順にする
  $reversed = strrev($bin);
  // バイナリデータを16進数の文字列に変換
  $reversedHex = bin2hex($reversed);
  return $reversedHex;
}

$blockInfo = $blockApiInstance->getBlockByHeight($height);
$block = $blockInfo["block"];
$previousBlockHash = $blockApiInstance->getBlockByHeight($height - 1);
$previousBlockHash = $previousBlockHash["meta"]["hash"];

if ($block['type'] === BlockType::NORMAL) {
  $hasher = hash_init('sha3-256');

  hash_update($hasher, hex2bin($block['signature'])); // signature
  hash_update($hasher, hex2bin($block['signer_public_key'])); // publicKey

  hash_update($hasher, hex2bin(reverseHex($block['version'],1)));
  hash_update($hasher, hex2bin(reverseHex($block['network'], 1)));
  hash_update($hasher, hex2bin(reverseHex($block['type'], 2)));
  hash_update($hasher, hex2bin(reverseHex($block['height'], 8)));
  hash_update($hasher, hex2bin(reverseHex($block['timestamp'], 8)));
  hash_update($hasher, hex2bin(reverseHex($block['difficulty'], 8)));

  hash_update($hasher, hex2bin($block['proof_gamma']));
  hash_update($hasher, hex2bin($block['proof_verification_hash']));
  hash_update($hasher, hex2bin($block['proof_scalar']));
  hash_update($hasher, hex2bin($previousBlockHash));
  hash_update($hasher, hex2bin($block['transactions_hash']));
  hash_update($hasher, hex2bin($block['receipts_hash']));
  hash_update($hasher, hex2bin($block['state_hash']));
  hash_update($hasher, hex2bin($block['beneficiary_address']));
  hash_update($hasher, hex2bin(reverseHex($block['fee_multiplier'], 4)));

  $hash = strtoupper(bin2hex(hash_final($hasher, true)));

  echo "===ブロックヘッダーの検証===" . PHP_EOL;
  var_dump($hash === $blockInfo['meta']['hash']);
}
//}

true が出力されればこのブロックハッシュは前ブロックハッシュ値の存在を認知していることになります。同様にしてn番目のブロックがn-1番目のブロックの存在を確認し、最後に検証中のブロックにたどり着きます。

これで、どのノードに問い合わせても確認可能な既知のファイナライズブロックが、検証したいブロックの存在に支えられていることが分かりました。

=== importanceブロックの検証

importanceBlockは、importance値の再計算が行われるブロック(720ブロック毎、テストネットは180ブロック毎)です。NormalBlockに加えて以下の情報が追加されています。

 * votingEligibleAccountsCount
 * harvestingEligibleAccountsCount
 * totalVotingBalance
 * previousImportanceBlockHash

//emlist[][php]{
$height = 1440;
// height = Importance Block のブロック高
$blockInfo = $blockApiInstance->getBlockByHeight($height);
$block = $blockInfo["block"];
$previousBlockHash = $blockApiInstance->getBlockByHeight($height - 1);
$previousBlockHash = $previousBlockHash["meta"]["hash"];

if($block['type'] === BlockType::IMPORTANCE){
  $hasher = hash_init('sha3-256');

  hash_update($hasher, hex2bin($block['signature'])); // signature
  hash_update($hasher, hex2bin($block['signer_public_key'])); // publicKey

  hash_update($hasher, hex2bin(reverseHex($block['version'],1)));
  hash_update($hasher, hex2bin(reverseHex($block['network'],1)));
  hash_update($hasher, hex2bin(reverseHex($block['type'],1)));
  hash_update($hasher, hex2bin(reverseHex($block['height'], 8)));
  hash_update($hasher, hex2bin(reverseHex($block['timestamp'], 8)));
  hash_update($hasher, hex2bin(reverseHex($block['difficulty'], 8)));

  hash_update($hasher, hex2bin($block['proof_gamma']));
  hash_update($hasher, hex2bin($block['proof_verification_hash']));
  hash_update($hasher, hex2bin($block['proof_scalar']));
  hash_update($hasher, hex2bin($previousBlockHash));
  hash_update($hasher, hex2bin($block['transactions_hash']));
  hash_update($hasher, hex2bin($block['receipts_hash']));
  hash_update($hasher, hex2bin($block['state_hash']));
  hash_update($hasher, hex2bin($block['beneficiary_address']));
  hash_update($hasher, hex2bin(reverseHex($block['fee_multiplier'], 4)));
  hash_update($hasher, hex2bin(reverseHex(
    $block['voting_eligible_accounts_count'], 4
  )));
  hash_update($hasher, hex2bin(reverseHex(
    $block['harvesting_eligible_accounts_count'], 8
  )));
  hash_update($hasher, hex2bin(reverseHex($block['total_voting_balance'], 8)));
  hash_update($hasher, hex2bin($block['previous_importance_block_hash']));

  $hash = strtoupper(bin2hex(hash_final($hasher, true)));

  echo "===importanceブロックの検証===" . PHP_EOL;
  var_dump($hash === $blockInfo['meta']['hash']);
}
//}

後述するアカウントやメタデータの検証のために、stateHashSubCacheMerkleRootsを検証しておきます。

=== stateHashの検証

//emlist[][php]{
print_r($blockInfo);
//}

//emlist[][php]{
SymbolRestClient\Model\BlockInfoDTO Object
(
    [openAPINullablesSetToNull:protected] => Array
        (
        )

    [container:protected] => Array
        (
            [id] => 644612E35FF7CEC60709D2B9
            [meta] => SymbolRestClient\Model\BlockMetaDTO Object
                (
                    [openAPINullablesSetToNull:protected] => Array
                        (
                        )

                    [container:protected] => Array
                        (
                            [hash] => 
BC3AA1488BFF936B82D0E610C127AE7943A189B44D8B0989E7E5F9EE2B5A6C2F
                            [total_fee] => 18400
                            [generation_hash] => 
9B5F2DA8E11FE168476EAAEBA4C7CA1902709717A16DF4E01DC9E4135263B145
                            [state_hash_sub_cache_merkle_roots] => Array
                                (
                                    [0] => 
F515B3574DBC4E25970911D31D3EC904FEC2F53A913818DD22767F0803AAA448
                                    [1] => 
2A9CC5948DF4D61A242BD4FBFE4059BD28035F92AF8C939E1619E555E228B365
                                    [2] => 
916F3DC4F4570B0605BE8D5B9F0AC6D7C9D6512C50317145667B8EC4A9C034CC
                                    [3] => 
CB1E5FB72D99DCF3323C61008ACDD88601E09627DAF6F26CB9A4D212DE2FAAE6
                                    [4] => 
7BD54F7855071892E30C38209069FEDA4053753631F29D9EFBFDE19D3D46986D
                                    [5] => 
71A589B4B0F1246A4959D7DDFB1A1DD52239F8061A000A6AA77EBE0CAFA82C13
                                    [6] => 
5BCA0065AFA492A9285B16B046A82E09F273ED09624E48D4A5172565C95C41F1
                                    [7] => 
5370FB2FFEE6CDD1BE697D98B30BC405F2456960A3047D8806D2CDA919245B31
                                    [8] => 
DA846A9D0C7E6C284B1ED0FFE11CFCA35CE659C2B3F8E562330974D9CCE08BDF
                                )

                            [total_transactions_count] => 1
                            [transactions_count] => 1
                            [statements_count] => 2
                        )

                )

            [block] => SymbolRestClient\Model\BlockInfoDTOBlock Object
                (
                    [openAPINullablesSetToNull:protected] => Array
                        (
                        )

                    [container:protected] => Array
                        (
                            [size] => 608
                            [signature] => 
"0D2A4A8833AC55FA18462D0321DFD4E1CC0653D46DD3BC4B08EB935E3BD84F"
. "8878F5D2416A36179A137F1475A877BDAE1F722A2FBD6D87D2773507538E81C001"
                            [signer_public_key] => 
87EEE5E3D69BAA60C093FC2080BA5D36E623C5C0BCDC529B8712A9B6212420D7
                            [version] => 1
                            [network] => 152
                            [type] => 33347
                            [height] => 1440
                            [timestamp] => 109140035
                            [difficulty] => 10000000000000
                            [proof_gamma] => 
6B0296960EF59524332C789834BE0D859076722B59C1233432A0D5506E356BC2
                            [proof_verification_hash] => 
0C8F8F162DD82A3F26D3019289365B57
                            [proof_scalar] => 
8EE0BEC47CFD1A77CDA848A6CFAFF02EAFC05EEE02D374BA7758BEFE22090F07
                            [previous_block_hash] => 
06EBD4C3C2C654FFD75463FF2F98DC26F5629446840DFEAC4803B29B5765E084
                            [transactions_hash] => 
5AB5E7A0E087BB3FB788BFC44443240E53C4F53AE558C23A60DA8140C6CCC50D
                            [receipts_hash] => 
374F773A5003D374FD185FF6EB2BE2997333108A600A38E85074E627261A420E
                            [state_hash] => 
518861727944E73D76EE10173DFE0ADA56BF252890BC27C75246989B2046B803
                            [beneficiary_address] => 
98BE9AC4CD3E833736762A12A63065FF42E476744E6FC597
                            [fee_multiplier] => 100
                            [voting_eligible_accounts_count] => 4
                            [harvesting_eligible_accounts_count] => 18
                            [total_voting_balance] => 19000392156923
                            [previous_importance_block_hash] => 
157F710AB66239C09F262BF2F243568F2E25B58FA16E85A47E22966D20041466
                        )

                )

        )

)
//}

//emlist[][php]{
$hasher = hash_init('sha3-256');
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][0]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][1]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][2]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][3]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][4]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][5]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][6]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][7]
));
hash_update($hasher, hex2bin(
    $blockInfo['meta']['state_hash_sub_cache_merkle_roots'][8]
));
$hash = strtoupper(bin2hex(hash_final($hasher, true)));
echo "===stateHashの検証===" . PHP_EOL;
var_dump($hash === $blockInfo['block']['state_hash']);
//}

//emlist[][php]{
bool(true)
//}

ブロックヘッダーの検証に利用した9個のstateがstateHashSubCacheMerkleRootsから構成されていることがわかります。

== 13.3 アカウント・メタデータの検証

マークルパトリシアツリーを利用して、トランザクションに紐づくアカウントやメタデータの存在を検証します。@<br>{}サービス提供者がマークルパトリシアツリーを提供すれば、利用者は自分の意志で選択したノードを使ってその真偽を検証することができます。

=== 検証用共通関数

//emlist[][php]{
 //検証用共通関数

function reverseHex($number, $bytes = 1) {
  // 10進数を16進数に変換し、必要に応じてゼロパディング
  $hex = str_pad(dechex($number), $bytes * 2, "0", STR_PAD_LEFT);
  // 16進数の文字列をバイナリデータに変換
  $bin = hex2bin($hex);
  // バイナリデータを逆順にする
  $reversed = strrev($bin);
  // バイナリデータを16進数の文字列に変換
  $reversedHex = bin2hex($reversed);
  return $reversedHex;
}

//葉のハッシュ値取得関数
function getLeafHash($encodedPath, $leafValue) {
  $hasher = hash_init('sha3-256');
  hash_update($hasher, hex2bin($encodedPath . $leafValue));
  $hash = strtoupper(bin2hex(hash_final($hasher, true)));
  return $hash;
}

// 枝のハッシュ値取得関数
function getBranchHash($encodedPath, $links) {
  $branchLinks = array_fill(0, 16, bin2hex(str_repeat(chr(0), 32)));
  foreach ($links as $link) {
      $index = hexdec($link['bit']);
      $branchLinks[$index] = $link['link'];
  }
  $concatenated = $encodedPath . implode("", $branchLinks);
  $hasher = hash_init('sha3-256');
  hash_update($hasher, hex2bin($concatenated));

  return strtoupper(bin2hex(hash_final($hasher, true)));
}

// ワールドステートの検証

function checkState($stateProof, $stateHash, $pathHash, $rootHash) {
  $merkleLeaf = null;
  $merkleBranches = [];
  foreach($stateProof['tree'] as $n){
    if($n['type'] === 255){
      $merkleLeaf = $n;
    } else {
      $merkleBranches[] = $n;
    }
  }
  $merkleBranches = array_reverse($merkleBranches);
  $leafHash = getLeafHash($merkleLeaf['encoded_path'], $stateHash);

  $linkHash = $leafHash;  // リンクハッシュの初期値は葉ハッシュ
  $bit = "";
  for($i=0; $i <  count($merkleBranches); $i++){
    $branch = $merkleBranches[$i];
    $branchLink = array_filter($branch['links'], function($link) use ($linkHash) {
      return $link['link'] === $linkHash;
    });
    $branchLink = reset($branchLink); // 最初の要素を取得
    $linkHash = getBranchHash($branch['encoded_path'], $branch['links']);
    $bit = substr(
        $merkleBranches[$i]['path'],
        0,
        $merkleBranches[$i]['nibble_count']) . $branchLink['bit'] . $bit;
  }
  $treeRootHash = $linkHash; //最後のlinkHashはrootHash
  $treePathHash = $bit . $merkleLeaf['path'];
  if(strlen($treePathHash) % 2 == 1){
    $treePathHash = substr($treePathHash, 0, -1);
  }

  // 検証
  var_dump($treeRootHash === $rootHash);
  var_dump($treePathHash === $pathHash);

}

function hexToUint8($hex) {
  // 16進数文字列をバイナリデータに変換
  $binary = hex2bin($hex);
  // バイナリデータを配列に変換
  return array_values(unpack('C*', $binary));
}
//}

== 13.3.1 アカウント情報の検証

アカウント情報を葉として、マークルツリー上の分岐する枝をアドレスでたどり、ルートに到着できるかを確認します。

//emlist[][php]{
$aliceRawAddress = "TBIL6D6RURP45YQRWV6Q7YVWIIPLQGLZQFHWFEQ";
$aliceAddress = new Address($aliceRawAddress);

$hasher = hash_init('sha3-256');
$alicePathHash = hash_update($hasher, hex2bin($aliceAddress));
$alicePathHash = strtoupper(bin2hex(hash_final($hasher, true)));

$hasher = hash_init('sha3-256');
$accountApiInstance = new AccountRoutesApi($client, $config);
$aliceInfo = $accountApiInstance->getAccountInfo($aliceRawAddress);
$aliceInfo = $aliceInfo["account"];

// アカウント情報から StateHash を導出
$format = (int)$aliceInfo['importance'] === 0 || 
    strlen($aliceInfo['activity_buckets']) <5 ? 0x00 : 0x01;

$supplementalPublicKeysMask = 0x00;
$linkedPublicKey = [];
if($aliceInfo['supplemental_public_keys']['linked'] !== null){
  $supplementalPublicKeysMask |= 0x01;  // OR 演算子と代入演算子を組み合わせ
  $linkedPublicKey = hexToUint8(
    $aliceInfo['supplemental_public_keys']['linked']['public_key']
  );
}

$nodePublicKey = [];
if($aliceInfo['supplemental_public_keys']['node'] !== null){
  $supplementalPublicKeysMask |= 0x02;
  $nodePublicKey = hexToUint8(
    $aliceInfo['supplemental_public_keys']['node']['public_key']
  );
}

$vrfPublicKey = [];
if($aliceInfo['supplemental_public_keys']['vrf'] !== null){
  $supplementalPublicKeysMask |= 0x04;
  $vrfPublicKey = hexToUint8(
    $aliceInfo['supplemental_public_keys']['vrf']['public_key']
  );
}

$votingPublicKeys = [];
if($aliceInfo['supplemental_public_keys']['voting'] !== null){
  foreach($aliceInfo['supplemental_public_keys']['voting']['public_key'] as $key){
    $votingPublicKeys = array_merge(
      $votingPublicKeys, hexToUint8($key['public_key'])
    );
  }
}

$importanceSnapshots = [];
if((int)$aliceInfo['importance'] !== 0){
  $importanceSnapshots = array_merge(
    hexToUint8(reverseHex($aliceInfo['importance_snapshot'], 8)),
    hexToUint8(reverseHex($aliceInfo['importance_snapshot_height'], 8))
  );
}

$activityBuckets = [];
if((int)$aliceInfo['importance'] > 0){
  for (
    $idx = 0;
    $idx < count($aliceInfo['activity_buckets']) || $idx < 5;
    $idx++
  ) {
    $bucket = $aliceInfo['activity_buckets'][$idx];
    $activityBuckets = array_merge(
      $activityBuckets,
      hexToUint8(reverseHex($bucket['start_height'], 8)),
      hexToUint8(reverseHex($bucket['total_fees_paid'], 8)),
      hexToUint8(reverseHex($bucket['beneficiary_count'], 4)),
      hexToUint8(reverseHex($bucket['raw_score'], 8))
    );
  }
}

$balances = [];
if(count($aliceInfo['mosaics']) > 0){
  foreach($aliceInfo['mosaics'] as $mosaic){
    $balances = array_merge(
      $balances,
      hexToUint8(bin2hex(strrev(hex2bin($mosaic['id'])))),
      hexToUint8(reverseHex($mosaic['amount'], 8))
    );
  }
}

$accountInfoBytes = array_merge(
  hexToUint8(reverseHex($aliceInfo['version'], 2)),
  hexToUint8($aliceInfo['address']),
  hexToUint8(reverseHex($aliceInfo['address_height'], 8)),
  hexToUint8($aliceInfo['public_key']),
  hexToUint8(reverseHex($aliceInfo['public_key_height'], 8)),
  hexToUint8(reverseHex($aliceInfo['account_type'], 1)),
  hexToUint8(reverseHex($format, 1)),
  hexToUint8(reverseHex($supplementalPublicKeysMask, 1)),
  hexToUint8(reverseHex(count($votingPublicKeys), 1)),
  $linkedPublicKey,
  $nodePublicKey,
  $vrfPublicKey,
  $votingPublicKeys,
  $importanceSnapshots,
  $activityBuckets,
  hexToUint8(reverseHex(count($aliceInfo['mosaics']), 2)),
  $balances,
);

$accountInfoBytesString = implode('', array_map('chr', $accountInfoBytes));

hash_update($hasher, $accountInfoBytesString);
$aliceStateHash = strtoupper(bin2hex(hash_final($hasher, true)));

//サービス提供者以外のノードから最新のブロックヘッダー情報を取得
$blockInfo = $blockApiInstance->searchBlocks(order: 'desc');
$rootHash = $blockInfo['data'][0]['meta']['state_hash_sub_cache_merkle_roots'][0];

//サービス提供者を含む任意のノードからマークル情報を取得
$stateProof = $accountApiInstance->getAccountInfoMerkle($aliceRawAddress);

//検証
checkState($stateProof, $aliceStateHash, $alicePathHash, $rootHash);
//}

== 13.3.2 モザイクへ登録したメタデータの検証

モザイクに登録したメタデータValue値を葉として、マークルツリー上の分岐する枝をメタデータキーで構成されるハッシュ値でたどり、 ルートに到着できるかを確認します。

//emlist[][php]{
$srcAddress = new Address('TDSSDPIPAJHVRZTQUAR36OQU6O7MV4BIAOLL5UA');
$targetAddress = new Address('TDSSDPIPAJHVRZTQUAR36OQU6O7MV4BIAOLL5UA');

$scopeKey = Metadata::metadataGenerateKey('key_mosaic'); //メタデータキー
$scopeKey = strtoupper(dechex($scopeKey));
$targetId = '6FA40B0B8B9E392F'  ; //モザイクID

$hasher = hash_init('sha3-256');
hash_update($hasher, $srcAddress->binaryData);
hash_update($hasher, $targetAddress->binaryData);
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($scopeKey))));
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($targetId))));
hash_update($hasher, chr(1));

$compositeHash = hash_final($hasher, true);

$hasher = hash_init('sha3-256');
hash_update($hasher, $compositeHash);
$pathHash1 = strtoupper(bin2hex(hash_final($hasher, true)));

//stateHash(Value値)
$hasher = hash_init('sha3-256');
$version = 1;
hash_update($hasher, pack('C*', ...hexToUint8(reverseHex($version, 2)))); //version
hash_update($hasher, $srcAddress->binaryData);
hash_update($hasher, $targetAddress->binaryData);
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($scopeKey))));
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($targetId))));
hash_update($hasher, chr(1));

// 処理対象の文字列
$value = "test";
$length = strlen($value);
$hexLength = dechex($length);
$paddedHex = str_pad($hexLength, 4, "0", STR_PAD_LEFT);

hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($paddedHex))));
hash_update($hasher, $value);

$stateHash1 = strtoupper(bin2hex(hash_final($hasher, true)));

//サービス提供者以外のノードから最新のブロックヘッダー情報を取得
$blockInfo = $blockApiInstance->searchBlocks(order: 'desc');
$rootHash1 = $blockInfo['data'][0]['meta']['state_hash_sub_cache_merkle_roots'][8];

//サービス提供者を含む任意のノードからマークル情報を取得
$metadataApiInstance = new MetadataRoutesApi($client, $config);
$stateProof1 = $metadataApiInstance->getMetadataMerkle(bin2hex($compositeHash));

//検証

checkState($stateProof1, $stateHash1, $pathHash1, $rootHash1);
//}

=== 13.3.3 アカウントへ登録したメタデータの検証

アカウントに登録したメタデータValue値を葉として、マークルツリー上の分岐する枝をメタデータキーで構成されるハッシュ値でたどり、ルートに到着できるかを確認します。

//emlist[][php]{
$srcAddress = new Address('TDNH6IMNTNWAYRM7MXBFNGNGZRCFOQY5MSPTZUI');
$targetAddress = new Address('TDNH6IMNTNWAYRM7MXBFNGNGZRCFOQY5MSPTZUI');

//compositePathHash(Key値)
$scopeKey = Metadata::metadataGenerateKey('key_account'); //メタデータキー
$scopeKey = strtoupper(dechex($scopeKey));
$targetId = '0000000000000000';

$hasher = hash_init('sha3-256');
hash_update($hasher, $srcAddress->binaryData);
hash_update($hasher, $targetAddress->binaryData);
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($scopeKey))));
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($targetId))));
hash_update($hasher, chr(0)); // account

$compositeHash = hash_final($hasher, true);

$hasher = hash_init('sha3-256');
hash_update($hasher, $compositeHash);
$pathHash2 = strtoupper(bin2hex(hash_final($hasher, true)));

//stateHash(Value値)
$hasher = hash_init('sha3-256');
$version = 1;
hash_update($hasher, pack('C*', ...hexToUint8(reverseHex($version, 2)))); //version
hash_update($hasher, $srcAddress->binaryData);
hash_update($hasher, $targetAddress->binaryData);
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($scopeKey))));
hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($targetId))));
hash_update($hasher, chr(0)); // account

$value = "test";
$length = strlen($value);
$hexLength = dechex($length);
$paddedHex = str_pad($hexLength, 4, "0", STR_PAD_LEFT);

hash_update($hasher, pack('C*', ...array_reverse(hexToUint8($paddedHex))));
hash_update($hasher, $value);

$stateHash2 = strtoupper(bin2hex(hash_final($hasher, true)));

//サービス提供者以外のノードから最新のブロックヘッダー情報を取得
$blockInfo = $blockApiInstance->searchBlocks(order: 'desc');
$rootHash2 = $blockInfo['data'][0]['meta']['state_hash_sub_cache_merkle_roots'][8];

//サービス提供者を含む任意のノードからマークル情報を取得
$metadataApiInstance = new MetadataRoutesApi($client, $config);
$stateProof2 = $metadataApiInstance->getMetadataMerkle(bin2hex($compositeHash));

//検証

checkState($stateProof2, $stateHash2, $pathHash2, $rootHash2);
//}

== 13.4 現場で使えるヒント

=== トラステッドウェブ

トラステッドウェブを簡単に説明すると、全てをプラットフォーマーに依存せず、かつ全てを検証せずに済むWebの実現です。

本章の検証で分かることは、ブロックチェーンが持つすべての情報はブロックヘッダーのハッシュ値によって検証可能ということです。ブロックチェーンはみんなが認め合うブロックヘッダーの共有とそれを再現できるフルノードの存在で成り立っています。しかし、ブロックチェーンを活用したいあらゆるシーンでこれらを検証するための環境を維持しておくことは非常に困難です。最新のブロックヘッダーが複数の信頼できる機関から常時ブロードキャストされていれば、検証の手間を大きく省くことができますこのようなインフラが整えば、都会などの数千万人が密集する超過密地帯、あるいは基地局が十分に配置できない僻地や災害時の広域ネットワーク遮断時などブロックチェーンの能力を超えた場所においても信頼できる情報にアクセスできるようになります。

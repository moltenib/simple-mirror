<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>ConfirmationDialog</name>
    <message>
        <source>Notice</source>
        <translation>確認</translation>
    </message>
    <message>
        <source>WARNING: POSSIBLE DATA LOSS!</source>
        <translation>警告: データ損失の可能性があります</translation>
    </message>
    <message>
        <source>This is a one-way synchronization. The origin will remain unchanged, and any files in the destination folder that do not exist in the origin will be deleted.

By continuing, you confirm that the selected paths are correct, and that you accept your responsibility for any data loss under applicable law.</source>
        <translation>これは一方向の同期です。コピー元は変更されず、コピー先にあるファイルのうちコピー元に存在しないものは削除されます。

続行すると、選択したパスが正しいこと、および適用法に基づくデータ損失に関する責任を受け入れることを確認したものとみなされます。</translation>
    </message>
    <message>
        <source>I understand the risk, continue</source>
        <translation>リスクを理解しました、続行</translation>
    </message>
</context>
<context>
    <name>DirectoryChooserWidget</name>
    <message>
        <source>Select</source>
        <translation>選択</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <source>Simple Mirror</source>
        <translation>シンプルミラー</translation>
    </message>
    <message>
        <source>Origin:</source>
        <translation>コピー元:</translation>
    </message>
    <message>
        <source>The folder to be backed up.</source>
        <translation>バックアップ元のフォルダー。</translation>
    </message>
    <message>
        <source>Select origin folder</source>
        <translation>コピー元フォルダーを選択</translation>
    </message>
    <message>
        <source>Destination:</source>
        <translation>コピー先:</translation>
    </message>
    <message>
        <source>A folder inside a back-up medium.</source>
        <translation>バックアップメディア内のフォルダー。</translation>
    </message>
    <message>
        <source>Select destination folder</source>
        <translation>コピー先フォルダーを選択</translation>
    </message>
    <message>
        <source>Synchronize</source>
        <translation>同期</translation>
    </message>
    <message>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <source>Idle</source>
        <translation>待機中</translation>
    </message>
    <message>
        <source>Choose two directories to synchronize.</source>
        <translation>同期する2つのフォルダーを選択してください。</translation>
    </message>
    <message>
        <source>Stopped</source>
        <translation>停止済み</translation>
    </message>
    <message>
        <source>The program has stopped. Press the button to resume.</source>
        <translation>プログラムは停止しました。再開するにはボタンを押してください。</translation>
    </message>
    <message>
        <source>Done</source>
        <translation>完了</translation>
    </message>
    <message>
        <source>Mirroring complete.</source>
        <translation>ミラーリング処理が終了しました。</translation>
    </message>
    <message>
        <source>Failed</source>
        <translation>失敗</translation>
    </message>
    <message>
        <source>Synchronization failed with exit code %1.</source>
        <translation>同期は終了コード %1 で失敗しました。</translation>
    </message>
    <message>
        <source>Synchronization failed</source>
        <translation>同期に失敗しました</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>実行中...</translation>
    </message>
    <message>
        <source>Comparing the two folders...</source>
        <translation>2つのフォルダーを比較しています...</translation>
    </message>
    <message>
        <source>Stopping...</source>
        <translation>停止中...</translation>
    </message>
    <message>
        <source>rsync not found</source>
        <translation>rsync が見つかりません</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <source>Synchronization error</source>
        <translation>同期エラー</translation>
    </message>
    <message>
        <source>Please choose both origin and destination folders.</source>
        <translation>コピー元フォルダーとコピー先フォルダーの両方を選択してください。</translation>
    </message>
    <message>
        <source>Cannot access origin folder: %1</source>
        <translation>コピー元フォルダーにアクセスできません: %1</translation>
    </message>
    <message>
        <source>Cannot access destination folder: %1</source>
        <translation>コピー先フォルダーにアクセスできません: %1</translation>
    </message>
    <message>
        <source>Origin folder does not exist: %1</source>
        <translation>コピー元フォルダーが存在しません: %1</translation>
    </message>
    <message>
        <source>Destination folder does not exist: %1</source>
        <translation>コピー先フォルダーが存在しません: %1</translation>
    </message>
    <message>
        <source>Crashed</source>
        <translation>異常終了</translation>
    </message>
    <message>
        <source>The synchronization process crashed.</source>
        <translation>同期プロセスが異常終了しました。</translation>
    </message>
    <message>
        <source>The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale.</source>
        <translation>同期プロセスが異常終了しました。Windows では通常、同梱されている rsync ランタイムが不完全か古くなっていることを意味します。</translation>
    </message>
    <message>
        <source>Synchronization crashed</source>
        <translation>同期が異常終了しました</translation>
    </message>
</context>
<context>
    <name>RsyncRunner</name>
    <message>
        <source>A synchronization is already running.</source>
        <translation>すでに同期処理が実行中です。</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1</source>
        <translation>SIMPLE_MIRROR_RSYNC は設定されていますが、有効なファイルを指していません: %1</translation>
    </message>
    <message>
        <source>Deleting %1</source>
        <translation>%1 を削除中</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing (msys-2.0.dll not found near rsync, app directory, or working directory): %1</source>
        <translation>SIMPLE_MIRROR_RSYNC は rsync を指していますが、MSYS2 ランタイムが不足しています（msys-2.0.dll が rsync の近く、アプリケーションディレクトリ、または作業ディレクトリに見つかりません）: %1</translation>
    </message>
    <message>
        <source>Could not find bundled MSYS2 rsync. Run &quot;make bundle-rsync&quot; or &quot;make windows-all&quot;, or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.</source>
        <translation>バンドルされた MSYS2 rsync が見つかりません。&quot;make bundle-rsync&quot; または &quot;make windows-all&quot; を実行するか、SIMPLE_MIRROR_RSYNC に MSYS2 の rsync.exe を設定してください。</translation>
    </message>
    <message>
        <source>Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle &quot;runtime/bin/rsync&quot;.</source>
        <translation>rsync が見つかりません。SIMPLE_MIRROR_RSYNC を設定するか、rsync を PATH に追加するか、&quot;runtime/bin/rsync&quot; を同梱してください。</translation>
    </message>
</context>
<context>
    <name>WelcomeDialog</name>
    <message>
        <source>Welcome!</source>
        <translation>ようこそ！</translation>
    </message>
    <message>
        <source>&lt;p&gt;It appears that no backups have been performed with this installation yet. How it works:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choose an origin. This is the folder to be backed up. It will not be modified by this program.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Insert a back-up medium, like an external drive. This will be the destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Press &lt;span class=&apos;accent&apos;&gt;&amp;quot;Synchronize&amp;quot;&lt;/span&gt; to turn the destination into a mirror of the origin. This is irreversible. Use with care!&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;The two folders are compared.&lt;/li&gt;&lt;li&gt;Files in the destination that do not exist in the origin will be &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;permanently deleted&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Files in the origin that are missing in the destination are copied from the origin.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;This message will disappear after the first run.&lt;/p&gt;</source>
        <translation>&lt;p&gt;このインストールでは、まだバックアップが一度も実行されていないようです。使い方は次のとおりです。&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; コピー元を選択します。これはバックアップ対象のフォルダーで、このプログラムが変更することはありません。&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; 外付けドライブなどのバックアップ媒体を接続します。これがコピー先になります。&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; &lt;span class=&apos;accent&apos;&gt;「同期」&lt;/span&gt; を押すと、コピー先をコピー元のミラーにします。この操作は元に戻せません。注意してご利用ください。&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;2つのフォルダーを比較します。&lt;/li&gt;&lt;li&gt;コピー先にありコピー元にないファイルは &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;完全に削除されます&lt;/span&gt;。&lt;/li&gt;&lt;li&gt;コピー元にありコピー先にないファイルは、コピー元からコピーされます。&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;このメッセージは初回実行後に表示されなくなります。&lt;/p&gt;</translation>
    </message>
</context>
</TS>

<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>ConfirmationDialog</name>
    <message>
        <source>Notice</source>
        <translation>提示</translation>
    </message>
    <message>
        <source>WARNING: POSSIBLE DATA LOSS!</source>
        <translation>警告：可能发生数据丢失</translation>
    </message>
    <message>
        <source>This is a one-way synchronization. The origin will remain unchanged, and any files in the destination folder that do not exist in the origin will be deleted.

By continuing, you confirm that the selected paths are correct, and that you accept your responsibility for any data loss under applicable law.</source>
        <translation>这是单向同步。源目录将保持不变，目标目录中任何在源目录中不存在的文件都将被删除。

继续即表示您确认所选路径正确，并且您接受在适用法律下对任何数据丢失承担责任。</translation>
    </message>
    <message>
        <source>I understand the risk, continue</source>
        <translation>我理解风险，继续</translation>
    </message>
</context>
<context>
    <name>DirectoryChooserWidget</name>
    <message>
        <source>Select</source>
        <translation>选择</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <source>Simple Mirror</source>
        <translation>简易镜像</translation>
    </message>
    <message>
        <source>Origin:</source>
        <translation>源目录：</translation>
    </message>
    <message>
        <source>The folder to be backed up.</source>
        <translation>要备份的文件夹。</translation>
    </message>
    <message>
        <source>Select origin folder</source>
        <translation>选择源目录</translation>
    </message>
    <message>
        <source>Destination:</source>
        <translation>目标目录：</translation>
    </message>
    <message>
        <source>A folder inside a back-up medium.</source>
        <translation>备份介质中的一个文件夹。</translation>
    </message>
    <message>
        <source>Select destination folder</source>
        <translation>选择目标目录</translation>
    </message>
    <message>
        <source>Synchronize</source>
        <translation>同步</translation>
    </message>
    <message>
        <source>Stop</source>
        <translation>停止</translation>
    </message>
    <message>
        <source>Idle</source>
        <translation>空闲</translation>
    </message>
    <message>
        <source>Choose two directories to synchronize.</source>
        <translation>请选择两个要同步的文件夹。</translation>
    </message>
    <message>
        <source>Stopped</source>
        <translation>已停止</translation>
    </message>
    <message>
        <source>The program has stopped. Press the button to resume.</source>
        <translation>程序已停止。按下按钮以继续。</translation>
    </message>
    <message>
        <source>Done</source>
        <translation>完成</translation>
    </message>
    <message>
        <source>Mirroring complete.</source>
        <translation>镜像任务结束。</translation>
    </message>
    <message>
        <source>Failed</source>
        <translation>失败</translation>
    </message>
    <message>
        <source>Synchronization failed with exit code %1.</source>
        <translation>同步失败，退出代码为 %1。</translation>
    </message>
    <message>
        <source>Synchronization failed</source>
        <translation>同步失败</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>正在运行...</translation>
    </message>
    <message>
        <source>Comparing the two folders...</source>
        <translation>正在比较两个文件夹...</translation>
    </message>
    <message>
        <source>Stopping...</source>
        <translation>正在停止...</translation>
    </message>
    <message>
        <source>rsync not found</source>
        <translation>未找到 rsync</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <source>Synchronization error</source>
        <translation>同步错误</translation>
    </message>
    <message>
        <source>Please choose both origin and destination folders.</source>
        <translation>请选择源目录和目标目录。</translation>
    </message>
    <message>
        <source>Cannot access origin folder: %1</source>
        <translation>无法访问源目录：%1</translation>
    </message>
    <message>
        <source>Cannot access destination folder: %1</source>
        <translation>无法访问目标目录：%1</translation>
    </message>
    <message>
        <source>Origin folder does not exist: %1</source>
        <translation>源目录不存在：%1</translation>
    </message>
    <message>
        <source>Destination folder does not exist: %1</source>
        <translation>目标目录不存在：%1</translation>
    </message>
    <message>
        <source>Crashed</source>
        <translation>崩溃</translation>
    </message>
    <message>
        <source>The synchronization process crashed.</source>
        <translation>同步进程已崩溃。</translation>
    </message>
    <message>
        <source>The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale.</source>
        <translation>同步进程已崩溃。在 Windows 上，这通常意味着捆绑的 rsync 运行时不完整或已过期。</translation>
    </message>
    <message>
        <source>Synchronization crashed</source>
        <translation>同步已崩溃</translation>
    </message>
</context>
<context>
    <name>RsyncRunner</name>
    <message>
        <source>A synchronization is already running.</source>
        <translation>已有同步任务正在运行。</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1</source>
        <translation>已设置 SIMPLE_MIRROR_RSYNC，但它没有指向有效文件：%1</translation>
    </message>
    <message>
        <source>Deleting %1</source>
        <translation>正在删除 %1</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing (msys-2.0.dll not found near rsync, app directory, or working directory): %1</source>
        <translation>SIMPLE_MIRROR_RSYNC 指向 rsync，但缺少 MSYS2 运行时（在 rsync 附近、应用目录或工作目录中未找到 msys-2.0.dll）：%1</translation>
    </message>
    <message>
        <source>Could not find bundled MSYS2 rsync. Run &quot;make bundle-rsync&quot; or &quot;make windows-all&quot;, or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.</source>
        <translation>未找到随附的 MSYS2 rsync。请运行 &quot;make bundle-rsync&quot; 或 &quot;make windows-all&quot;，或将 SIMPLE_MIRROR_RSYNC 设置为 MSYS2 的 rsync.exe。</translation>
    </message>
    <message>
        <source>Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle &quot;runtime/bin/rsync&quot;.</source>
        <translation>找不到 rsync。请设置 SIMPLE_MIRROR_RSYNC、将 rsync 添加到 PATH，或捆绑 &quot;runtime/bin/rsync&quot;。</translation>
    </message>
</context>
<context>
    <name>WelcomeDialog</name>
    <message>
        <source>Welcome!</source>
        <translation>欢迎！</translation>
    </message>
    <message>
        <source>&lt;p&gt;It appears that no backups have been performed with this installation yet. How it works:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choose an origin. This is the folder to be backed up. It will not be modified by this program.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Insert a back-up medium, like an external drive. This will be the destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Press &lt;span class=&apos;accent&apos;&gt;&amp;quot;Synchronize&amp;quot;&lt;/span&gt; to turn the destination into a mirror of the origin. This is irreversible. Use with care!&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;The two folders are compared.&lt;/li&gt;&lt;li&gt;Files in the destination that do not exist in the origin will be &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;permanently deleted&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Files in the origin that are missing in the destination are copied from the origin.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;This message will disappear after the first run.&lt;/p&gt;</source>
        <translation>&lt;p&gt;看起来此安装尚未执行过任何备份。使用方法如下：&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; 选择源目录。这是要备份的文件夹。本程序不会修改它。&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; 插入备份介质，例如外置硬盘。该位置将作为目标目录。&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; 点击 &lt;span class=&apos;accent&apos;&gt;「同步」&lt;/span&gt;，将目标目录变为源目录的镜像。此操作不可逆，请谨慎使用。&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;系统会比较两个文件夹。&lt;/li&gt;&lt;li&gt;目标目录中在源目录不存在的文件将被 &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;永久删除&lt;/span&gt;。&lt;/li&gt;&lt;li&gt;源目录中存在而目标目录缺失的文件会从源目录复制过去。&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;此消息将在首次运行后消失。&lt;/p&gt;</translation>
    </message>
</context>
</TS>

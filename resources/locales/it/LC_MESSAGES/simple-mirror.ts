<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="it_IT">
<context>
    <name>ConfirmationDialog</name>
    <message>
        <source>Notice</source>
        <translation>Avviso</translation>
    </message>
    <message>
        <source>WARNING: POSSIBLE DATA LOSS!</source>
        <translation>ATTENZIONE: POSSIBILE PERDITA DI DATI</translation>
    </message>
    <message>
        <source>This is a one-way synchronization. The origin will remain unchanged, and any files in the destination folder that do not exist in the origin will be deleted.

By continuing, you confirm that the selected paths are correct, and that you accept your responsibility for any data loss under applicable law.</source>
        <translation>Questa è una sincronizzazione unidirezionale. L&apos;origine rimarrà invariata e tutti i file nella cartella di destinazione che non esistono nell&apos;origine verranno eliminati.

Continuando, conferma che i percorsi selezionati sono corretti e che si assume la responsabilità per eventuali perdite di dati ai sensi della legge applicabile.</translation>
    </message>
    <message>
        <source>I understand the risk, continue</source>
        <translation>Comprendo il rischio, continuare</translation>
    </message>
</context>
<context>
    <name>DirectoryChooserWidget</name>
    <message>
        <source>Select</source>
        <translation>Seleziona</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <source>Simple Mirror</source>
        <translation>Specchio semplice</translation>
    </message>
    <message>
        <source>Origin:</source>
        <translation>Origine:</translation>
    </message>
    <message>
        <source>The folder to be backed up.</source>
        <translation>La cartella da salvare.</translation>
    </message>
    <message>
        <source>Select origin folder</source>
        <translation>Seleziona cartella di origine</translation>
    </message>
    <message>
        <source>Destination:</source>
        <translation>Destinazione:</translation>
    </message>
    <message>
        <source>A folder inside a back-up medium.</source>
        <translation>Una cartella all&apos;interno di un supporto di backup.</translation>
    </message>
    <message>
        <source>Select destination folder</source>
        <translation>Seleziona cartella di destinazione</translation>
    </message>
    <message>
        <source>Synchronize</source>
        <translation>Sincronizza</translation>
    </message>
    <message>
        <source>Stop</source>
        <translation>Ferma</translation>
    </message>
    <message>
        <source>Idle</source>
        <translation>Inattivo</translation>
    </message>
    <message>
        <source>Choose two directories to synchronize.</source>
        <translation>Seleziona due cartelle da sincronizzare.</translation>
    </message>
    <message>
        <source>Stopped</source>
        <translation>Fermato</translation>
    </message>
    <message>
        <source>The program has stopped. Press the button to resume.</source>
        <translation>Il programma si è fermato. Premi il pulsante per riprendere.</translation>
    </message>
    <message>
        <source>Done</source>
        <translation>Completato</translation>
    </message>
    <message>
        <source>Mirroring complete.</source>
        <translation>Specchiatura terminata.</translation>
    </message>
    <message>
        <source>Failed</source>
        <translation>Non riuscito</translation>
    </message>
    <message>
        <source>Synchronization failed with exit code %1.</source>
        <translation>La sincronizzazione non è riuscita con codice di uscita %1.</translation>
    </message>
    <message>
        <source>Synchronization failed</source>
        <translation>Sincronizzazione non riuscita</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>In esecuzione...</translation>
    </message>
    <message>
        <source>Comparing the two folders...</source>
        <translation>Confronto delle due cartelle...</translation>
    </message>
    <message>
        <source>Stopping...</source>
        <translation>Arresto in corso...</translation>
    </message>
    <message>
        <source>rsync not found</source>
        <translation>rsync non trovato</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>Errore</translation>
    </message>
    <message>
        <source>Synchronization error</source>
        <translation>Errore di sincronizzazione</translation>
    </message>
    <message>
        <source>Please choose both origin and destination folders.</source>
        <translation>Seleziona sia la cartella di origine sia quella di destinazione.</translation>
    </message>
    <message>
        <source>Cannot access origin folder: %1</source>
        <translation>Impossibile accedere alla cartella di origine: %1</translation>
    </message>
    <message>
        <source>Cannot access destination folder: %1</source>
        <translation>Impossibile accedere alla cartella di destinazione: %1</translation>
    </message>
    <message>
        <source>Origin folder does not exist: %1</source>
        <translation>La cartella di origine non esiste: %1</translation>
    </message>
    <message>
        <source>Destination folder does not exist: %1</source>
        <translation>La cartella di destinazione non esiste: %1</translation>
    </message>
    <message>
        <source>Crashed</source>
        <translation>Arrestato</translation>
    </message>
    <message>
        <source>The synchronization process crashed.</source>
        <translation>Il processo di sincronizzazione si è arrestato in modo anomalo.</translation>
    </message>
    <message>
        <source>The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale.</source>
        <translation>Il processo di sincronizzazione si è arrestato in modo anomalo. In Windows questo di solito significa che il runtime rsync incluso è incompleto o obsoleto.</translation>
    </message>
    <message>
        <source>Synchronization crashed</source>
        <translation>Sincronizzazione arrestata</translation>
    </message>
</context>
<context>
    <name>RsyncRunner</name>
    <message>
        <source>A synchronization is already running.</source>
        <translation>È già in esecuzione una sincronizzazione.</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1</source>
        <translation>SIMPLE_MIRROR_RSYNC è impostata ma non punta a un file valido: %1</translation>
    </message>
    <message>
        <source>Deleting %1</source>
        <translation>Eliminazione di %1</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing (msys-2.0.dll not found near rsync, app directory, or working directory): %1</source>
        <translation>SIMPLE_MIRROR_RSYNC punta a rsync, ma il runtime MSYS2 è mancante (msys-2.0.dll non trovata vicino a rsync, nella cartella dell&apos;applicazione o nella cartella di lavoro): %1</translation>
    </message>
    <message>
        <source>Could not find bundled MSYS2 rsync. Run &quot;make bundle-rsync&quot; or &quot;make windows-all&quot;, or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.</source>
        <translation>Impossibile trovare il rsync MSYS2 incluso. Eseguire &quot;make bundle-rsync&quot; o &quot;make windows-all&quot;, oppure impostare SIMPLE_MIRROR_RSYNC su un rsync.exe di MSYS2.</translation>
    </message>
    <message>
        <source>Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle &quot;runtime/bin/rsync&quot;.</source>
        <translation>Impossibile trovare rsync. Impostare SIMPLE_MIRROR_RSYNC, aggiungere rsync al PATH oppure includere &quot;runtime/bin/rsync&quot;.</translation>
    </message>
</context>
<context>
    <name>WelcomeDialog</name>
    <message>
        <source>Welcome!</source>
        <translation>Benvenuto!</translation>
    </message>
    <message>
        <source>&lt;p&gt;It appears that no backups have been performed with this installation yet. How it works:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choose an origin. This is the folder to be backed up. It will not be modified by this program.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Insert a back-up medium, like an external drive. This will be the destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Press &lt;span class=&apos;accent&apos;&gt;&amp;quot;Synchronize&amp;quot;&lt;/span&gt; to turn the destination into a mirror of the origin. This is irreversible. Use with care!&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;The two folders are compared.&lt;/li&gt;&lt;li&gt;Files in the destination that do not exist in the origin will be &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;permanently deleted&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Files in the origin that are missing in the destination are copied from the origin.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;This message will disappear after the first run.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Sembra che con questa installazione non sia ancora stato eseguito alcun backup. Ecco come funziona:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Scelga un&apos;origine. È la cartella da salvare. Questo programma non la modificherà.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Colleghi un supporto di backup, ad esempio un&apos;unità esterna. Questa sarà la destinazione.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Prema &lt;span class=&apos;accent&apos;&gt;«Sincronizza»&lt;/span&gt; per trasformare la destinazione in uno specchio dell&apos;origine. Questa operazione è irreversibile. La usi con cautela.&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;Le due cartelle vengono confrontate.&lt;/li&gt;&lt;li&gt;I file nella destinazione che non esistono nell&apos;origine saranno &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;eliminati definitivamente&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;I file presenti nell&apos;origine ma mancanti nella destinazione vengono copiati dall&apos;origine.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;Questo messaggio scomparirà dopo la prima esecuzione.&lt;/p&gt;</translation>
    </message>
</context>
</TS>

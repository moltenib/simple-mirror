<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>ConfirmationDialog</name>
    <message>
        <source>Notice</source>
        <translation>Avertissement</translation>
    </message>
    <message>
        <source>WARNING: POSSIBLE DATA LOSS!</source>
        <translation>AVERTISSEMENT : PERTE DE DONNÉES POSSIBLE</translation>
    </message>
    <message>
        <source>This is a one-way synchronization. The origin will remain unchanged, and any files in the destination folder that do not exist in the origin will be deleted.

By continuing, you confirm that the selected paths are correct, and that you accept your responsibility for any data loss under applicable law.</source>
        <translation>Il s&apos;agit d&apos;une synchronisation unidirectionnelle. Le dossier source restera inchangé et tous les fichiers du dossier de destination qui n&apos;existent pas dans le dossier source seront supprimés.

En continuant, vous confirmez que les chemins sélectionnés sont corrects et que vous acceptez votre responsabilité en cas de perte de données au regard du droit applicable.</translation>
    </message>
    <message>
        <source>I understand the risk, continue</source>
        <translation>Je comprends le risque, continuer</translation>
    </message>
</context>
<context>
    <name>DirectoryChooserWidget</name>
    <message>
        <source>Select</source>
        <translation>Sélectionner</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <source>Simple Mirror</source>
        <translation>Miroir simple</translation>
    </message>
    <message>
        <source>Origin:</source>
        <translation>Origine :</translation>
    </message>
    <message>
        <source>The folder to be backed up.</source>
        <translation>Le dossier à sauvegarder.</translation>
    </message>
    <message>
        <source>Select origin folder</source>
        <translation>Sélectionner le dossier d&apos;origine</translation>
    </message>
    <message>
        <source>Destination:</source>
        <translation>Destination :</translation>
    </message>
    <message>
        <source>A folder inside a back-up medium.</source>
        <translation>Un dossier sur un support de sauvegarde.</translation>
    </message>
    <message>
        <source>Select destination folder</source>
        <translation>Sélectionner le dossier de destination</translation>
    </message>
    <message>
        <source>Synchronize</source>
        <translation>Synchroniser</translation>
    </message>
    <message>
        <source>Stop</source>
        <translation>Arrêter</translation>
    </message>
    <message>
        <source>Idle</source>
        <translation>Inactif</translation>
    </message>
    <message>
        <source>Choose two directories to synchronize.</source>
        <translation>Choisissez deux dossiers à synchroniser.</translation>
    </message>
    <message>
        <source>Stopped</source>
        <translation>Arrêté</translation>
    </message>
    <message>
        <source>The program has stopped. Press the button to resume.</source>
        <translation>Le programme s&apos;est arrêté. Appuyez sur le bouton pour reprendre.</translation>
    </message>
    <message>
        <source>Done</source>
        <translation>Terminé</translation>
    </message>
    <message>
        <source>Mirroring complete.</source>
        <translation>Mise en miroir achevée.</translation>
    </message>
    <message>
        <source>Failed</source>
        <translation>Échec</translation>
    </message>
    <message>
        <source>Synchronization failed with exit code %1.</source>
        <translation>La synchronisation a échoué avec le code de sortie %1.</translation>
    </message>
    <message>
        <source>Synchronization failed</source>
        <translation>Échec de la synchronisation</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>En cours...</translation>
    </message>
    <message>
        <source>Comparing the two folders...</source>
        <translation>Comparaison des deux dossiers...</translation>
    </message>
    <message>
        <source>Stopping...</source>
        <translation>Arrêt en cours...</translation>
    </message>
    <message>
        <source>rsync not found</source>
        <translation>rsync introuvable</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>Erreur</translation>
    </message>
    <message>
        <source>Synchronization error</source>
        <translation>Erreur de synchronisation</translation>
    </message>
    <message>
        <source>Please choose both origin and destination folders.</source>
        <translation>Veuillez sélectionner les dossiers d&apos;origine et de destination.</translation>
    </message>
    <message>
        <source>Cannot access origin folder: %1</source>
        <translation>Impossible d&apos;accéder au dossier d&apos;origine : %1</translation>
    </message>
    <message>
        <source>Cannot access destination folder: %1</source>
        <translation>Impossible d&apos;accéder au dossier de destination : %1</translation>
    </message>
    <message>
        <source>Origin folder does not exist: %1</source>
        <translation>Le dossier d&apos;origine n&apos;existe pas : %1</translation>
    </message>
    <message>
        <source>Destination folder does not exist: %1</source>
        <translation>Le dossier de destination n&apos;existe pas : %1</translation>
    </message>
    <message>
        <source>Crashed</source>
        <translation>Planté</translation>
    </message>
    <message>
        <source>The synchronization process crashed.</source>
        <translation>Le processus de synchronisation a planté.</translation>
    </message>
    <message>
        <source>The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale.</source>
        <translation>Le processus de synchronisation a planté. Sous Windows, cela signifie généralement que l&apos;environnement d&apos;exécution rsync fourni est incomplet ou obsolète.</translation>
    </message>
    <message>
        <source>Synchronization crashed</source>
        <translation>Synchronisation plantée</translation>
    </message>
</context>
<context>
    <name>RsyncRunner</name>
    <message>
        <source>A synchronization is already running.</source>
        <translation>Une synchronisation est déjà en cours.</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1</source>
        <translation>SIMPLE_MIRROR_RSYNC est défini mais ne pointe pas vers un fichier valide : %1</translation>
    </message>
    <message>
        <source>Deleting %1</source>
        <translation>Suppression de %1</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing (msys-2.0.dll not found near rsync, app directory, or working directory): %1</source>
        <translation>SIMPLE_MIRROR_RSYNC pointe vers rsync, mais l&apos;environnement d&apos;exécution MSYS2 est manquant (msys-2.0.dll introuvable près de rsync, dans le dossier de l&apos;application ou dans le dossier de travail) : %1</translation>
    </message>
    <message>
        <source>Could not find bundled MSYS2 rsync. Run &quot;make bundle-rsync&quot; or &quot;make windows-all&quot;, or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.</source>
        <translation>Impossible de trouver le rsync MSYS2 fourni. Exécutez &quot;make bundle-rsync&quot; ou &quot;make windows-all&quot;, ou définissez SIMPLE_MIRROR_RSYNC vers un rsync.exe MSYS2.</translation>
    </message>
    <message>
        <source>Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle &quot;runtime/bin/rsync&quot;.</source>
        <translation>Impossible de trouver rsync. Définissez SIMPLE_MIRROR_RSYNC, ajoutez rsync au PATH, ou incluez &quot;runtime/bin/rsync&quot;.</translation>
    </message>
</context>
<context>
    <name>WelcomeDialog</name>
    <message>
        <source>Welcome!</source>
        <translation>Bienvenue !</translation>
    </message>
    <message>
        <source>&lt;p&gt;It appears that no backups have been performed with this installation yet. How it works:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choose an origin. This is the folder to be backed up. It will not be modified by this program.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Insert a back-up medium, like an external drive. This will be the destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Press &lt;span class=&apos;accent&apos;&gt;&amp;quot;Synchronize&amp;quot;&lt;/span&gt; to turn the destination into a mirror of the origin. This is irreversible. Use with care!&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;The two folders are compared.&lt;/li&gt;&lt;li&gt;Files in the destination that do not exist in the origin will be &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;permanently deleted&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Files in the origin that are missing in the destination are copied from the origin.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;This message will disappear after the first run.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Il semble qu&apos;aucune sauvegarde n&apos;ait encore été effectuée avec cette installation. Voici comment cela fonctionne :&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choisissez une source. C&apos;est le dossier à sauvegarder. Ce programme ne le modifiera pas.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Branchez un support de sauvegarde, par exemple un disque externe. Ce sera la destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Appuyez sur &lt;span class=&apos;accent&apos;&gt;« Synchroniser »&lt;/span&gt; pour transformer la destination en miroir de la source. Cette action est irréversible. Utilisez-la avec prudence.&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;Les deux dossiers sont comparés.&lt;/li&gt;&lt;li&gt;Les fichiers présents dans la destination mais absents de la source seront &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;supprimés définitivement&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Les fichiers présents dans la source mais absents de la destination sont copiés depuis la source.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;Ce message disparaîtra après la première exécution.&lt;/p&gt;</translation>
    </message>
</context>
</TS>

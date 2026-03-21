<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es_ES">
<context>
    <name>ConfirmationDialog</name>
    <message>
        <source>Notice</source>
        <translation>Aviso</translation>
    </message>
    <message>
        <source>WARNING: POSSIBLE DATA LOSS!</source>
        <translation>ADVERTENCIA: POSIBLE PÉRDIDA DE DATOS</translation>
    </message>
    <message>
        <source>This is a one-way synchronization. The origin will remain unchanged, and any files in the destination folder that do not exist in the origin will be deleted.

By continuing, you confirm that the selected paths are correct, and that you accept your responsibility for any data loss under applicable law.</source>
        <translation>Esta es una sincronización unidireccional. El origen permanecerá sin cambios, y todo archivo en la carpeta de destino que no exista en el origen será eliminado.

Al continuar, Ud. confirma que las rutas son correctas, y que asume su responsabilidad por pérdida de datos según la ley aplicable.</translation>
    </message>
    <message>
        <source>I understand the risk, continue</source>
        <translation>Comprendo el riesgo, continuar</translation>
    </message>
</context>
<context>
    <name>DirectoryChooserWidget</name>
    <message>
        <source>Select</source>
        <translation>Seleccionar</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <source>Simple Mirror</source>
        <translation>Espejo simple</translation>
    </message>
    <message>
        <source>Origin:</source>
        <translation>Origen:</translation>
    </message>
    <message>
        <source>The folder to be backed up.</source>
        <translation>La carpeta que se va a respaldar.</translation>
    </message>
    <message>
        <source>Select origin folder</source>
        <translation>Seleccionar carpeta de origen</translation>
    </message>
    <message>
        <source>Destination:</source>
        <translation>Destino:</translation>
    </message>
    <message>
        <source>A folder inside a back-up medium.</source>
        <translation>Una carpeta dentro de un medio de copia de seguridad.</translation>
    </message>
    <message>
        <source>Select destination folder</source>
        <translation>Seleccionar carpeta de destino</translation>
    </message>
    <message>
        <source>Synchronize</source>
        <translation>Sincronizar</translation>
    </message>
    <message>
        <source>Stop</source>
        <translation>Detener</translation>
    </message>
    <message>
        <source>Idle</source>
        <translation>Inactivo</translation>
    </message>
    <message>
        <source>Choose two directories to synchronize.</source>
        <translation>Seleccione dos carpetas para sincronizar.</translation>
    </message>
    <message>
        <source>Stopped</source>
        <translation>Detenido</translation>
    </message>
    <message>
        <source>The program has stopped. Press the button to resume.</source>
        <translation>El programa se ha detenido. Pulse el botón para reanudar.</translation>
    </message>
    <message>
        <source>Done</source>
        <translation>Completado</translation>
    </message>
    <message>
        <source>Mirroring complete.</source>
        <translation>Espejado terminado.</translation>
    </message>
    <message>
        <source>Failed</source>
        <translation>Fallido</translation>
    </message>
    <message>
        <source>Synchronization failed with exit code %1.</source>
        <translation>La sincronización falló con el código de salida %1.</translation>
    </message>
    <message>
        <source>Synchronization failed</source>
        <translation>Sincronización fallida</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>En ejecución...</translation>
    </message>
    <message>
        <source>Comparing the two folders...</source>
        <translation>Comparando las dos carpetas...</translation>
    </message>
    <message>
        <source>Stopping...</source>
        <translation>Deteniendo...</translation>
    </message>
    <message>
        <source>rsync not found</source>
        <translation>rsync no encontrado</translation>
    </message>
    <message>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <source>Synchronization error</source>
        <translation>Error de sincronización</translation>
    </message>
    <message>
        <source>Please choose both origin and destination folders.</source>
        <translation>Seleccione las carpetas de origen y destino.</translation>
    </message>
    <message>
        <source>Cannot access origin folder: %1</source>
        <translation>No se puede acceder a la carpeta de origen: %1</translation>
    </message>
    <message>
        <source>Cannot access destination folder: %1</source>
        <translation>No se puede acceder a la carpeta de destino: %1</translation>
    </message>
    <message>
        <source>Origin folder does not exist: %1</source>
        <translation>La carpeta de origen no existe: %1</translation>
    </message>
    <message>
        <source>Destination folder does not exist: %1</source>
        <translation>La carpeta de destino no existe: %1</translation>
    </message>
    <message>
        <source>Crashed</source>
        <translation>Bloqueado</translation>
    </message>
    <message>
        <source>The synchronization process crashed.</source>
        <translation>El proceso de sincronización se bloqueó.</translation>
    </message>
    <message>
        <source>The synchronization process crashed. On Windows this usually means the bundled rsync runtime is incomplete or stale.</source>
        <translation>El proceso de sincronización se bloqueó. En Windows esto suele significar que el entorno de ejecución rsync incluido está incompleto o desactualizado.</translation>
    </message>
    <message>
        <source>Synchronization crashed</source>
        <translation>La sincronización se bloqueó</translation>
    </message>
</context>
<context>
    <name>RsyncRunner</name>
    <message>
        <source>A synchronization is already running.</source>
        <translation>Ya hay una sincronización en ejecución.</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC is set but does not point to a valid file: %1</source>
        <translation>SIMPLE_MIRROR_RSYNC está definida pero no apunta a un archivo válido: %1</translation>
    </message>
    <message>
        <source>Deleting %1</source>
        <translation>Eliminando %1</translation>
    </message>
    <message>
        <source>SIMPLE_MIRROR_RSYNC points to rsync but MSYS2 runtime is missing (msys-2.0.dll not found near rsync, app directory, or working directory): %1</source>
        <translation>SIMPLE_MIRROR_RSYNC apunta a rsync, pero falta el entorno de ejecución de MSYS2 (no se encontró msys-2.0.dll junto a rsync, en el directorio de la aplicación ni en el directorio de trabajo): %1</translation>
    </message>
    <message>
        <source>Could not find bundled MSYS2 rsync. Run &quot;make bundle-rsync&quot; or &quot;make windows-all&quot;, or set SIMPLE_MIRROR_RSYNC to an MSYS2 rsync.exe.</source>
        <translation>No se encontró el rsync de MSYS2 incluido. Ejecute &quot;make bundle-rsync&quot; o &quot;make windows-all&quot;, o configure SIMPLE_MIRROR_RSYNC con una ruta a un rsync.exe de MSYS2.</translation>
    </message>
    <message>
        <source>Could not find rsync. Set SIMPLE_MIRROR_RSYNC, add rsync to PATH, or bundle &quot;runtime/bin/rsync&quot;.</source>
        <translation>No se pudo encontrar rsync. Configure SIMPLE_MIRROR_RSYNC, añada rsync al PATH, o incluya &quot;runtime/bin/rsync&quot;.</translation>
    </message>
</context>
<context>
    <name>WelcomeDialog</name>
    <message>
        <source>Welcome!</source>
        <translation>¡Bienvenido!</translation>
    </message>
    <message>
        <source>&lt;p&gt;It appears that no backups have been performed with this installation yet. How it works:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Choose an origin. This is the folder to be backed up. It will not be modified by this program.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Insert a back-up medium, like an external drive. This will be the destination.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Press &lt;span class=&apos;accent&apos;&gt;&amp;quot;Synchronize&amp;quot;&lt;/span&gt; to turn the destination into a mirror of the origin. This is irreversible. Use with care!&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;The two folders are compared.&lt;/li&gt;&lt;li&gt;Files in the destination that do not exist in the origin will be &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;permanently deleted&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Files in the origin that are missing in the destination are copied from the origin.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;This message will disappear after the first run.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Parece que con esta instalación todavía no se ha realizado ninguna copia de seguridad. Así funciona:&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;1)&lt;/span&gt; Elija un origen. Es la carpeta que se va a respaldar. Este programa no la modificará.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;2)&lt;/span&gt; Conecte un medio de respaldo, por ejemplo una unidad externa. Ese será el destino.&lt;/p&gt;&lt;p&gt;&lt;span class=&apos;accent&apos;&gt;3)&lt;/span&gt; Pulse &lt;span class=&apos;accent&apos;&gt;«Sincronizar»&lt;/span&gt; para convertir el destino en un espejo del origen. Esta acción es irreversible. Úsela con cuidado.&lt;/p&gt;&lt;ul style=&apos;margin-top: 0px;&apos;&gt;&lt;li&gt;Se comparan ambas carpetas.&lt;/li&gt;&lt;li&gt;Los archivos del destino que no existan en el origen se &lt;span style=&apos;color: #b51616; font-weight: 600;&apos;&gt;eliminarán de forma permanente&lt;/span&gt;.&lt;/li&gt;&lt;li&gt;Los archivos del origen que falten en el destino se copian desde el origen.&lt;/li&gt;&lt;/ul&gt;&lt;p&gt;Este mensaje desaparecerá después de la primera ejecución.&lt;/p&gt;</translation>
    </message>
</context>
</TS>

within InterProcessCommunication.SharedMemory;

function SharedMemoryRWEnd
  extends Modelica.Icons.Function;
    external "C" shmEnd();
    annotation(
    Include = "#include \"ShmMI_Win.c\"", Library="rt", Documentation(info = "<html>
<p>
<b>Inter Process Communication Library V1.0</b><br /><br />
<b>SharedMemoryRWEnd</b> Function ends data sharing from the shared memory using external C function shmEnd(). The shmEnd() function closes shared memory file mappings which share data between two processes.
</p>
</html>"),
    Diagram(graphics = {Bitmap(extent = {{-62, 66}, {-62, 66}})}));

end SharedMemoryRWEnd;

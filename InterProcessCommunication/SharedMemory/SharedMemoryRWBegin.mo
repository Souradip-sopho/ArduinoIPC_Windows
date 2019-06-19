within InterProcessCommunication.SharedMemory;

function SharedMemoryRWBegin
  extends Modelica.Icons.Function;
  output Integer err;
  
  external "C" err = shmBegin();
    annotation(
    Include = "#include \"ShmMI_Win.c\"", Library="rt", Documentation(info = "<html>
<p>
<b>Inter Process Communication Library V1.0</b><br /><br />
<b>SharedMemoryRead</b> Function starts data sharing from the shared memory using external C function shmBegin(). The shmBegin() function opens shared memory file mappings to share data between two processes. It returns zero on success and non-zero on failure.
</p>
</html>"),
    Diagram(graphics = {Bitmap(extent = {{-62, 66}, {-62, 66}})}));

end SharedMemoryRWBegin;

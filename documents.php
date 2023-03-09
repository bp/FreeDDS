<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <title>Data Dictionary System: Documents</title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <meta name="keywords" content="Data Dictionary System Free Seismic Processing System">
    <link rel="icon" href="images/favicon.ico" type="image/x-icon">
    <link rel="shortcut icon" href="images/favicon.ico" type="image/x-icon">
    <link href="styles.css" type="text/css" rel="stylesheet">
    <script src="javascript.js" language="JavaScript" type="text/javascript"></script>
  </head>
  <body>
    <script type="text/javascript">header("documents")</script>
    <!*********************************************************************>

    <center><h1>DDS Documents</h1></center>

   <?php
     // Get the files             
     $dir = substr(getenv("SCRIPT_FILENAME"),0,-4); 
     $handle = @opendir($dir);
     while ($file = @readdir($handle)) {
        if ($file == "." || $file == "..") continue;
        $files[]=$file;
     }
     closedir($handle);

     sort($files);

     $n = 0;
     foreach ($files as $file) {
	if ( $n == 0 ) {
           $output= "<table border=0 align=center cellpadding=5><thead><tr style='font: bold 14pt serif'>"
                   ."<th align=left>DOCUMENT FILE</th><th align=left>FILE SIZE</th></tr></thead><tbody>\n";
        }
        $n = $n + 1;
        $dfile = "${dir}/${file}";
        $size = intval(filesize($dfile)/(102.4*102.4))/100;
        $output.="<tr><td><b><a href='documents/$file'>$file</a></b></td><td>$size Mbytes</td></tr>\n";
     }

     if ( $n == 0 ) {
        echo "<center><h3>No Documents Available</h3></center>";
     } else {
        echo $output."</tbody></table>";
     }
           
     ?>

    <!*********************************************************************>
    <script type="text/javascript">footer()</script>
  </body>
</html>

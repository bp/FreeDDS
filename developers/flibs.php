<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <title>Data Dictionary System: Developers Fortran Application libraries</title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <meta name="keywords" 
      content="Data Dictionary System Free Seismic Processing System">
    <link rel="icon" href="../images/favicon.ico" type="image/x-icon">
    <link rel="shortcut icon" href="../images/favicon.ico" type="image/x-icon">
    <link href="../styles.css" type="text/css" rel="stylesheet">
    <script src="../javascript.js" language="JavaScript"
      type="text/javascript"></script>
  </head>
  <body>
    <script type="text/javascript">
      header("developers","flibs")
    </script>
    <!*********************************************************************>
    <script type="text/javascript">index_header("flibs","flibs")</script>

    <?php 
       $LIB = $_GET['lib'];
       $API = $_GET['api'];

       if ($LIB) {
         echo
	   "<img src='../images/leftArrow.gif' alt='' align='middle' /> ",
	   "<a href='flibs.php'>Fortran Application Libs</a>";
         if ($API) {
	   echo
             "<h1><img src='../images/dsktools.gif' alt=''> ",
             "$API (Library $LIB)</h1>";
         } else {
	   echo
             "<h1><img src='../images/book_q.gif' alt=''> ",
             "\"$LIB\" Application Library Overview</h1>";
         }

         echo 
           "<table><tr><td style='width:100px;vertical-align:top;",
           "background-color:#e0e0e0;padding:0px 10px 10px 10px;",
           "border:3px ridge #808080;'>",
           "<center style='font-weight:bold;font-size:18pt;",
           "text-decoration:underline;'>Index</center>";
 
	 $handle = @opendir("flibs/$LIB");
         $files = array();
	 while ($file = @readdir($handle)) $files[] = $file;
         closedir($handle);
         asort($files);

         foreach($files as $file) {
	   if ("$file" == "." || "$file" == "..") continue;
	   if (preg_match("/(.html)$/i", $file)) {
	     $api = str_replace(".html","",$file);
	     echo "<a href='flibs.php?lib=$LIB&amp;api=$api'>$api</a><br/>";
	   }
	 }
         echo "</td><td style='vertical-align:top;padding:0px 20px 0px 20px;font-size:14pt;'>";	

         if ($API) {
           include("flibs/${LIB}/${API}.html");
         } else {
           include("flibs/${LIB}.html");
         }
         echo "</td></tr></table>";	
       } else { 
	 // ************* List Application Libraries *************
	 echo 
	   "<h1><img src='../images/book_q.gif' alt=''>",
	   "Application Library Table of Contents</h1>",
	   "<p>These Fortran application libraries are created, installed ",
	   "and documented by various developers. Each library is ",
	   "documented separately. Many of these libraries may not be ",
	   "available for FreeDDS.</p><ul>";

	 $handle = @opendir("flibs");
         $files = array();
	 while ($file = @readdir($handle)) $files[] = $file;
         closedir($handle);
         asort($files);

         foreach($files as $file) {
           if (substr("$file",0,1) == ".") continue;
	   if (is_dir("flibs/$file")) {
	     echo "<li><a href='flibs.php?lib=$file'>$file</a><br/></li>";
	   }
	 }
	 echo
	   "</ul><hr /><i><h2>Instructions for adding a new Application ",
	   "Library to this webpage</h2>",
	   "<ol><li>Use the previous directories under 'flibs' ",
	   "as a template for 'Fortran' interface API routines ",
	   "(eg. flibs/lapack.html and flibs/lapack/...)</li>",
	   "<li>Optionally add any Fortran Templates.  For examples ",
	   "look at ftemlpates/f_template3.html</li></ol></i>";
       }
    ?>

    <script type="text/javascript">index_footer()</script>
    <!*********************************************************************>
    <script type="text/javascript">footer()</script>
  </body>
</html>

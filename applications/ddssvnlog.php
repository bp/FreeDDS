<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>
<html>
  <head>
    <title>Data Dictionary System: Applications</title>
    <meta http-equiv='Content-Type' 
      content='text/html; charset=iso-8859-1'>
    <link href='../styles.css' type='text/css' rel='stylesheet'>
    <script src='../javascript.js' language='JavaScript' 
      type='text/javascript'></script>
    <style type='text/css'>
      .list {border:1px inset;background-color:#edfeff;}</style>
  </head>
<body>

<?php
  /*
   ddssvnlog.php
   Written:  Jan 2011 - Jerry Ehlers 
 
   Arguments:
   query: print list of source files for program
   src:   print log information for a file
  */

$DDSROOT = "/hpc/tstapps/asi/src/dds";
//$DIR = "$DDSROOT/src/cmd";
$DIR = "/hpc/internal/asi/src/dds";
$SVN = "/hpc/tstapps/RHEL/2.6/x86_64/subversion/1.8.3/bin/svn";
$CONFIG = "/hpc/tstapps/RHEL/2.6/x86_64/subversion/1.8.3/etc";
$SVNDB = "file:///hpc/tstapps/svn/devel/ddsapps/svndb/dds";


////////////////////////////////////////////////////////////////////////
// printhead() 
////////////////////////////////////////////////////////////////////////
function printhead($lvl1,$lvl2)
{
   if (isset($lvl2)) {
      $lvl = ",'$lvl1','$lvl2'";
   } else {
      $lvl = ",'$lvl1'";
   }

   echo "<script type='text/javascript'>",
      "header('applications'${lvl})</script>";
}


////////////////////////////////////////////////////////////////////////
// printlist()  
////////////////////////////////////////////////////////////////////////
function printlist($query,$src)
{
   global $DIR, $SVN, $CONFIG, $SVNDB;
   
   $output = "<table align='center' border='1' cellspacing='1' "
      ."cellpadding='1' bgcolor='#03629C'><tr><td>"
      ."<table class='list' cellpadding='10'><tr>";

   $dir = $DIR;
   $n = $m = 0;
   $title = "<h2 style='text-align:center;'>"
      ."Source files for program: $query</h2>";
   $ncol = 4;
   $dir = "$dir/$query";
   $files = preg_replace("/\s/","^",`ls $dir`);
   $files = explode("^",$files);
   foreach ($files as $file) {
      if (strlen($file) <= 0) continue;
      $mklog = strpos($file,"mklog");
      if ("$mklog" == "0") continue;
      if ($file == "man1") {
         $mans = preg_replace("/\s/","^",`ls $dir/man1`);
         $mans = explode("^",$mans);
         foreach ($mans as $man) {
            if (!is_file("$dir/$file/$man")) continue; 
            if (!is_readable("$dir/$file/$man")) continue; 
            $n++; $m++;
            if ($n > $ncol) {
               $n -= $ncol;
               $output .= "</tr><tr>";
            }
            $output .= "<td class='list'>"
               ."<a href='ddssvnlog.php?query=$query&amp;src=$file/$man'>$man"
               ."</a></td>";
            if ("$src" == "$file/$man") {
               $path = "$dir/$file";
               $module = "$man";
            }
         }
         continue;   
      }
      if (!is_file("$dir/$file")) continue; 
      if (!is_readable("$dir/$file")) continue; 
      $n++; $m++;
      if ($n > $ncol) {
         $n -= $ncol;
         $output .= "</tr><tr>";
      }
      $output .= "<td class='list'>"
         ."<a href='ddssvnlog.php?query=$query&amp;src=$file'>$file"
         ."</a></td>";
      if ("$src" == "$file") {
         $path = "$dir";
         $module = "$file";
      }
   }
   if ($n == 0) {
      $output .= "<td>Nothing found with prgram: '$query'</td>";
   } else while($n < $ncol && $n != $m) {
      $n++;
      $output .= "<td class='list'>&nbsp;</td>";
   }
   $output .= "</tr></table></td></tr></table>";

   echo "$title";
   if ($m == 1) {
      $path = "$dir";
      $module = "$file";
   } else {
      echo "$output";
   }
   if (! $module) {
      $path = $DIR;
      $module = $query;
   }

   if (strlen("$path") > 0) {
      $file = substr($path,strlen($DIR));
      if ( $file ) {
         $file = substr("$file/$module",1);
      } else {
         $file = $module;
      }
      $cmd = "$SVN --config-dir $CONFIG log $SVNDB/$file";
      exec("$cmd",$output);
      if (count($output) > 0) { 
         echo "<h2 style='text-align:center;'>Log information for $file</h2><hr /><pre>\n";
         foreach ($output as $line) {
            $line = preg_replace("/[<]/", "&lt;", $line); 
            $line = preg_replace("/[>]/", "&gt;", $line);
            $line = preg_replace("/&([^\w;]+)/","&amp;\\1",$line);
            echo "$line\n";
         }
	 echo "</pre>\n";
      } else {
         echo "<h2 style='text-align:center;'>No Repository Information Available!</h2>";
      }      
   }
}


$query = (isset($_GET[query])) ? $_GET[query] : '';
$src   = (isset($_GET[src]))   ? $_GET[src]   : '';

ob_start();

printhead("Program: \"$query\"");
printlist($query,$src);

?>

<!*********************************************************************>
<script type="text/javascript">footer()</script>
</body>
</html>


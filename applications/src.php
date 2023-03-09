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
   src.php
   Written:  July 6, 2007 - Jerry Ehlers 
 
   Arguments:
   query: print list of source files for program
   src:   print fource file
  */

$DDSROOT = "/hpc/tstapps/asi/src/dds";
$dirs = array("$DDSROOT/src/cmd",
              "$DDSROOT/src/xtra",
              "$DDSROOT/src/scripts");


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
   global $dirs;
   
   $output = "<table align='center' border='1' cellspacing='1' "
      ."cellpadding='1' bgcolor='#03629C'><tr><td>"
      ."<table class='list' cellpadding='10'><tr>";

   $path = "";
   $n = $m = 0;
   foreach ($dirs as $dir) {
      if (is_file("$dir/$query")) {
         $title = "<h2 style='text-align:center;'>"
            ."Source file for script: $query</h2>";
         $ncol = 1;
         $files = $query;
      } else {
         $title = "<h2 style='text-align:center;'>"
            ."Source files for program: $query</h2>";
         $ncol = 4;
         $dir = "$dir/$query";
         $files = preg_replace("/\s/","^",`ls $dir`);
      }
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
                  ."<a href='src.php?query=$query&amp;src=$file/$man'>$man"
                  ."</a></td>";
               if ("$src" == "$file/$man") $path = "$dir/$file/$man";
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
            ."<a href='src.php?query=$query&amp;src=$file'>$file"
            ."</a></td>";
         if ("$src" == "$file") $path = "$dir/$file";
      }
   }
   if ($n == 0) {
      $output .= "<td>Nothing found with program: '$query'</td>";
   } else while($n < $ncol && $n != $m) {
      $n++;
      $output .= "<td class='list'>&nbsp;</td>";
   }
   $output .= "</tr></table></td></tr></table>";

   echo "$title";
   if ($m == 1) {
      $path = "$dir/$file";
   } else {
      echo "$output";
   }

   if (strlen("$path") > 0) {
      $cmd = "cat $path";
      $output = `$cmd`;
      $output = preg_replace("/[<]/", "&lt;", $output); 
      $output = preg_replace("/[>]/", "&gt;", $output);
      $output = preg_replace("/&([^\w;]+)/","&amp;\\1",$output);
      echo "<h2 style='text-align:center;'>File: $path</h2><hr /><pre>$output</pre>";
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


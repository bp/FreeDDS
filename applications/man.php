<?php
  /*
   man.php
   Written:  Feb 13, 2002 - Fawad Halim (http://www.linuxpakistan.net/man.php)
   Modified: Nov 30, 2006 - Jerry Ehlers 

   Code distributed under the GNU General Public License <http://www.gnu.org/copyleft/gpl.html>

   The interface was written to (largely) emulate the FreeBSD.org man
   interface. However, this program itself was written from scratch. This
   has been tested to run on Linux and FreeBSD 4.x. Please email me if you
   manage to run or port this code to some other platform.

   This program uses the Linux commands "man" and "col".

  */
  /*
   Arguments:
   topic: print list of apps based on (topic: *)
   progs: print list of apps based on comma separated list
   query: print man page $query
  */

  //Cusomizing variables
  // Unix path for DDS man pages
  //    This should follow the same convention as /usr/share/man etc
  //    i.e. should have directories named "manX" where X==[1-8] is the
  //    section number. Seperate multiple directories by a colon(:)
$web="none";
$manpath="/home/freeusp/DDS_new:/home/freeusp/DDS";
putenv("MANPATH=$manpath");
putenv("MANWIDTH=80");

$descmd = "awk '{if ($1 ~ /^[a-zA-Z0-9\\\\]/) {for(i=3;i<=NF;i++) print \$i;exit}}' $manpath/man1/";


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

   echo
      "<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>",
      "<html><head><title>Data Dictionary System: Applications</title>",
      "<meta http-equiv='Content-Type' ",
      "content='text/html; charset=iso-8859-1'>",
      "<link href='../styles.css' type='text/css' rel='stylesheet'>",
      "<script src='../javascript.js' language='JavaScript' ",
      "type='text/javascript'></script><style type='text/css'>",
      ".list {border:1px inset;background-color:#edfeff;}</style>",
      "</head><body><script type='text/javascript'>",
      "header('applications'${lvl})</script>";
}


////////////////////////////////////////////////////////////////////////
// printlist()  
////////////////////////////////////////////////////////////////////////
function printlist($progs)
{
   global $manpath, $descmd;

   $output = "<table align='center' border='1' cellspacing='1' ";
   $output .= "cellpadding='1' bgcolor='#03629C'><tr><td>";
   $output .= "<table class='list' cellpadding='3'>";

   if (strlen($progs) < 1) {
      $cmd = "cd $manpath/man1;ls  *.1";
      $progs = `$cmd`;
      $progs = explode(".1",$progs);
      $n = 0;
      $output .= "<tr>";
      foreach ($progs as $prog) {
         $prog = preg_replace("/\s/","",$prog);
         if ($prog == "") continue;
         $n++;
         if ($n > 4) {
            $n -= 4;
            $output .= "</tr><tr>";
         }
         $output .= 
            "<td class='list'><a href='man.php?query=$prog'>$prog</a></td>";
      }
      while($n < 4) {
         $n++;
         $output .= "<td class='list'>&nbsp;</td>";
      }
      $output .= "</tr>";
   } else {
      $progs = explode(',',$progs);
      $n = 0;
      foreach ($progs as $prog) {
         $prog = preg_replace("/\s/","",$prog);
         if ($prog == "") continue;
         $desc = `$descmd/$prog.1`;
         $n++;
         $output .= 
            "<tr><td class='list'><a href='man.php?query=$prog'>$prog</a>";
         $output .= 
            "&nbsp;</td><td class='list'>$desc&nbsp;</td></tr>";
      }
      if ($n == 0) {
         $output .= "<tr><td>Nothing found with progs: '$progs'</td></tr>";
      }
   }
   $output .= "</table></td></tr></table>";
   echo $output;
}


////////////////////////////////////////////////////////////////////////
// printtopic()  
////////////////////////////////////////////////////////////////////////
function printtopic($topic)
{
   global $manpath, $descmd;

   if (strlen($topic) < 1) {
      echo "<h3>No topic given</h3>";
      return;
   }

   $cmd = "cd $manpath/man1;ls  *.1";
   $progs = `$cmd`;
   $progs = explode('.1',$progs);

   echo
      "<table align='center' border='1' cellspacing='1' ",
      "cellpadding='1' bgcolor='#03629C'><tr><td>",
      "<table class='list' cellpadding='3'>";
   $topic = strtolower($topic);
   $topic = preg_replace("/\s/","",$topic);
   $ors = explode("||",$topic);
   foreach($ors as $key => $val) $ors[$key] = explode("&&",trim($val));

   $n = 0;
   foreach ($progs as $prog) {
      $prog = preg_replace("/\s/","",$prog);
      if ($prog == "") continue;
      $desc = `$descmd/$prog.1`;
      $descl = strtolower($desc) . strtolower($prog);

      $found = true;
      foreach($ors as $or) {
	 $found = true;
	 foreach($or as $token) {
  	    $pos = stripos($descl,$token);
	    if ($pos === false) {
	       $found = false;
               break;
            }
	 }
	 if ($found) break;
      }

      if ($found) {
         $n = $n + 1;
         echo 
            "<tr><td class='list'><a href='man.php?query=$prog'>$prog</a>",
            "&nbsp;</td><td class='list'>$desc&nbsp;</td></tr>";
      }
   }
   if ($n == 0) echo "<tr><td>Nothing found with Query: '$topic'</td></tr>";
   echo "</table></td></tr></table><br/>";

   echo "<table align='center' border='1' cellspacing='1' cellpadding='1' bgcolor='#03629C'><tr><td>";
   echo "<table border='1' cellspacing='0' cellpadding='5' bgcolor='#FFFFFF'><tr align='center' valign='top'><td>";
   echo "<form style='margin: 10px;' method='GET' action='man.php'>";
   echo "<input type='hidden' name='title' value='query'>";
   echo "<input type='text' name='topic' size='32' value=''>";
   echo "<input type='submit' value='Query Topic'><br>";
   echo '(Use "&amp;&amp;" and/or "||" with "||" taking precedence)';
   echo "</form></td></tr></table></td></tr></table><br>";
}


////////////////////////////////////////////////////////////////////////
// print man page
////////////////////////////////////////////////////////////////////////
function printman($query) 
{
   global $web;

   // Just keep the first word of the query, as man(1) only accepts one
   $query = preg_split("/[\s]+/",$query);
   $query = $query[0];

   if ($web != "none") {
      echo "<table align='center' border='1' cellspacing='1' cellpadding='1' bgcolor='#03629C'><tr><td>";
      echo "<table border='1' cellspacing='0' cellpadding='5' bgcolor='#FFFFFF'><tr align='center'><td><p>";
      echo "See <a href=https://seismicimaging.${web}/3dmbs_web/dds_users.html#$query>Houston Users</a> of $query<br>";
      echo "See <a href=https://seismicimaging.${web}/3dmbs_web/dds_sunbury_users.html#$query>Sunbury Users</a> of $query";
      echo "</td><td>";
      echo "See <a href=https://seismicimaging.${web}/3dmbs_web/Projects/ListOfDDSScripts.html#$query>Script Examples</a> using $query<br>";
      echo "See <a href=http://eptgweb.${web}:3001/search?q=$query>Reported Issues</a> about $query<br>";
      echo "</td></tr></table></td></tr></table><br>";
   }

   echo "<table align='center' border='1' cellspacing='1' cellpadding='1' bgcolor='#03629C'><tr><td>";
   echo "<table border='1' cellspacing='0' cellpadding='5' bgcolor='#FFFFFF'><tr align='center' valign='top'><td>";
   echo "<form style='margin: 10px;' method='GET' action='man.php'>";
   echo "<input type='text' name='query' size='20' value=''>";
   echo "<input type='submit' value='ManPage'>";
   echo "</form></td><td>";
   echo "<form style='margin: 10px;' method='GET' action='man.php'>";
   echo "<input type='hidden' name='title' value='Query'>";
   echo "<input type='text' name='query' size='32' value=''>";
   echo "<input type='submit' value='Query Topic'><br>";
   echo '(Use "&amp;&amp;" and/or "||" with "||" taking precedence)';
   echo "</form></td></tr></table></td></tr></table><br>";


   // Get rid of invalid queries
   // Currently just 'dies'. Will probably have it do some fancy stuff later
   $allow_exp="/^[A-Za-z0-9\-\_\:\.]*$/i";
   if (!preg_match($allow_exp,$query)) {
      echo "allow= \"$allow_exp\" (\"".preg_match($allow_exp,$query)."\")<br/>";
      echo "Query with disallowed characters passed: \"$query\"";
      return;
   }

   $cmd = "man $query|col -bx|sed -e's/2[0-9]m//g' -e's/[0-9]m//g' -e's/\xe2\x80\x90/-/g'";
   $output = `$cmd`;

   if (preg_match("/^\s*$/",$output)) {
      echo "<h3>No man page found for \"$query\"</h3>";
      echo "<center><h2>Query: for \"$query\"</h2></center>";
      printtopic($query);
      return;
   }

   //Delete any blank spaces from the beginning
   $output = preg_replace("/[<]/", "&lt;", $output);

   // Highlight the first word of each line if not indented 
   // (Crude check for keywords like 'NAME', 'SYNOPSIS' etc.)
   $output = preg_replace("/(?m)^(\w[ -.\w]*)/","<b>\\1</b>",$output);

   // Highlight any occurences for the keyword sought
   $q = escapeshellcmd($query);
   $output = preg_replace("/(\s+)($q)(\s+)/","\\1<b>\\2</b>\\3",$output);


   //Highlight options
   $output = preg_replace("/([\,\s\[])([-]{1,2}[-\n\.\=\w\d]+)([\,\s\]])/",
                          "\\1<b><i>\\2</i></b>\\3",$output);

   //Trim blank lines to a max of 2 (better for printing)
   $output = preg_replace("/\n\n\n+/","\n\n",$output);

   // Escape ampersands
   $output = preg_replace("/&([^\w;]+)/","&amp;\\1",$output);

   echo "<pre>${output}</pre>";
}


$topic = (isset($_GET[topic])) ? $_GET[topic] : '';
$progs = (isset($_GET[progs])) ? $_GET[progs] : '';
$query = (isset($_GET[query])) ? $_GET[query] : '';
$title = (isset($_GET[title])) ? $_GET[title] : '';

ob_start();
if (strlen($query)) { 
   printhead("Manpage: \"$query\"");
   printman($query);
 } else if (strlen($progs)) { 
   printhead("Progs: \"$title\"");
   printlist($progs);
 } else if ($title == "Query") {
   printhead("Query: \"$topic\"");
   printtopic($topic);
 } else {
   printhead("Topic: \"$title\"");
   printtopic($topic);
 }

?>

<!*********************************************************************>
<script type="text/javascript">footer()</script>
</body>
</html>

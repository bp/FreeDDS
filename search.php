<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
  <head>
    <title>Data Dictionary System: search</title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <script src="javascript.js" language="JavaScript"
      type="text/javascript"></script>
    <link href="search.css" type="text/css" rel="stylesheet">
    <link href="styles.css" type="text/css" rel="stylesheet">
    <style type="text/css">
      thead {font-weight:bold;}
      td {text-align:left;}
      img {border:0px none;}
    </style>
  </head>
  <body>

<?php
/* terraserver.de/search-0.2-11.04.2002 - http://www.terraserver.de/

Copyright (C) 2002 Holger Eichert, mailto:h.eichert@gmx.de. All rights
reserved.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License http://www.gnu.org/copyleft/gpl.html for more
details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

About:
Some people say php is not the language to do a fulltextsearch and they
are right ;-) but anyway: terraserver.de/search performs a realtime 
fulltext-search over specified directories including subdirectorys and
returns a link and an extract of each file. htmlspecialchars are
supported so a search for "über" will return documents having
"&uuml;ber" as well as documents having "über". 

Changes:
- Added some more features like 'match case' and the possibility to
limit the number of results.

*/


// Server
//$my_server = "http://".getenv("SERVER_NAME").":".getenv("SERVER_PORT")."/DDS";
$my_server = "http://".getenv("SERVER_NAME")."/DDS";
// Document root
$my_root = getenv("DOCUMENT_ROOT")."/DDS"; 
// Directories to search (array(""); searches entire server)
//$s_dirs = array("/search/dir1","/search/dir2"); 
$s_dirs = array(""); 
// Files/Dirs to skip
$s_skip = array("..",".","wiki","images","doxygen","wikiDbase",".snapshot");
// File types to be searched? Example: "html$|htm$|php4$"
//$s_files = "html|htm|HTM|HTML|php3|php4|php|txt"; 
$s_files = "html"; 
// Min. chars that must be entered to perform the search
$min_chars = "2"; 
// Max. chars that can be submited to perform the search
$max_chars = "80"; 
// Default value in searchfield
$default_val = "Search Phrase"; 
// # hits to be displayed (single value to suppress select-menu)
$limit_hits = array("5","10","25","50","100","1000"); 
// Invalid search term
$message_1 = "Invalid Search Term!"; 
// Invalid search term length ($min_chars/$max_chars)
$message_2 = "Please enter at least '$min_chars', highest '$max_chars' characters."; 
// Headline search results
$message_3= "Your search result for:"; 
// No hits
$message_4 = "Sorry, no hits!"; 
// Hits
$message_5 = "results"; 
// Match case
$message_6 = "Match case"; 
// This should be displayed if no title or empty title is found in file
$no_title = "Untitled"; 
// # extracts per file to display (e.g. "3"; default: "" -> all)
$limit_extracts_extracts = "20"; 
// # bytes per file should be searched? Reduce to increase speed
$byte_size = "51200"; 
// Sections to search
$sections = array("home","users","developers","applications","codeslinger",
                  "maintainers","download","all");

//ini_set("error_reporting", "2047"); // Debugger

$action=$_GET['action'];
$keyword=$_GET['keyword'];
$limit=$_GET['limit'];
$case=$_GET['case'];
$home=$_GET['home'];
$users=$_GET['users'];
$developers=$_GET['developers'];
$applications=$_GET['applications'];
$codeslinger=$_GET['codeslinger'];
$maintainers=$_GET['maintainers'];
$download=$_GET['download'];
$all=$_GET['all'];

////////////////////////////////////////////////////////////////////////
// search_form()
////////////////////////////////////////////////////////////////////////
function search_form() 
{
   global $s_skip, $sections, $keyword, $limit, $limit_hits, 
      $default_val, $message_5, $message_6;
   global $home, $users, $developers, $applications, $codeslinger, $maintainers,
      $download, $all;

   echo
      "<h2>DDS SearchForm</h2>",
      "<form action='$PHP_SELF' method='GET'>\n", 
      "<input type='hidden' value='SEARCH' name='action'>\n",
      "<input type='text' name='keyword' class='text' ",
      "size='10'  maxlength='30' ";
   if($keyword) {
      $keywords = str_replace("&amp;","&",htmlentities($keyword));
      echo "value='$keywords' "
         . "onFocus=\" if (value == '$keywords') {value=''}\" "
         . "onBlur=\"if (value == '') {value='$keywords'}\"> ";
   } else {
      echo "value='$default_val' onFocus=\" "
         . "if (value == '$default_val') {value=''}\" "
         . "onBlur=\"if (value == '') {value='$default_val'}\"> ";
   }

   $j = count($limit_hits);
   if ($j==1) {
      echo 
         "<input type='hidden' value='${limit_hits[0]}' name='limit'>";
   } else if ($j>1) {
      echo "<select name='limit' class='select'>\n";
      for($i=0;$i<$j;$i++) {
         echo "<option value='${limit_hits[$i]}'";
         if ($limit==$limit_hits[$i]) echo "SELECTED";
         echo ">${limit_hits[$i]} $message_5</option>";
      }
      echo "</select> ";
   }
   echo
      "<br>\n",
      "<span class='checkbox'>$message_6</span> ",
      "<input type='checkbox' name='case' value='true' ",
      "class='checkbox'";
   if ($case) echo " CHECKED";
   echo ">";

   // selected sections to search
   echo "<br /><table>";

   $ss_skip = array("..",".","wiki");
   $n = 0;
   foreach ($sections as $sect) {
      $n++;
      if ($n == 1) echo "<tr>";
      if ($sect == "all") {
         $n++;
         echo "<td>&nbsp;</td>";
      }
      echo 
         "<td class='checkbox'><span class='checkbox'>$sect</span> ",
         "<input type='checkbox' name='$sect' ",
         "class='checkbox' value='true'";
      if ($$sect) {
         echo " CHECKED";
      } else if (! $all) {
         @$s_skip[] = "$sect";
         if ($sect == "home") {
            @$s_skip[] = "index.html";
         } else {
            @$s_skip[] = "$sect.html";
         }
      }
      echo "></td>";
      if ($n >= 3) {
         $n = 0;
         echo "</tr>";
      }
   }
   if ($n) {
      for(;$n<3;$n++) echo "<td>&nbsp;</td>";
      echo "</tr>";
   }
   echo "</table>";

   // go back
   echo "<hr /><input type='submit' value='SEARCH' class='button'>",
      "&nbsp;<input type='button' value='RETURN' class='button' ",
      "onclick='history.back();'>";

   echo "</form>";
}


////////////////////////////////////////////////////////////////////////
// search_headline()
////////////////////////////////////////////////////////////////////////
function search_headline()
{
   global $keyword, $action, $message_3;
   if ($action == "SEARCH") {
      echo "<h1 class='result'>$message_3 '<i>",
         htmlentities(stripslashes($keyword))."</i>'</h1>";
   }
}


////////////////////////////////////////////////////////////////////////
// search_error()
////////////////////////////////////////////////////////////////////////
function search_error()
{
   global $keyword, $action, $limit, $min_chars, $max_chars, $message_1, 
      $message_2, $limit_hits;
   if ($action == "SEARCH") {
      if (strlen($keyword)<$min_chars || strlen($keyword)>$max_chars ||
          !in_array ($limit, $limit_hits)) {
         echo "<p class='result'><b>$message_1</b><br>$message_2</p>";
         $_GET['action'] = "ERROR";
         $action = "ERROR";
         }
      }
   }


////////////////////////////////////////////////////////////////////////
// search_dir()
////////////////////////////////////////////////////////////////////////
function search_dir($s_dirs) 
{
   global $count_hits, $action, $limit, $case;
   global $my_server, $my_root, $s_files, $s_skip;
   global $message_1, $message_2, $no_title, $limit_extracts, $byte_size;

   @$keyword=$_GET['keyword'];
   $str="<i><b>..</b> %s<b>%s</b>%s <b>..</b></i>";
   if ($action == "SEARCH") { 
      foreach ($s_dirs as $dir) {
         if (in_array($dir, $s_skip)) continue;
         $handle = @opendir($my_root.$dir);
         while ($file = @readdir($handle)) {
            if (in_array($file, $s_skip)) continue;
            if ($count_hits>=$limit) break;
            if (is_dir($my_root.$dir."/".$file)) {
               search_dir(array("$dir/$file"));
            } else if (preg_match("/($s_files)$/i", $file)) {
               $fd=fopen($my_root.$dir."/".$file,"r");
               $text=fread($fd, $byte_size);
               $keyword_html = htmlentities($keyword);
               if ($case) {
                  $do=strstr($text, $keyword) || 
                     strstr($text, $keyword_html);
               } else {
                  $do=stristr($text, $keyword) ||
                     stristr($text, $keyword_html);
               }
               if ($do) {
                  $count_hits++;
                  if (preg_match_all("=<title[^>]*>(.*)</title>=siU",
                                     $text, $title)) { 
                     if(!$title[1][0]) {
                        $link_title=$no_title;
                     } else {
                        $link_title=$title[1][0]; 
                     }
                  } else {
                     $link_title=$no_title;
                  }
                  echo 
                     "<a href='$my_server$dir/$file' ",
                     "target='_self' class='result'>$count_hits.  ",
                     "$link_title</a><br />";
                  $auszug = strip_tags($text);
                  $keyword = preg_quote($keyword);
                  $keyword = str_replace("/","\/","$keyword");
                  $keyword_html = preg_quote($keyword_html); 
                  $keyword_html = str_replace("/","\/","$keyword_html");

                  echo "<span class='extract'>&nbsp;";
                  $mtch="/((\s\S*){0,3})($keyword|$keyword_html)((\s?\S*){0,3})/i";
                  if (preg_match_all($mtch, $auszug, $match,
                                     PREG_SET_ORDER)); {
                     if(!$limit_extracts) {
                        $number=count($match);
                     } else {
                        $number=$limit_extracts;
                     }
                     for ($h=0;$h<$number;$h++) {
                        if (!empty($match[$h][3])) {
                           printf($str, $match[$h][1], $match[$h][3], 
                                  $match[$h][4]);
                        }
                     }
                  }
                  echo "</span><br /><br />";
                  flush();
               }
               fclose($fd);
            }
         }
         @closedir($handle);
      }
   }
}


////////////////////////////////////////////////////////////////////////
// search_no_hits()
////////////////////////////////////////////////////////////////////////
function search_no_hits() 
{
   global $action, $count_hits, $message_4;
   if ($action == "SEARCH" && $count_hits<1) {
      echo "<p class='result'>$message_4</p>";
   }
}

    $section = 'users';
    foreach ($sections as $sect) if ($$sect) $section = $sect;
    echo "<script type=\"text/javascript\">header(\"$section\")</script>";
?>
    <center>
      <table border="1" cellspacing="1" cellpadding="1" bgcolor="#03629C">
          <tr>
            <td>
              <table border="1" cellspacing="0"
                cellpadding="5" bgcolor="#FFFFFF">
                  <tr>
                    <td style="text-align:center;">

                      <?php search_form(); ?>

                    </td>
                  </tr>
              </table>   
            </td>
          </tr>
      </table>
    </center>

<?php
    search_headline();
    search_error();
    search_dir($s_dirs);
    search_no_hits();
?>
    <!*********************************************************************>
    <script type="text/javascript">footer()</script>
  </body>
</html>

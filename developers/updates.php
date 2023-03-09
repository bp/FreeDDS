<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <title>Data Dictionary System: Developers updates</title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <link href="../styles.css" type="text/css" rel="stylesheet">
    <script src="../javascript.js" language="JavaScript" 
      type="text/javascript"></script>
    <style type="text/css">
      .list {border:1px inset;background-color:#edfeff;}
    </style>
  </head>
    <body>
      <script type="text/javascript">
        header("developers","updates")
      </script>

      <center>
        <b>DDS Updates from Wiki</b><br /><br />
      </center>

      <?php

          // Fetch any supplemental info from the wiki sorted by date
          $cmd = "cat `pwd`/../wikiDbase/pages/DDS_Updates";
             $content = `$cmd`;
             $content = unserialize($content);
             $content = $content["content"];

             $progs = "";
             foreach ($content as $prog) {
                if (strncmp($prog,"* [",3)==0) {
                   $prog = substr($prog,3);
                   $n = strpos($prog,"]");
                   $prog = substr($prog,0,$n);
                   $n = strpos($prog,"|");
                   if ($n !== false) $prog = substr($prog,$n+1);
                   $progs .= "${prog} ";
                }
             }
             $cmd = "cd `pwd`/../wikiDbase/pages;ls -lt ${progs}";
             $list = `$cmd`;
             $list = explode("\n",$list);

             $output = "<table align='center' border='1' cellspacing='1' ";
             $output .= "cellpadding='1' bgcolor='#03629C'><tr><td>\n";
             $output .= "<table class='topic' border='1' cellspacing='0' cellpadding='5'>\n";
             $output .= "<thead><tr><th class='list'>Program</th>";
             $output .= "<th class='list'>Updated</th></tr></thead>\n<tbody>";
             foreach ($list as $line) {
                $line = preg_replace("/  /"," ",$line);
                $line = explode(" ",$line);
                $n = count($line);
                if ($n > 8) {
                   $date = "${line[$n-4]} ${line[$n-3]} ${line[$n-2]}";
                   $prog = $line[$n-1];
                   $item = "<tr><td class='list'>";
                   $item .= "<a href=\"../wiki/index.php?$prog\">";
                   $item .= "$prog</a></td><td class='list'>";
                   $item .= "$date</td></tr>\n";
                   $output .= "$item";
                }
             }   

             $output .= "</tbody></table></td></tr></table>";
             echo $output;
      ?>

      <!*********************************************************************>
      <script type="text/javascript">footer()</script>
    </body>
</html>

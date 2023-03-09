//
// These routines maintain a common header and footer javascript functions
// for all pages along with colors, menus, etc.
//

var menu;
var root;
var prev=up=next="";
var active0, iactive, active1, active2, active3;
var title1, title2, title3;
var directions;
var print = (location.search.search("print=") >= 0);
if (print) {
   document.write("<style type='text/css'>"
      +"h1 {font-size:16pt;}"
      +"h2 {font-size:14pt;}"
      +"h3 {font-size:12pt;}"
      +"h4 {font-size:10pt;}"
      +"p {font-size:10pt;}"
      +"a {font-size:10pt;}"
      +"li {font-size:10pt;}"
      +"div {font-size:10pt;}"
      +"pre {font-size:8pt;}"
      +"code {font-size:8pt;}"
      +".menu {font-size:12pt;}"
      +".active {font-size:10pt;}"
      +".footnnote {font-size:8pt;}"
      +"#footer {font-size:7pt;}"
      +"<\/style>");
 }

// Main tab themes
var title = new Array();
title["home"] = "DDS Home Page";
title["users"] = "DDS User Tutorials";
title["applications"] = "DDS Applications";
title["developers"] = "DDS Developers Documentation";
title["codeslinger"] = "DDS Code Slinger Training";
title["maintainers"] = "DDS Maintainers Documentation";
title["download"] = "FreeDDS Download";
//title["documents"] = "DDS Documents";
//title["ddswiki"] = "DDS Wiki";

var color = new Array();
color["home"] = "#0007f4";
color["users"] = "#037c00";
color["applications"] = "#005976";
color["developers"] = "#956400";
color["codeslinger"] = "#b96800";
color["maintainers"] = "#980004";
color["download"] = "#810fc3";
//color["documents"] = "#4a402e";
//color["ddswiki"] = "#6c6c6c";

var edgeColor = new Array();
edgeColor["home"] = "#cbd0eb";
edgeColor["users"] = "#cbe9c9";
edgeColor["applications"] = "#cbe7eb";
edgeColor["developers"] = "#f8e3a6";
edgeColor["maintainers"] = "#e2b6bd";
edgeColor["codeslinger"] = "#ffcc99";
edgeColor["download"] = "#dfcfed";
//edgeColor["documents"] = "#d0b483";
//edgeColor["ddswiki"] = "#c4c4c4";

var mainLbl;
var ndxLbl = [0,
              [0,"I.&nbsp;","II.&nbsp;","III.&nbsp;","IV.&nbsp;",
               "V.&nbsp;","VI.&nbsp;","VII.&nbsp;","VIII.&nbsp;",
               "IX.&nbsp;","X.&nbsp;","XI.&nbsp;","XII.&nbsp;",
               "XIII.&nbsp;","XIV.&nbsp;","XV.&nbsp;","XVI.&nbsp;"],
              [0,"A.&nbsp;","B.&nbsp;","C.&nbsp;","D.&nbsp;","E.&nbsp;",
               "F.&nbsp;","G.&nbsp;","H.&nbsp;","I.&nbsp;","J.&nbsp;",
               "K.&nbsp;","L.&nbsp;","M.&nbsp;","N.&nbsp;","O.&nbsp;",
               "P.&nbsp;","Q.&nbsp;","R.&nbsp;","S.&nbsp;","T.&nbsp;",
               "U.&nbsp;","V.&nbsp;","W.&nbsp;","X.&nbsp;","Y.&nbsp;",
               "Z.&nbsp;","AA.&nbsp;","BB.&nbsp;","CC.&nbsp;","DD.&nbsp;",
               "EE.&nbsp;","FF.&nbsp;"],
              [0,"1.&nbsp;","2.&nbsp;","3.&nbsp;","4.&nbsp;","5.&nbsp;",
               "6.&nbsp;", "7.&nbsp;", "8.&nbsp;","9.&nbsp;","10.&nbsp;",
               "11.&nbsp;","12.&nbsp;","13.&nbsp;","14.&nbsp;","15.&nbsp;",
               "16.&nbsp;","17.&nbsp;","18.&nbsp;","19.&nbsp;","20.&nbsp;",
               "21.&nbsp;","22.&nbsp;","23.&nbsp;","24.&nbsp;","25.&nbsp;",
               "26.&nbsp;","27.&nbsp;","28.&nbsp;","29.&nbsp;","30.&nbsp;"]
              ];



////////////////////////////////////////////////////////////////////////////
//
// main tab management
//
////////////////////////////////////////////////////////////////////////////
function newImage(arg) {
   var rslt = new Image();
   rslt.src = arg;
   return rslt;
}

function setImages() {
   home_out  = newImage(root+'images/home.png');
   home_active = newImage(root+'images/homeActive.png');
   users_out  = newImage(root+'images/users.png');
   users_active  = newImage(root+'images/usersActive.png');
   applications_out  = newImage(root+'images/applications.png');
   applications_active  = newImage(root+'images/applicationsActive.png');
   developers_out  = newImage(root+'images/developers.png');
   developers_active  = newImage(root+'images/developersActive.png');
   maintainers_out  = newImage(root+'images/maintainers.png');
   maintainers_active  = newImage(root+'images/maintainersActive.png');
   codeslinger_out  = newImage(root+'images/codeslinger.png');
   codeslinger_active  = newImage(root+'images/codeslingerActive.png');
   download_out  = newImage(root+'images/download.png');
   download_active  = newImage(root+'images/downloadActive.png');
   documents_out  = newImage(root+'images/documents.png');
   documents_active  = newImage(root+'images/documentsActive.png');
   ddswiki_out  = newImage(root+'images/ddswiki.png');
   ddswiki_active  = newImage(root+'images/ddswikiActive.png');
}

function bOver(image) {
   document[image].src = eval(image +"_active.src");
}

function bOut(image) {
   if (image == active0) {
      document[image].src = eval(image +"_active.src");
   } else {
      document[image].src = eval(image +"_out.src");
   }
}

function setTab(level) {
   var page = level;
   var tab = "";
   if (level=="home") {
      page = "index.html";
   } else if (level=="download") {
      page = "download.html";
   } else {
      page = level+".html";
   }
   if (level==active0) tab = "Active";
   document.write("<a href='"+root+page+"'><img name='"
      +level+"' src='"+root+"images/"+level+tab
      +".png' onmouseover='bOver(\""+level
      +"\")' onmouseout='bOut(\""+level+"\")'></a>");
}

////////////////////////////////////////////////////////////////////////////
//
// menu management 
//
////////////////////////////////////////////////////////////////////////////

function menuItem(lvl0, lvl1, title1, lvl2, title2, lvl3, title3) {
   var n, m;
   
   if (lvl1 == 'none' || lvl2 == 'none' || lvl3 == 'none') return;
   if (title1 == undefined) return;

   if (menu == undefined) menu = new Array();
   if (menu[lvl0] == undefined) menu[lvl0] = new Array();

   title1 = title1.replace(/\s/g,"&nbsp;");
   if (title2 != undefined) title2 = title2.replace(/\s/g,"&nbsp;");
   if (title3 != undefined) title3 = title3.replace(/\s/g,"&nbsp;");
  
   n = menu[lvl0].length;

   menu[lvl0][n] = { 
      lvl1: lvl1,
      title1: title1,                                    
      lvl2: lvl2,
      title2: title2,
      lvl3: lvl3,
      title3: title3
   };
}

// Home
menuItem("home","overview","Overview Diagram"); 
menuItem("home","about","About Developers"); 
menuItem("home","news","News"); 
menuItem("home","bugs","Known Bugs"); 
 
//Users
menuItem("users","dict","Dictionaries"); 
menuItem("users","dict","Dictionaries","internal","Internal Dictionaries"); 
menuItem("users","dict","Dictionaries","data","Data Dictionaries"); 
menuItem("users","dict","Dictionaries","special","Special Dictionaries"); 
menuItem("users","dict","Dictionaries","search","Search Dictionaries");
 
menuItem("users","defn","Definitions"); 
menuItem("users","defn","Definitions","tokens","Definition Tokens"); 
menuItem("users","defn","Definitions","alias","Alias Definitions"); 
menuItem("users","defn","Definitions","axis","Axis Definitions"); 
menuItem("users","defn","Definitions","cmd","Cmd Line Definitions"); 
menuItem("users","defn","Definitions","par","Par File Definitions"); 

menuItem("users","datasets","Datasets"); 
menuItem("users","datasets","Datasets","binary","Binary Data"); 
menuItem("users","datasets","Datasets","binary","Binary Data",
         "data","data"); 
menuItem("users","datasets","Datasets","binary","Binary Data",
         "format","format"); 
menuItem("users","datasets","Datasets","binary","Binary Data",
         "fmt","fmt:"); 
menuItem("users","datasets","Datasets","piped","Piped Dataset"); 
menuItem("users","datasets","Datasets","sliced","Sliced Datasets");
 
menuItem("users","formats","Formats"); 
menuItem("users","formats","Formats","linehdr","Line Header"); 
menuItem("users","formats","Formats","trchdr","Trace Header"); 
menuItem("users","formats","Formats","attributes","Attributes"); 
menuItem("users","formats","Formats","usp","usp"); 
menuItem("users","formats","Formats","usp","usp",
         "linehdr","Line Header"); 
menuItem("users","formats","Formats","usp","usp",
         "trchdr","Trace Header"); 
menuItem("users","formats","Formats","usp","usp",
         "map","Mapping"); 
menuItem("users","formats","Formats","usp","usp",
         "uspfmt","usp.fmt.dds"); 
menuItem("users","formats","Formats","segy","segy"); 
menuItem("users","formats","Formats","segy","segy",
         "cardhdr","Card Header"); 
menuItem("users","formats","Formats","segy","segy",
         "binhdr","Binary Header"); 
menuItem("users","formats","Formats","segy","segy",
         "trchdr","Trace Header"); 
menuItem("users","formats","Formats","segy","segy",
         "map","Mapping"); 
menuItem("users","formats","Formats","segy","segy",
         "segyfmt","segy.fmt.dds"); 
menuItem("users","formats","Formats","segy1","segy1"); 
menuItem("users","formats","Formats","segy1","segy1",
         "trchdr","Trace Header"); 
menuItem("users","formats","Formats","segy1","segy1",
         "map","Mapping"); 
menuItem("users","formats","Formats","segy1","segy1",
         "segy1fmt","segy1.fmt.dds"); 
menuItem("users","formats","Formats","su","su"); 
menuItem("users","formats","Formats","su","su",
         "map","Mapping"); 
menuItem("users","formats","Formats","sep","sep"); 
menuItem("users","formats","Formats","asp","asp"); 
menuItem("users","formats","Formats","asp","asp",
         "map","Mapping"); 
menuItem("users","formats","Formats","asp","asp",
         "aspfmt","asp.fmt.dds"); 
menuItem("users","formats","Formats","asp","asp",
         "zip","zip"); 

menuItem("users","iocontrol","I/O Data"); 
menuItem("users","iocontrol","I/O Data","in","Input Dataset"); 
menuItem("users","iocontrol","I/O Data","out","Output Dataset"); 
menuItem("users","iocontrol","I/O Data","auxiliary","Auxiliary Dataset"); 
menuItem("users","iocontrol","I/O Data","par","Par File"); 
menuItem("users","iocontrol","I/O Data","printout","Printout File"); 

menuItem("users","datatypes","Data Types"); 
menuItem("users","datatypes","Data Types","dependent","Host Dependent"); 
menuItem("users","datatypes","Data Types","independent","Host Independent");
menuItem("users","datatypes","Data Types","uspt","usp Types"); 
menuItem("users","datatypes","Data Types","segyt","segy Types"); 

menuItem("users","devices","Devices"); 
menuItem("users","devices","Devices","attributes","Attributes"); 
menuItem("users","devices","Devices","predefined","Predefined Tapes"); 

menuItem("users","fmt","Fmt Control"); 
menuItem("users","fmt","Fmt Control","fmt","fmt:"); 
menuItem("users","fmt","Fmt Control","fmt","fmt:",
         "fmt","Conversion"); 
menuItem("users","fmt","Fmt Control","sort","sort:"); 
menuItem("users","fmt","Fmt Control","edit","edit:"); 
menuItem("users","fmt","Fmt Control","modfield","MOD_FIELD"); 
menuItem("users","fmt","Fmt Control","functions","Functions"); 
menuItem("users","fmt","Fmt Control","dictionaries","Dictionaries"); 

menuItem("users","mapping","Mapping");
menuItem("users","mapping","Mapping","map","map:");
menuItem("users","mapping","Mapping","read","read:");
menuItem("users","mapping","Mapping","write","write:");
menuItem("users","mapping","Mapping","rename","rename:");
menuItem("users","mapping","Mapping","functions","Functions");
menuItem("users","mapping","Mapping","dictionaries","Dictionaries");
menuItem("users","mapping","Mapping","defaults","Defaults");
menuItem("users","mapping","Mapping","bugs","Bugs");
 
menuItem("users","runtime","Runtime Control"); 
menuItem("users","debug","Debug Support"); 

menuItem("users","faq","FAQ");
menuItem("users","faq","FAQ","padding","Padding Traces");
menuItem("users","faq","FAQ","ordinal","Ordinal Usage");
menuItem("users","faq","FAQ","segy_card3200","Segy EBCDIC Header");
menuItem("users","faq","FAQ","working_with_usp","Working With USP");

// Applications
menuItem("applications","xsd","xsd");
menuItem("applications","xsd","xsd","server","Start Server");
menuItem("applications","xsd","xsd","data","Load Data");
menuItem("applications","xsd","xsd","data","Load Data",
         "definition","Data Definition");
menuItem("applications","xsd","xsd","data","Load Data",
         "selection","Data Selection");
menuItem("applications","xsd","xsd","data","Load Data",
         "parameters","Plot Parameters");
menuItem("applications","xsd","xsd","load","Load Images");
menuItem("applications","xsd","xsd","windows","Display Windows");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "menu","Window Menu");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "plot","Plot/Save Images");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "global","Global Window");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "wiggle","Wiggle Displays");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "animation","Animation");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "overlay","Overlay");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "interleave","Interleave");
menuItem("applications","xsd","xsd","windows","Display Windows",
         "rgbi","RGBI");
menuItem("applications","xsd","xsd","picking","Picking");
menuItem("applications","xsd","xsd","picking","Picking",
         "picks","Picks");
menuItem("applications","xsd","xsd","picking","Picking",
         "segments","Segments");
menuItem("applications","xsd","xsd","colors","Colors");
menuItem("applications","xsd","xsd","labels","Image Labels");
menuItem("applications","xsd","xsd","options","Options");

menuItem("applications","ddscan","ddscan");
menuItem("applications","ddscan","ddscan","G",'"G"');
menuItem("applications","ddscan","ddscan","CP",'"CP"');
menuItem("applications","ddscan","ddscan","CS",'"CS"');
menuItem("applications","ddscan","ddscan","H",'"H|HB|HC"');
menuItem("applications","ddscan","ddscan","L",'"L"');
menuItem("applications","ddscan","ddscan","T",'"T"');
menuItem("applications","ddscan","ddscan","TA",'"TA"');
menuItem("applications","ddscan","ddscan","R",'"R"');
menuItem("applications","ddscan","ddscan","F",'"F"');
menuItem("applications","ddscan","ddscan","DP",'"DP"');
menuItem("applications","ddscan","ddscan","DH",'"DH"');
menuItem("applications","ddscan","ddscan","DS",'"DS"');
menuItem("applications","ddscan","ddscan","AH",'"AH"');
menuItem("applications","ddscan","ddscan","AS",'"AS"');
menuItem("applications","ddscan","ddscan","ION",'"ION|IOFF"');
menuItem("applications","ddscan","ddscan","SH",'"SH"');
menuItem("applications","ddscan","ddscan","SS",'"SS"');
menuItem("applications","ddscan","ddscan","ST",'"ST"');
menuItem("applications","ddscan","ddscan","SR",'"SR"');

//menuItem("applications","ddsbrief","DDS Brief List");
//menuItem("applications","ddslong","DDS Long List");
//menuItem("applications","functions","Functionality List");
// menuItem("applications","source","Program Sources");
//menuItem("applications","ddssvnlog","Program Logs");
//menuItem("applications","scripts","Script Sources");

// Developers
menuItem("developers","make","Code Development"); 
menuItem("developers","make","Code Development","make","make");
menuItem("developers","make","Code Development","make","make",
         "help","make help"); 
menuItem("developers","make","Code Development","make","make",
         "variables","make variables"); 
menuItem("developers","make","Code Development","make","make",
         "flags","make flags"); 
menuItem("developers","make","Code Development","multiple","Multiple Codes");
menuItem("developers","make","Code Development","libraries","Application Libraries");
menuItem("developers","make","Code Development","distribute","Distribution");
menuItem("developers","make","Code Development","tests","Application Tests");
menuItem("developers","ftutorial","Fortran Tutorial"); 
menuItem("developers","ftutorial","Fortran Tutorial",
         "initialize","Initialization");
menuItem("developers","ftutorial","Fortran Tutorial",
         "log","Message Logging");
menuItem("developers","ftutorial","Fortran Tutorial",
         "input","Open Input");
menuItem("developers","ftutorial","Fortran Tutorial",
         "input","Open Input","convenience","Convenience Routines");
menuItem("developers","ftutorial","Fortran Tutorial",
         "scan","Scan Parameters");
menuItem("developers","ftutorial","Fortran Tutorial",
         "print","Print Parameters");
menuItem("developers","ftutorial","Fortran Tutorial",
         "printfile","Print messages");
menuItem("developers","ftutorial","Fortran Tutorial",
         "allocate","Allocate Memory");
menuItem("developers","ftutorial","Fortran Tutorial",
         "output","Open Output");
menuItem("developers","ftutorial","Fortran Tutorial",
         "output","Open Output","convenience","Convenience Routines");
menuItem("developers","ftutorial","Fortran Tutorial",
         "process","Process Data");
menuItem("developers","ftutorial","Fortran Tutorial",
         "process","Process Data","convenience","Convenience Routines");
menuItem("developers","ftutorial","Fortran Tutorial",
         "process","Process Data","access","Data Access");
menuItem("developers","ftutorial","Fortran Tutorial",
         "close","Closeout");
menuItem("developers","fapi","Fortran API");
menuItem("developers","fxapi","Fortran Convenience"); 
menuItem("developers","fft","Fortran FFT Routines"); 
menuItem("developers","flibs","Fortran Application Libs"); 
menuItem("developers","ftemplates","Fortran Templates"); 
menuItem("developers","ftemplates","Fortran Templates",
         "f_template1","Base DDS Routines"); 
menuItem("developers","ftemplates","Fortran Templates",
         "f_template2","Simple Convenience"); 
menuItem("developers","ftemplates","Fortran Templates",
         "f_template3","Complex Convenience"); 
menuItem("developers","ftemplates","Fortran Templates",
         "f_template4","MPI Convenience"); 
menuItem("developers","ftemplates","Fortran Templates",
         "f_template5","MPI Convenience2"); 

menuItem("developers","ctutorial","\"C\" Tutorial"); 
menuItem("developers","ctutorial","\"C\" Tutorial",
         "initialize","Initialization");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "log","Message Logging");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "input","Open Input");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "parm","Scan Parameters");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "print","Print Parameters");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "allocate","Allocate Memory");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "output","Open Output");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "process","Process Data");
menuItem("developers","ctutorial","\"C\" Tutorial",
         "close","Closeout");
menuItem("developers","capi","\"C\" API"); 
menuItem("developers","cxapi","\"C\" Convenience"); 
menuItem("developers","cfft","\"C\" FFT Routines");
menuItem("developers","clibs","\"C\" Application Libs"); 
menuItem("developers","ctemplates","\"C\" Templates");
menuItem("developers","ctemplates","\"C\" Templates",
         "c_template1","Base DDS Routines"); 
menuItem("developers","ctemplates","\"C\" Templates",
         "c_template2","Simple Convenience"); 
menuItem("developers","ctemplates","\"C\" Templates",
         "c_template3","Complex Convenience"); 
menuItem("developers","updates","DDS Updates");

// Code Slinger
menuItem("codeslinger","dictionaries","Dictionary Basics"); 
menuItem("codeslinger","dictionaries","Dictionary Basics",
         "text","Text Dictionaries"); 
menuItem("codeslinger","dictionaries","Dictionary Basics",
         "binary","Binary Dictionaries"); 
menuItem("codeslinger","dictionaries","Dictionary Basics",
         "cmdln","Command Line Dictionaries"); 
menuItem("codeslinger","dictionaries","Dictionary Basics",
         "parfile","Parfile Dictionaries"); 
menuItem("codeslinger","guidelines","Coding Guidelines"); 
menuItem("codeslinger","prgm_flow","DDS Program Flow"); 
menuItem("codeslinger","fapishortutl","API Short List - Fortran"); 
menuItem("codeslinger","capishortutl","API Short List - C"); 
menuItem("codeslinger","pitfalls","Common Pitfalls");
menuItem("codeslinger","basiccodes","Basic Example Codes"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_A","Access to Samples"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_B","Access to User Params."); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_C","Communicate with Users"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_D","Data Sizes Change"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_E","Working Around Headers"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_F","Changing Sample Types"); 
menuItem("codeslinger","basiccodes","Basic Example Codes",
         "bc_G","Working with Headers"); 
//menuItem("codeslinger","basiccodes","Basic Example Codes",
//         "bc_L","Dynamic Headers"); 
menuItem("codeslinger","othercodes","Other Example Codes"); 
menuItem("codeslinger","debugging","Debugging (with) DDS"); 
menuItem("codeslinger","resources","Obscure Resources"); 
menuItem("codeslinger","glossary","Glossary of DDS Terms"); 

// Maintainers
menuItem("maintainers","environ","Environment");
menuItem("maintainers","dir","Directory Tree"); 
menuItem("maintainers","dir","Directory Tree","host","src/lib/host"); 
menuItem("maintainers","dir","Directory Tree","host","src/lib/host",  
         "libsrc","Library Routines");
menuItem("maintainers","dir","Directory Tree","host","src/lib/host",  
         "includes","Include Files");
menuItem("maintainers","dir","Directory Tree","host","src/lib/host",  
         "miscsrc","Misc Programs");
menuItem("maintainers","dir","Directory Tree","host","src/lib/host",  
         "tstsrc","Test Programs");
menuItem("maintainers","dir","Directory Tree","gio","src/lib/gio"); 
menuItem("maintainers","dir","Directory Tree","gio","src/lib/gio",
         "discussion","GIO Discussion"); 
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3");
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3",
         "csource","Source Files"); 
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3",
         "dsource","Generated Files"); 
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3",
         "includes","Include Files"); 
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3",
         "bison","bison"); 
menuItem("maintainers","dir","Directory Tree","dds3","src/lib/dds3",
         "flex","flex"); 
menuItem("maintainers","dir","Directory Tree","fft","src/lib/fft"); 
menuItem("maintainers","dir","Directory Tree","fftw","src/lib/fftw"); 
menuItem("maintainers","dir","Directory Tree","cmd","src/cmd"); 
menuItem("maintainers","dir","Directory Tree","scripts","src/scripts"); 
menuItem("maintainers","dir","Directory Tree","doc","doc/html"); 
menuItem("maintainers","lib","Library Installation"); 
menuItem("maintainers","lib","Library Installation",
         "config","Makefile Configuration"); 
menuItem("maintainers","lib","Library Installation",
         "scheme","Makefile Scheme"); 
menuItem("maintainers","lib","Library Installation",
         "examples","Makefile Examples"); 
menuItem("maintainers","apps","Applications Installation"); 
menuItem("maintainers","doc","Documentation Installation"); 
menuItem("maintainers","details","dds3 Details"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories");
menuItem("maintainers","details","dds3 Details","categories","Source Categories","init","System Initialization"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","dict","Dictionary Manipulation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","scan","Definition Scanning"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","print","Definition/Msg Printing"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","open","Open Data Files"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","close","Close Data Files"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","seek","Random Access Control"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","read","Read Binary Data"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","write","Write Binary Data"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","get","Get Trace Header Values"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","put","Put Trace Header Values"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","media","Media manipulation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","mt","Magnetic Tape Operations"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","mem","Memory Allocation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","str","String Manipulation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","msg","Messages/Error Tracking"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","sym","Flex/Bison Compilation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","map","Header Mapping"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","field","Field Format Access"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","stack","Stack Manipulation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","table","Hash Table Manipulation"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","util","Misc Utility Routines"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","dump","Dump Information"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","zip","ZIP Data Compression"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","convenience","Convenience Routines"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","mp","Parallelization Routines"); 
menuItem("maintainers","details","dds3 Details","categories","Source Categories","svn","Subversion Routine"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","api","dds_api.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","binary","dds_binary.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","dict","dds_dict.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","dump","dds_dump.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","media","dds_media.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","message","dds_message.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","ptype","dds_ptype.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","stack","dds_stack.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","state","dds_state.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","string","dds_string.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","symbol","dds_symbol.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","table","dds_table.h"); 
menuItem("maintainers","details","dds3 Details","structures","Data Structures","util","dds_util.h"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions","compiler","Compiler"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions","symbols","Symbol Tables"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions","symflags","Symbol Flags"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions","hash","Hash Tables"); 
menuItem("maintainers","details","dds3 Details","discussions","Topical Discussions","con","Conversion Macros"); 

// Download
//menuItem("download","lib","Quick Lib Installation"); 
//menuItem("download","apps","Quick Apps Installation"); 
//menuItem("download","doc","Quick Doc Installation");

menuItem("documents"); 

// DDS Wiki 
menuItem("ddswiki","wiki","DDS Wiki"); 


function setRef(i,dir) {
   var href;
   var img1="";
   var img2="";
   var m = menu[active0];

   if (i < 0 || i >= m.length) return "";

   if (dir == "prev") {
      img1 = "<img border='0' align='middle' src='"
         +root+"images/leftArrow.png'>&nbsp;";
   } else if (dir == "next") {
      img2 = "&nbsp;<img border='0' align='middle' src='"
         +root+"images/rightArrow.png'>";
   } else {
      img1 = "<img border='0' align='bottom' src='"
         +root+"images/upArrow.png' alt='up'><br/>";
   }

   if (m[i].lvl3 != undefined) {
      if (dir != "up") {
         href = "<a class='direct' href='"+root+active0+"/"+m[i].lvl1+"/"
            +m[i].lvl2+"/"+m[i].lvl3+".html'>"+img1+m[i].title3+img2+"</a>";
      } else {
         href = "<a class='direct' href='"+root+active0+"/"+m[i].lvl1+"/"
            +m[i].lvl2+".html'>"+img1+m[i].title2+img2+"</a>";
      }
   } else if (m[i].lvl2 != undefined) {
      if (dir != "up") {
         href = "<a class='direct' href='"+root+active0+"/"+m[i].lvl1+"/"
            +m[i].lvl2+".html'>"+img1+m[i].title2+img2+"</a>";
      } else {
         href = "<a class='direct' href='"+root+active0+"/"+m[i].lvl1
            +".html'>"+img1+m[i].title1+img2+"</a>";
      }
   } else {
      if (dir != "up") {
         href = "<a class='direct' href='"+root+active0+"/"+m[i].lvl1
            +".html'>"+img1+m[i].title1+img2+"</a>";
      } else if (active0 == "home") {
          href = "<a class='direct' href='"+root
             +"index.html'>"+img1+active0+img2+"</a>";
      } else if (active0 == "ddswiki") {
          href = "";
      } else {
          href = "<a class='direct' href='"+root+active0
            +".html'>"+img1+active0+img2+"</a>";
     }
   }
   return href;
}


function getMenu() {
   var m = menu[active0];
   var k = 0;
   iactive = 0;
   
   if (m == undefined || active0 == "ddswiki") return;
   if (active1 != undefined && active2 != "none") title1 = active1;
   if (active2 != undefined && active2 != "none") title2 = active2;
   if (active3 != undefined && active3 != "none") title3 = active3;

   iactive = -1;
   for (i=0;i<m.length;i++) {
      if (m[i].lvl1 == active1) {
         k = 1;
         title1 = m[i].title1;
         if (m[i].lvl2 == active2) {
            k = 2;
            title2 = m[i].title2;
            if (m[i].lvl3 == active3) {
               k = 3;
               title3 = m[i].title3;
               iactive = i;
               return;
            } else if (k >= 3) break;
         } else if (k >= 2) break;
      } else if (k >= 1) break;
   }
}


function setMainLbl() {
   var m = menu[active0];
   var i1=i2=i3=0;
     
   getMenu();

   for (i=0;i<m.length;i++) {
      if (m[i].lvl3 != undefined) {
         if (m[i].lvl2 == active2) {
            ++i3;
            if (i == iactive) {
               mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
                  +"</center>"+ndxLbl[2][i2]+m[i].title2+"</u>"
                  +"<br/>&nbsp;"+ndxLbl[3][i3]+m[i].title3+"</h1>";
            }
         }
      } else if (m[i].lvl2 != undefined) {
         if (m[i].lvl1 == active1) {
            ++i2;
            if (i == iactive) {
               mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
                  +"</center>"+ndxLbl[2][i2]+m[i].title2+"</u></h1>";
            }
         }
      } else {
         ++i1;
         if (i == iactive) {
            mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
               +"</center></u></h1>";
         }
      }
   }
}


function setMenu() {
   var m = menu[active0];
   var spc = "&nbsp;&nbsp;&nbsp;&nbsp;";
   var i1=i2=i3=0;

   if (m == undefined) return;
     
   prev = up = next = "";
   for (i=0;i<m.length;i++) {
      if (m[i].lvl3 != undefined) {
         if (m[i].lvl1 == active1 && m[i].lvl2 == active2) {
            if (i == iactive) {
               document.write(
                  "<div class='menu' style='color:#ffffff;'>"
                  +spc+spc+"&rArr;"
                  +ndxLbl[3][++i3]+m[i].title3+"</div>");
               mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
                  +"</center>"+ndxLbl[2][i2]+m[i].title2+"</u>"
                  +"<br/>&nbsp;"+ndxLbl[3][i3]+m[i].title3+"</h1>";
               prev = setRef(i-1,"prev");
               next = setRef(i+1,"next");
               up   = setRef(i,"up");
            } else {
               document.write("<div class='menu'>"+spc+spc+spc
                  +"<a class='menu' href='"
                  +root+active0+"/"+m[i].lvl1+"/"+m[i].lvl2+"/"
                  +m[i].lvl3+".html'>"
                  +ndxLbl[3][++i3]+m[i].title3+"</a></div>");
            }
         }
      } else if (m[i].lvl2 != undefined) {
         if (m[i].lvl1 == active1) {
            if (i == iactive) {
               document.write(
                  "<div class='menu' style='color:#ffffff;'>"
                  +spc+"&rArr;"
                  +ndxLbl[2][++i2]+m[i].title2+"</div>");
               mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
                  +"</center>"+ndxLbl[2][i2]+m[i].title2+"</u></h1>";
               prev = setRef(i-1,"prev");
               next = setRef(i+1,"next");
               up   = setRef(i,"up");
            } else {
               document.write("<div class='menu'>"+spc+spc
                  +"<a class='menu' href='"
                  +root+active0+"/"+m[i].lvl1+"/"+m[i].lvl2+".html'>"
                  +ndxLbl[2][++i2]+m[i].title2+"</a></div>");
            }
         }
      } else {
         if (i == iactive) {
            document.write(
               "<div class='menu' style='color:#ffffff;'>"
               +spc+ndxLbl[1][++i1]+m[i].title1+"</div>");
            mainLbl="<h1><u><center>"+ndxLbl[1][i1]+m[i].title1
               +"</center></u></h1>";
            prev = setRef(i-1,"prev");
            next = setRef(i+1,"next");
            up   = setRef(i,"up");
         } else {
            document.write("<div class='menu'>"+spc+"<a class='menu' href='"
               +root+active0+"/"+m[i].lvl1+".html'>"
               +ndxLbl[1][++i1]+m[i].title1+"</a></div>");
         }
      }
   }
}


////////////////////////////////////////////////////////////////////////////
//
// Email functions
//
////////////////////////////////////////////////////////////////////////////
function sendEmail(name, domain, subject) {  
   document.location="mailto:"+name+"@"+domain+
      "?subject="+subject;
}


function setEmail(emailName, domain, displayName, subject, color) {
   var a, dn, name, sub, bgc;
   var dnre = /\./;
   var wre = /\w/;

   if (domain != undefined && dnre.test(domain)) {
      dn = domain;
   } else {
      dn = "bp.com";
   }

   if (displayName != undefined && wre.test(displayName)) {
      name = displayName;
   } else {
      name = emailName+"@"+dn;
   }

   if (subject != undefined && wre.test(subject)) {
      sub = escape(subject);
   } else {
      sub = escape("From DDS Website");
   }

   if (color != undefined) {
      bgc = color;
   } else {
      bgc = "#ffffff";
   }

   a="<a style='color:#0000ff;background-color:"+bgc+";' "
      +"href='javascript:sendEmail(\""+emailName+"\",\""
      +dn+"\",\""+sub+"\")'>"+name+"</a>";

   document.write(a);
}


////////////////////////////////////////////////////////////////////////////
//
// print function
//
////////////////////////////////////////////////////////////////////////////
function setPrint() {
   var non = "";
   var tmp = location.search;
   tmp = tmp.replace(/&print=/, "");
   tmp = tmp.replace(/print=&/, "");
   tmp = tmp.replace(/print=/, "");
   tmp = tmp.replace("?", "");
   if (print) {
      non = "Non-";
     	tmp = "?"+tmp;
   } else if (tmp == "") {
      tmp = "?print=";
   } else {
      tmp = "?"+tmp+"&print=";
   }
   document.write("<br/><br/><center><form><input type='button' value='"
      +non+"Print Version' onclick='location=\""+tmp+"\";'><\/form></center>");
}


////////////////////////////////////////////////////////////////////////////
//
// header function
//
////////////////////////////////////////////////////////////////////////////
function header(lvl0,lvl1,lvl2,lvl3) {
   var dir;
   active0 = lvl0;
   active1 = lvl1;
   active2 = lvl2;
   active3 = lvl3;
   if (active3 != undefined) {
      root = "../../../";
   } else if (active2 != undefined) {
      root = "../../";
   } else if (active1 != undefined) {
      root = "../";
   } else {
      root = "";
   }
   
   if (active0 == "all") active0 = "home";
   
   if (print) {
      setPrint();
      setMainLbl();
      document.write("<div style='margin:20;text-align:justify;font-size:10pt;'>");
      if (mainLbl != undefined) document.write(mainLbl+"<p/>");
      return;
   }

   setImages();

   // setup title logo 
   document.write("<table width='100%' border='0' cellspacing='0' "
                  +"cellpadding='0'>");
   document.write("<tr><td colspan='2' style='background-color:#c6c3c6;'>"
                  +"<img src='"+root+"images/Logo.jpg'>&nbsp;&nbsp;"
                  +"<img src='"+root+"images/DDSName.jpg'>"
                  +"</td></tr>");
   // setup main tabs
   document.write("<tr><td colspan='2' style='background-color:#c6c3c6;'>");
   setTab("home");
   setTab("users");
   setTab("applications");
   setTab("developers");
   setTab("codeslinger");
   setTab("maintainers");
   setTab("download");
   //setTab("documents");
   //setTab("ddswiki");
   document.write("</td></tr>");

   // setup bread crumbs
   document.write("<tr><td colspan='2' style='padding:0 0 0 10px;"
                  +"background-color:"+color[active0]+";'>"
                  +"<table width='100%' style='color:#ffffff;"
                  +"font-family:times;font-size:16pt;font-weight:bold;'>"
                  +"<tr><td>"+title[active0]);
   getMenu();
   if (title1 != undefined) {
      document.write("&nbsp;&rarr;&nbsp;"+title1);
   }
   if (title2 != undefined) {
      document.write("&nbsp;&rarr;&nbsp;"+title2);
   }
   if (title3 != undefined) {
      document.write("&nbsp;&rarr;&nbsp;"+title3);
   }

   document.write("</td><td>");

//   document.write("<form style='padding: 10px 0 0 0;' "
//                  +"action='"+root+"search.php' method='GET'>"
//                  +"<input type='hidden' name='action' value='SEARCH'>"
//                  +"<input type='hidden' name='limit' value='25'>"
//                  +"<input type='hidden' name='"+active0+"' value='true' CHECKED>"
//                  +"<input type='text' name='keyword' value='' size='15'>"
//                  +"<input type='submit' value='Search'></form>");
   document.write("</td></tr></table></td></tr>");

   // setup menus
   document.write("<tr><td valign='top' style='color:#f0f000;padding:0 10px 0 0;"
                  +"background-color:"+color[active0]+";'>");
   setMenu();
   setPrint();
   document.write("</td><td style='border-style:solid;"
                  +"border-width:15 0 0 15;border-color:"+edgeColor[active0]
                  +";margin:20;text-align:justify;'>");

   // setup top direction arrows
   directions = "<table width='100%' style='margin:5;'><tr><td width='33%'>";
   dir = 0;
   if (prev == "") {
      directions += "&nbsp;";
   } else {
      dir++;
      directions += prev;
   }
   directions += "</td><td border=1 width='34%' style='text-align:center;'>";
   if (up == "") {
      directions += "&nbsp;";
   } else {
      dir++;
      directions += up;
   }
   directions += "</td><td width='33%' style='text-align:right;'>";
   if (next == "") {
      directions += "&nbsp;";
   } else {
      dir++;
      directions += next;
   }
   directions += "</td></tr></table>";
   if (dir) {
      document.write(directions+"<hr/>");  
   } else {
      directions = "";
   }

   // setup for generic page & title
   document.write("<div style='margin:20;text-align:justify;font-size:14pt;'>");
   if (mainLbl != undefined) document.write(mainLbl+"<p/>");
}


////////////////////////////////////////////////////////////////////////////
//
// footer function
//
////////////////////////////////////////////////////////////////////////////
function footer() {

   document.write("</div>");
   
   if (print) return;

   // setup bottom direction arrows
   if (directions) document.write("<hr/>"+directions);

   // setup bottom info
   document.write("</td></tr>");
   document.write("<tr><td style='background-color:"+color[active0]+";'>"
      +"</td><td style='background-color:"+edgeColor[active0]
      +";text-align:center;font-size:10pt'>"
      +"<address>For additional help, open an issue  <a href='https://github.com/bp/FreeDDS/issues'>here</a>");
   document.write("</address></td></tr>");

   document.write("<tr><td id='footer' colspan='2'>"
      +"Copyright 2010 &copy; BP America Inc, Houston, Tx, USA. "
      +"All Rights Reserved.</td></tr></table>");
}

// Javascript routines for maintaining api lists

////////////////////////////////////////////////////////////////////////////
//
// api management 
//
////////////////////////////////////////////////////////////////////////////
function api(dir, routine) {
   if (api == undefined) api = new Array();
   if (api[dir] == undefined) api[dir] = new Array();

   if (routine == "setargcv") {
      api[dir][api[dir].length] = {ref:routine, name:routine};
   } else {
      switch (dir) {
      case "capi":
         api[dir][api[dir].length] = {ref:routine, name:"cdds_"+routine};
         break;
      case "fapi":
         api[dir][api[dir].length] = {ref:routine, name:"fdds_"+routine};
         break;
      case "cconvenience":
         api[dir][api[dir].length] = {ref:routine, name:"cddx_"+routine};
         break;
      case "fconvenience":
         api[dir][api[dir].length] = {ref:routine, name:"fddx_"+routine};
         break;
      case "fft":
         api[dir][api[dir].length] = {ref:routine, name:"fft_"+routine};
         break;
      default:
         api[dir][api[dir].length] = {ref:routine, name:routine};
      }
   }
}

api("gio","gio"); 
api("gio","gopen"); 
api("gio","gopenmax"); 
api("gio","gclose"); 
api("gio","gfcntl"); 
api("gio","gsetbuf"); 
api("gio","gsetpos"); 
api("gio","gseek"); 
api("gio","gread"); 
api("gio","gwrite"); 
api("gio","gerror"); 


////////////////////////////////////////////////////////////////////////////
//
// index header 
//
////////////////////////////////////////////////////////////////////////////
function index_header(dir,sub) {
   var tmp;
   tmp = "<table border=0 width=100% valign=top><tr>";

   if (dir != undefined) {
      if (sub == undefined) {
         sub = "";
      } else {
         sub = sub+"/";
      }

      tmp += "<td style='width:100px;vertical-align:top;"
         +"background-color:#e0e0e0;padding:0px 10px 10px 10px;"
         +"border:3px ridge #808080;'>"
         +"<center style='font-weight:bold;font-size:18pt;"
         +"text-decoration:underline;'>Index</center>";
      for(i=0;i<api[dir].length;i++) {
         tmp += "<a href='"+sub+api[dir][i].ref+".html'>"
            +api[dir][i].name+"</a><br/>";
      }
      tmp += "</td>";
   }

   tmp += "<td style='vertical-align:top;padding:0px 0px 0px 20px;'>";
   document.write(tmp);
}

////////////////////////////////////////////////////////////////////////////
//
// index footer
//
////////////////////////////////////////////////////////////////////////////
function index_footer(dir,sub) {
   var tmp, a;
   
   tmp = "";
   
   if (dir != undefined) {
      if (sub == undefined) {
         sub = "";
      } else {
         sub = sub+"/";
      }

      tmp = "</td><td style='width:100px;vertical-align:top;"
         +"background-color:#e0e0e0;padding:0px 10px 10px 10px;"
         +"border:3px ridge #808080;'><h3>Index</h3>";
      for(i=0;i<api[dir].length;i++) {
         tmp += "<a href='"+sub+api[dir][i].ref+".html'>"
            +api[dir][i].name+"</a><br/>";
      }
   }

   tmp += "</td></tr></table>";
   document.write(tmp);
}

// Javascript routines for maintaining api short lists

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
      api[dir][api[dir].length] = {ref:routine, name:routine};
   }
}


api("capishort","cdds_axis_prod"); 
api("capishort","cdds_close"); 
api("capishort","cdds_closepr"); 
api("capishort","cdds_dict"); 
api("capishort","cdds_errors"); 
api("capishort","cdds_free"); 
api("capishort","cdds_get"); 
api("capishort","cdds_index"); 
api("capishort","cdds_initmpi"); 
api("capishort","cdds_initopenmp"); 
api("capishort","cdds_lseek8"); 
api("capishort","cdds_malloc"); 
api("capishort","cdds_member"); 
api("capishort","cdds_openpr"); 
api("capishort","cdds_printf"); 
api("capishort","cdds_prtcon"); 
api("capishort","cdds_prterr"); 
api("capishort","cdds_prtmsg"); 
api("capishort","cdds_put"); 
api("capishort","cdds_scanf"); 
api("capishort","cdds_scank"); 
api("capishort","cdds_scant"); 
api("capishort","cddx_dict"); 
api("capishort","cddx_in"); 
api("capishort","cddx_in2"); 
api("capishort","cddx_out"); 
api("capishort","cddx_read"); 
api("capishort","cddx_write"); 


api("fapishort","fdds_axis_prod"); 
api("fapishort","fdds_close"); 
api("fapishort","fdds_closepr"); 
api("fapishort","fdds_dict"); 
api("fapishort","fdds_errors"); 
api("fapishort","fdds_free"); 
api("fapishort","fdds_get"); 
api("fapishort","fdds_index"); 
api("fapishort","fdds_initmpi"); 
api("fapishort","fdds_initopenmp"); 
api("fapishort","fdds_lseek8"); 
api("fapishort","fdds_malloc8"); 
api("fapishort","fdds_member"); 
api("fapishort","fdds_openpr"); 
api("fapishort","fdds_printf"); 
api("fapishort","fdds_prtcon"); 
api("fapishort","fdds_prterr"); 
api("fapishort","fdds_prtmsg"); 
api("fapishort","fdds_put"); 
api("fapishort","fdds_scanf"); 
api("fapishort","fdds_scank"); 
api("fapishort","fddx_dict"); 
api("fapishort","fddx_in"); 
api("fapishort","fddx_in2"); 
api("fapishort","fddx_out"); 
api("fapishort","fddx_read"); 
api("fapishort","fddx_write"); 


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

   tmp += "<td style='vertical-align:top;padding:0px 20px 0px 20px;font-size:14pt;'>";
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

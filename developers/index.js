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
      case "cxapi":
         api[dir][api[dir].length] = {ref:routine, name:"cddx_"+routine};
         break;
      case "fxapi":
         api[dir][api[dir].length] = {ref:routine, name:"fddx_"+routine};
         break;
      case "fft":
         api[dir][api[dir].length] = {ref:routine, name:"fft_"+routine};
         break;
      case "cfft":
         api[dir][api[dir].length] = {ref:routine, name:"cfft_"+routine};
         break;
      default:
         api[dir][api[dir].length] = {ref:routine, name:routine};
      }
   }
}


api("fapi","addaxis"); 
api("fapi","align"); 
api("fapi","axis_prod"); 
api("fapi","binfd"); 
api("fapi","calloc"); 
api("fapi","calloc8"); 
api("fapi","check"); 
api("fapi","close"); 
api("fapi","closepr"); 
api("fapi","closepr2"); 
api("fapi","copy"); 
api("fapi","count"); 
api("fapi","datadefn"); 
api("fapi","dict"); 
api("fapi","errors"); 
api("fapi","fflush"); 
api("fapi","free"); 
api("fapi","genus"); 
api("fapi","get"); 
api("fapi","history"); 
api("fapi","in"); 
api("fapi","index"); 
api("fapi","info"); 
api("fapi","initmpi"); 
api("fapi","initmpix"); 
api("fapi","initopenmp"); 
api("fapi","isreg"); 
api("fapi","key"); 
api("fapi","lseek"); 
api("fapi","lseek8"); 
api("fapi","malloc"); 
api("fapi","malloc8"); 
api("fapi","map"); 
api("fapi","member"); 
api("fapi","memuse"); 
api("fapi","mod"); 
api("fapi","name"); 
api("fapi","newer"); 
api("fapi","next"); 
api("fapi","object"); 
api("fapi","offset"); 
api("fapi","open"); 
api("fapi","openm"); 
api("fapi","openpr"); 
api("fapi","out"); 
api("fapi","parent"); 
api("fapi","prec"); 
api("fapi","prev"); 
api("fapi","prime"); 
api("fapi","printf"); 
api("fapi","printt"); 
api("fapi","printv"); 
api("fapi","prtcon"); 
api("fapi","prterr"); 
api("fapi","prthdr"); 
api("fapi","prtmsg"); 
api("fapi","ptr"); 
api("fapi","put"); 
api("fapi","read"); 
api("fapi","readm"); 
api("fapi","realloc"); 
api("fapi","realloc8"); 
api("fapi","rmaxis"); 
api("fapi","savepr"); 
api("fapi","scale"); 
api("fapi","scanf"); 
api("fapi","scank"); 
api("fapi","scant"); 
api("fapi","scantn"); 
api("fapi","scanv"); 
api("fapi","sprintf"); 
api("fapi","sscanf"); 
api("fapi","switch"); 
api("fapi","write"); 
api("fapi","writem"); 

api("fxapi","addaxis"); 
api("fxapi","dict"); 
api("fxapi","getbin"); 
api("fxapi","getdict"); 
api("fxapi","in"); 
api("fxapi","in2"); 
api("fxapi","index"); 
api("fxapi","inhdr"); 
api("fxapi","mod"); 
api("fxapi","mod2"); 
api("fxapi","out"); 
api("fxapi","outhdr"); 
api("fxapi","read"); 
api("fxapi","readall"); 
api("fxapi","readfrom"); 
api("fxapi","rmaxis"); 
api("fxapi","write"); 
api("fxapi","writeto"); 
api("fxapi","wtime"); 

api("fft","nrfft5"); 
api("fft","nrfft5odd"); 
api("fft","ccfftm"); 
api("fft","ccfft2d"); 
api("fft","ccfft3d"); 
api("fft","crfftm"); 
api("fft","crfft2d"); 
api("fft","crfft3d"); 
api("fft","rcfftm"); 
api("fft","rcfft2d"); 
api("fft","rcfft3d"); 
api("fft","zzfftm"); 
api("fft","zzfft2d"); 
api("fft","zzfft3d"); 

api("capi","addaxis"); 
api("capi","align"); 
api("capi","axis_prod"); 
api("capi","binfd"); 
api("capi","calloc"); 
api("capi","check"); 
api("capi","close"); 
api("capi","closepr"); 
api("capi","closepr2"); 
api("capi","copy"); 
api("capi","count"); 
api("capi","datadefn"); 
api("capi","dict"); 
api("capi","errors"); 
api("capi","fflush"); 
api("capi","free"); 
api("capi","genus"); 
api("capi","get"); 
api("capi","history"); 
api("capi","in"); 
api("capi","index"); 
api("capi","info"); 
api("capi","initmpi"); 
api("capi","initmpix"); 
api("capi","initopenmp"); 
api("capi","isreg"); 
api("capi","key"); 
api("capi","lseek"); 
api("capi","lseek8"); 
api("capi","malloc"); 
api("capi","map"); 
api("capi","member"); 
api("capi","memuse"); 
api("capi","mod"); 
api("capi","name"); 
api("capi","newer"); 
api("capi","next"); 
api("capi","object"); 
api("capi","offset"); 
api("capi","open"); 
api("capi","openm"); 
api("capi","openpr"); 
api("capi","out"); 
api("capi","parent"); 
api("capi","prec"); 
api("capi","prev"); 
api("capi","prime"); 
api("capi","printf"); 
api("capi","printt"); 
api("capi","printv"); 
api("capi","prtcon"); 
api("capi","prterr"); 
api("capi","prthdr"); 
api("capi","prtmsg"); 
api("capi","put"); 
api("capi","read"); 
api("capi","readm"); 
api("capi","realloc"); 
api("capi","rmaxis"); 
api("capi","savepr"); 
api("capi","scale"); 
api("capi","scanf"); 
api("capi","scank"); 
api("capi","scant"); 
api("capi","scantn"); 
api("capi","scanv"); 
api("capi","switch"); 
api("capi","write"); 
api("capi","writem"); 
api("capi","setargcv"); 

api("cxapi","addaxis"); 
api("cxapi","dict"); 
api("cxapi","getbin"); 
api("cxapi","getdict"); 
api("cxapi","in"); 
api("cxapi","in2"); 
api("cxapi","index"); 
api("cxapi","inhdr"); 
api("cxapi","mod"); 
api("cxapi","mod2"); 
api("cxapi","out"); 
api("cxapi","outhdr"); 
api("cxapi","read"); 
api("cxapi","readall"); 
api("cxapi","readfrom"); 
api("cxapi","rmaxis"); 
api("cxapi","write"); 
api("cxapi","writeto"); 
api("cxapi","wtime"); 

api("cfft","nrfft5"); 
api("cfft","nrfft5odd"); 
api("cfft","ccfftm"); 
api("cfft","ccfft2d"); 
api("cfft","ccfft3d"); 
api("cfft","crfftm"); 
api("cfft","crfft2d"); 
api("cfft","crfft3d"); 
api("cfft","rcfftm"); 
api("cfft","rcfft2d"); 
api("cfft","rcfft3d"); 
api("cfft","zzfftm"); 
api("cfft","zzfft2d"); 
api("cfft","zzfft3d"); 

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

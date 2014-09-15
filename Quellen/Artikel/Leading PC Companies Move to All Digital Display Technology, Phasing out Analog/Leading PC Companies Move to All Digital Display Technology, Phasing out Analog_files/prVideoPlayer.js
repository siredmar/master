if($j(".prVideoPlayer").length){$j(".prVideoPlayer").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e="";e='<div align="center"><object width="'+c+'" height="'+d+'">'+'<param name="movie" value="http://www.youtube.com/v/'+b+'?fs=1&hl=en_US"></param>'+'<param name="allowFullScreen" value="true"></param>'+'<param name="allowscriptaccess" value="sameDomain"></param>'+'<embed src="http://www.youtube.com/v/'+b+'?fs=1&hl=en_US" '+'type="application/x-shockwave-flash" '+'allowscriptaccess="samedomain" '+'allowfullscreen="true" width="'+c+'" height="'+d+'" wmode="transparent"></embed>'+"</object><br/>\n"+$j(this).html()+"</div>";$j(this).html(e);$j(this).attr("title","")})}if($j(".prVideoPlayerEmbed").length){$j(".prVideoPlayerEmbed").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e="";e='<div align="center"><object width="'+c+'" height="'+d+'">'+'<param name="movie" value="http://www.youtube.com/p/'+b+'?hl=en_US&fs=1"></param>'+'<param name="allowFullScreen" value="true"></param>'+'<param name="allowscriptaccess" value="sameDomain"></param>'+'<embed src="http://www.youtube.com/p/'+b+'?hl=en_US&fs=1" '+'type="application/x-shockwave-flash" '+'allowscriptaccess="samedomain" '+'allowfullscreen="true" width="'+c+'" height="'+d+'" wmode="transparent"></embed>'+"</object><br/>\n"+$j(this).html()+"</div>";$j(this).html(e);$j(this).attr("title","")})}if($j(".prVideoSlideShow").length){$j(".prVideoSlideShow").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e="";e='<div align="center"><object width="'+c+'" height="'+d+'"> '+'<param name="flashvars" value="offsite=true&lang=en-us&page_show_url=%2Fphotos%2Fintelfreepress%2Fsets%2F'+b+"%2Fshow%2F&page_show_back_url=%2Fphotos%2Fintelfreepress%2Fsets%2F"+b+"%2F&set_id="+b+'&jump_to="></param> '+'<param name="movie" value="http://www.flickr.com/apps/slideshow/show.swf?v=71649"></param>  '+'<param name="allowFullScreen" value="true"></param> '+"<embed  "+'type="application/x-shockwave-flash" '+'src="http://www.flickr.com/apps/slideshow/show.swf?v=71649"  '+'allowFullScreen="true" '+'flashvars="offsite=true&lang=en-us&page_show_url=%2Fphotos%2Fintelfreepress%2Fsets%2F'+b+"%2Fshow%2F&page_show_back_url=%2Fphotos%2Fintelfreepress%2Fsets%2F"+b+"%2F&set_id="+b+'&jump_to=" '+'width="'+c+'" '+'height="'+d+'" wmode="transparent"> '+"</embed> "+"</object><br/>\n"+$j(this).html()+"</div>";$j(this).html(e);$j(this).attr("title","")})}if($j(".prMultiVideoPlayer").length){$j(".prMultiVideoPlayer").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e="";e='<div align="center" height="360px"><object width="'+c+'" height="'+d+'">'+'<param name="movie" value="http://www.youtube.com/cp/'+b+'"></param>'+'<param name="allowFullScreen" value="true"></param>'+'<param name="allowscriptaccess" value="sameDomain"></param>'+'<embed src="http://www.youtube.com/cp/'+b+'" '+'type="application/x-shockwave-flash" '+'allowscriptaccess="samedomain" '+'allowfullscreen="true" width="'+c+'" height="'+d+'" wmode="transparent"></embed>'+'</object><p></p><div style="margin-top:-50px">'+$j(this).html()+"</div></div>";$j(this).html(e);$j(this).attr("title","")})}if($j(".prNonStandardVideo").length){$j(".prNonStandardVideo").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e="";e='<object width="'+c+'" height="'+d+'"> '+'<param name="flashvars" value="'+b+'"></param> '+'<param name="movie" value="'+b+'"></param> '+'<param name="allowFullScreen" value="true"></param> '+'<param name="wmode" value="transparent"></param> '+'<embed type="application/x-shockwave-flash" src="'+b+'"  '+'allowFullScreen="true" flashvars="" width="'+c+'" height="'+d+'"> '+"</embed>"+"</object>";$j(this).html(e);$j(this).attr("title","")})}if($j(".prFlickrSlideshow").length){$j(".prFlickrSlideshow").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e=a[3];var f="";f='<object width="'+d+'" height="'+e+'"> '+'<param name="flashvars" value="'+c+'"></param> '+'<param name="movie" value="'+b+'"></param> '+'<param name="allowFullScreen" value="true"></param> '+'<embed type="application/x-shockwave-flash" src="'+b+'"  '+'allowFullScreen="true" flashvars="'+c+'" width="'+d+'" height="'+e+'"> '+"</embed>"+"</object>";$j(this).html(f);$j(this).attr("title","")})}if($j(".prMediaWithVariable").length){$j(".prMediaWithVariable").each(function(){var a=$j(this).attr("title").split("|");var b=a[0];var c=a[1];var d=a[2];var e=a[3];var f="";f='<div align="center" height="'+d+'" width="'+c+'">'+'<embed src="'+b+'" height="'+d+'" width="'+c+'" allowscriptaccess="samedomain" allowfullscreen="true"'+' flashvars="'+e+'"/></div>';$j(this).html(f);$j(this).attr("title","")})}if($j(".prPodCast").length){$j(".prPodCast").each(function(){var a=$j(this).attr("title").split("|")[1];var b=$j(this).attr("title").split("|")[0];var c="<embed "+'type="application/x-shockwave-flash" '+'flashvars="audioUrl='+b+'"'+'src="http://www.google.com/reader/ui/3523697345-audio-player.swf" '+'width="400" '+'height="27" '+'quality="best">'+"</embed>";$j(this).html("<p>"+a+"<p>"+c+"<p>Click on the play button above to listen to the full audio Podcast</p>");$j(this).attr("title","")})}

if($j(".prFlashPlayer").length){
  $j(".prFlashPlayer").each(function(){
  var iSrc = $j(this).attr("title").split("|");
  $j(this).html("<iframe src='"+ iSrc[0]+"' width='"+ iSrc[1]+"' height='"+ iSrc[2]+"' marginheight='0' marginwidth='0' frameborder='0' scrolling='no' />");
})
}

if($j(".prLivecast").length){
  $j(".prLivecast").each(function(){
  var iSrc = $j(this).attr("title").split("|");
  $j(this).html('<iframe width="'+iSrc[1]+'" height="'+iSrc[2]+'" src="'+iSrc[0]+'" style="border:0;outline:0" frameborder="0" scrolling="no"></iframe>');
})
}

//brightcove
if($j(".prBrightCove").length){
  $j(".prBrightCove").each(function(){
  var iSrc = $j(this).attr("title").split("|");

  var str = '<object id="flashObj" width="'+iSrc[2]+'" height="'+iSrc[3]+'" '
            + 'classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" '
            + 'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,47,0">'
            + '<param name="movie" value="'+iSrc[0]+'" />'
            + '<param name="bgcolor" value="#FFFFFF" />'
            + '<param name="width" value="'+iSrc[2]+'" />'
            + '<param name="height" value="'+iSrc[3]+'" />'
            + '<param name="flashVars" value="'+iSrc[1]+'" />'
            + '<param name="base" value="http://admin.brightcove.com" />'
            + '<param name="seamlesstabbing" value="false" />'
            + '<param name="allowFullScreen" value="true" />'
            + '<param name="swLiveConnect" value="true" />'
            + '<param name="wmode" value="transparent" />'
            + '<param name="allowScriptAccess" value="always" />'
            + '<embed src="'+iSrc[0]+'" bgcolor="#FFFFFF" flashVars="'+iSrc[1]+'" '
            + 'base="http://admin.brightcove.com" name="flashObj" width="'+iSrc[2]+'" height="'+iSrc[3]+'" '
            + 'seamlesstabbing="false" type="application/x-shockwave-flash" allowFullScreen="true" '
            + 'allowScriptAccess="always" swLiveConnect="true" wmode="transparent" '
            + 'pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash"></embed>'
            + '</object>';

         $j(this).html(str);

         })
}

function renderPlusone(){
  //gapi.plusone.render("gplusone",{size:"medium",count:"true"});
}
var shortURL="";
 function bitlyfi(url, func) {
    // Put your own login and apiKey
    var defaults = {
        login: 'pdarlingintel',
        apiKey: 'R_1694c6bd53a0bd466235970f4d655344',
        longUrl: url
    };
    // Build the URL to query
    var bitly = "http://api.bit.ly/v3/shorten?" + "&login=" + defaults.login + "&apiKey=" + defaults.apiKey + "&longUrl=" + defaults.longUrl + "&format=json&callback=?";

    // Utilize the bit.ly API
    $j.getJSON(bitly, function (results) {
        if (results.status_txt === "OK") {
            return func(results.data["url"]);
        }
        else {
            return func(url);
        }
    });
}

document.write('<script type="text/javascript" src="http://platform.tumblr.com/v1/share.js"></scr'+"ipt>");
//document.write('<script type="text/javascript" src="http://static.ak.fbcdn.net/connect.php/js/FB.Share"></scr'+"ipt>");
document.write('<script type="text/javascript" src="http://platform.twitter.com/widgets.js"></scr'+"ipt>");
document.write('<script type="text/javascript" src="http://www.google.com/buzz/api/button.js"></scr'+"ipt>");


var prTweetButton="";
var prFBButton="";
var prGoogleButton="";
var prEmailPrint="";
var prTumblrButton="";
var socialDescription=$j("#socialDescription").length?$j("#socialDescription").html():"";
var socialThumbnail=$j("#socialThumbnail").length?$j("#socialThumbnail").attr("src"):"";
var tumblr_photo_source=socialThumbnail;
var tumblr_photo_caption=socialDescription;
var tumblr_photo_click_thru=window.location.href;
var prShareThis="";
var prVia=typeof showVia!="undefined"?showVia:"";
var prTitle=$j.trim(document.title);
var prImgBase="http://www.intel.com/newsroom/assets/images/";
var searchRegExp="Intel Newsroom: ";
var matchPos=prTitle.search(searchRegExp);
if(matchPos!=-1){
    var replaceStr="";
    prTitle=prTitle.replace("Intel Newsroom: ",replaceStr);
}

        prTweetButton='<a href="http://twitter.com/share?url='+encodeURIComponent(location.href)+"&via="+prVia+'&original_referer='+encodeURIComponent(location.href)+'" class="twitter-share-button">Tweet</a>';


        document.write('<div id="fb-root"></div> '
                        +' <script>(function(d, s, id) { '
                          +' var js, fjs = d.getElementsByTagName(s)[0];  '
                          +' if (d.getElementById(id)) return;    '
                          +' js = d.createElement(s); js.id = id;   '
                          +' js.src = "//connect.facebook.net/en_US/all.js#xfbml=1&appId=9ad773232a5788f71e39f44dba81e07f";   '
                          +' fjs.parentNode.insertBefore(js, fjs);    '
                        +' }(document, "script", "facebook-jssdk"));</script>');


        prFBButton = '<div class="fb-like" data-send="false" data-layout="button_count" data-width="35" data-show-faces="true"></div>';

        prTumblrButton='<a href="http://www.tumblr.com/share/photo?source='+encodeURIComponent(tumblr_photo_source)+"&caption="+encodeURIComponent(tumblr_photo_caption)+"&clickthru="+encodeURIComponent(tumblr_photo_click_thru)+'" title="Share on Tumblr" style="display:block; text-indent:-9999px; overflow:hidden; width:20px; height:20px; background:url(http://platform.tumblr.com/v1/share_4.png) top left no-repeat transparent;">Share on Tumblr</a>';
        prGooglePlusOne="";
        prEmailPrint='<div style="margin:0 0 0 0">'+'<a href="javascript:self.print();" onMouseOver="window.status=\'Print\'; return true;" '+"onMouseOut=\"window.status=''; return true;\">"+'<img src="'+prImgBase+'gbl-nav-print.png" width="26" height="17" alt="Print" title="Print" class="icon" border="0" /></a>'+'<a href="javascript:emailFriend();" onMouseOver="window.status=\'Email a Friend\';return true;" style="margin-left:5px">'+'<img src="'+prImgBase+'gbl-nav-email.png" style="width:26px;height:17" title="Email a Friend" class="icon" border="0" /></a></div>';
        prShareThis='<span id="shareThisButton"></span>';
        var prFontSize='<img class="btnDecrease" src="'+prImgBase+'NR_fontsize_btn_minus.png" border="0"/>'+'<img class="btnIncrease" src="'+prImgBase+'NR_fontsize_btn_plus.png" border="0"/>'+'<img src="'+prImgBase+'NR_fontsize_btn_AAA.png" border="0" class="btnIncrease"/>';
        $j(document).ready(function(){
            var a=$j("#prMasterContainer").length?$j("#prMasterContainer"):$j("#fpMasterContainer");
            $j(a).css("font-size",$j("#prMasterContainer").length?"12px":"14px");
            var b="12";
            $j(".btnIncrease").click(function(){
                b=parseInt(b)+1;
                $j(a).css("font-size",b+"px");
                $j("#prSocialWidget").css("font-size","12px")});
                $j(".btnDecrease").click(function(){b=parseInt(b)-1;
                $j(a).css("font-size",b+"px")})
        });

        if($j("#prSocialWidget").length){
            if($j("#prSocialWidget").length){
                $j("#prSocialWidget").html('<div style="margin: 1pt 0px 10px; width: 650px;">'
                                            +'<div style="float: left">'+prTweetButton+'</div>'
                                            +'<div style="float: left; margin-right: 10px;margin-left:-6px;">'+prFBButton+'</div>'
                                            +'<div style="float: left; margin-right: 10px;">'+prTumblrButton+'</div>'
                                            +'<div style="float: left; margin-left:-2px;">'
                                                +'<span class="g-plusone" data-size="medium">+1</span>'+prGooglePlusOne+'</div>'
                                            +'<div style="float: left; margin-right:5px;margin-left:-25px">'+prShareThis+'</div>'
                                            +'<div style="float: left; margin-right:0px">'+prEmailPrint+'</div>'
                                            +'<div style="float: right;">'+prFontSize+'</div>'
                                            +'</div><p style="clear:both"> </p><br/>')
            }

        }


//Place this tag after the last +1 button tag.

  (function() {
    var po = document.createElement('script'); po.type = 'text/javascript'; po.async = true;
    po.src = 'https://apis.google.com/js/plusone.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(po, s);
  })();




var URL,width,height,scroll,menubar,toolbar,resize,xPos,yPos,s_gs;winName = "";
function openWin(URL,width,height,scroll,menubar,toolbar,resize,xPos,yPos,winName){
	 var focusFail = false;
	 if (width == "") width = "500";
	 if (height == "") height = "500";
	 if (scroll == "") scroll = "auto";
	 if (menubar =="") menubar = "no";
	 if (toolbar == "") toolbar = "no";
	 if (resize == "") resize = "yes";
	 if (xPos == "") xPos = "5";
	 if (yPos == "") yPos = "5";
	 if (winName == "") winName = "win";
	 var features ="width=" +width+ ",height=" +height+ ",scrollbars=" +scroll+ ",menubar=" +menubar+ ",toolbar=" +toolbar+ ",resizable=" +resize+ ",left=" +xPos+ ",top=" +yPos;
	 var newWin = window.open(URL,winName,features);
	 if (navigator.appVersion.indexOf("NT")!= -1){
	 if (navigator.appVersion.indexOf("NT 5")!= -1){
	 focusFail = false;
	 } else if (ie4 == true) focusFail = true;
	 }
	 if (focusFail == false){
	 if (window.focus) newWin.focus();
	 }
}

function emailFriend(location) {
	if (!location) { location = "en_US"; }
	var shareUrl = "http://www.intel.com/sites/sitewide/" + location + "/emailfriend.htm?param=" + document.location.href;
	openWin(shareUrl,340,550,"no","no","no","yes",60,40,"emailfriend");
}

 function trimMe(stringToTrim) {return stringToTrim.replace(/^\s+|\s+$/g,"");}

	function relative_time(time_value) {
      var parsed_date = Date.parse(time_value);
      var relative_to = (arguments.length > 1) ? arguments[1] : new Date();
      var delta = parseInt((relative_to.getTime() - parsed_date) / 1000);
      if(delta < 60) {
      return 'less than a minute ago';
      } else if(delta < 120) {
      return 'about a minute ago';
      } else if(delta < (45*60)) {
      return (parseInt(delta / 60)).toString() + ' minutes ago';
      } else if(delta < (90*60)) {
      return 'about an hour ago';
      } else if(delta < (24*60*60)) {
      return 'about ' + (parseInt(delta / 3600)).toString() + ' hour(s) ago';
      } else if(delta < (48*60*60)) {
      return '1 day ago';
      } else {
      return (parseInt(delta / 86400)).toString() + ' days ago';
      }
    }

	function returnFormat(t,d,at){
		s = t;
		d = relative_time(d);
		bit = s.replace(httpHash, "<a href='$1' target='_blank'>$1</a>");
		h = bit.replace(linkBit,"<a href='http://twitter.com/$1' target='_blank'>@$1</a>")
		str = "<li><a href='http:\/\/www.twitter.com/"+at+"' target='_blank'>"+at+"</a> - "+h.replace(linkHash, " <a href='http://search.twitter.com/search?q=&tag=$1&lang=all&from=Intel OR IntelNews' target='_blank'>#$1</a>")+" - "+d+"<br/><div class='hr-dotted' style='margin-left: -2px;'></div></li>";
		return str;
	}

function stripSpecialCharacter(w){
   var regExp = /\s+/g;
   return w.replace(regExp,'');
}

var httpHash = /((ftp|http|https):\/\/(\w+:{0,1}\w*@)?(\S+)(:[0-9]+)?(\/|\/([\w#!:.?+=&%@!\-\/]))?)/gi;
var linkBit = /[\@]+([A-Za-z0-9-_]+)/gi;
var linkHash = / [\#]+([A-Za-z0-9-_]+)/gi;

function getQuerystring(key, default_)
{
  if (default_==null) default_="";
  key = key.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
  var regex = new RegExp("[\\?&]"+key+"=([^&#]*)");
  var qs = regex.exec(window.location.href);
  if(qs == null)
    return default_;
  else
    return qs[1];
}

function gFlash(){
    try {
      try {
        // avoid fp6 minor version lookup issues
        var axo = new ActiveXObject('ShockwaveFlash.ShockwaveFlash.6');
        try { axo.AllowScriptAccess = 'samedomain'; }
        catch(e) { return '6,0,0'; }
      } catch(e) {}
      return new ActiveXObject('ShockwaveFlash.ShockwaveFlash').GetVariable('$version').replace(/\D+/g, ',').match(/^,?(.+),?$/)[1];
      // other browsers
      } catch(e) {
      try {
        if(navigator.mimeTypes["application/x-shockwave-flash"].enabledPlugin){
          return (navigator.plugins["Shockwave Flash 2.0"] || navigator.plugins["Shockwave Flash"]).description.replace(/\D+/g, ",").match(/^,?(.+),?$/)[1];
        }
      } catch(e) {}
    }
    return '0,0,0';
}

//generic expand
 var expandBaseImageUrl = "http://www.intel.com/newsroom/assets/images/";
 $j(document).ready(function() {
     //expand
     $j(".openSesameToo").click(function(){
       var id = $j(this).attr("title");
       var id_img = $j(this).attr("src");
       //alert(id)
       if(id_img==expandBaseImageUrl+"blue_downArrow.png"){
            $j(this).attr("src",expandBaseImageUrl+"blue_upArrow.png");
            $j("#more_"+id).html("less ");
            $j("#"+id).show("slow");
         }else{
            $j(this).attr("src",expandBaseImageUrl+"blue_downArrow.png");
            $j("#more_"+id).html("more ");
            $j("#"+id).hide("slow");
         }
     })
  });

 $j(".openSesame").each(function(){
   $j(this).html("<span style='float:right;margin:3px;cursor: pointer; border: solid 1px #e6e6e6; padding: 3px 3px 3px 2px;'><span id='more_"+$j(this).attr("title")+"'>more </span><img src='"+expandBaseImageUrl+"blue_downArrow.png' title='"+$j(this).attr("title")+"' class='openSesameToo' style='cursor:pointer'/></span>")  
 })

 // photo viewer -->
var feedTitleURL = ""
var viewParam = "";
var viewWidth = "637";
var viewHeight = "400";

if($j(".prPhotoViewerContainer").length){
  $j(".prPhotoViewerContainer").each(function(){
    feedTitleURL = $j(this).attr("title");

    if(feedTitleURL.split("|")[1] == "s"){
        viewWidth = "223";viewHeight = "210";
    }else if(feedTitleURL.split("|")[1] == "m"){
            viewWidth = "465";viewHeight = "292";
    }else if(feedTitleURL.split("|")[1] == "t" || feedTitleURL.split("|")[1] == "tp"){
            viewWidth = "465";viewHeight = "605";
    }else{
        viewWidth = "690";viewHeight = "400";
    }

    if(feedTitleURL.split("|")[1] == "t" || feedTitleURL.split("|")[1] == "tp"){
    $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/photo-video/prPhotoViewFrame_tall.htm?p="+encodeURIComponent(feedTitleURL)+"' width='"+viewWidth+"' height='"+viewHeight+"' frameborder='0' scrolling='no'></iframe>")
}else{

     if(feedTitleURL.split("|")[1] == "s" || feedTitleURL.split("|")[1] == "m"){
        $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/photo-video/prPhotoViewFrame.htm?p="+encodeURIComponent(feedTitleURL)+"' width='"+viewWidth+"' height='"+viewHeight+"' frameborder='0' scrolling='no'></iframe>")
    }else{
        $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/2014/prPhotoViewerKit.htm?p="+encodeURIComponent(feedTitleURL)+"' width='"+viewWidth+"' height='"+viewHeight+"' frameborder='0' scrolling='no'></iframe>")

         }
     }
  })
}

if($j(".prPhotoViewerKit").length){
  $j(".prPhotoViewerKit").each(function(){

    feedTitleURL = $j(this).attr("title");
    viewWidth = "690";viewHeight = "400";

   $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/2014/prPhotoViewerKit.htm?p="+encodeURIComponent(feedTitleURL)+"' width='"+viewWidth+"' height='"+viewHeight+"' frameborder='0' scrolling='no'></iframe>")

  })
}

// video viewer -->
var vidTitleURL = ""
var vidContainerWidth = "637";
var vidContainerHeight = "400";
if($j(".prVideoViewerContainer").length || $j(".prVideoPlayerKit").length || $j(".prVideoViewerContainerTall").length || $j(".prVideoViewerContainerSpecial").length){
  $j(".prVideoViewerContainer").each(function(){
    vidTitleURL = $j(this).attr("title");
    if(vidTitleURL.split("|")[1]=="s"){
      vidContainerWidth = "225";vidContainerHeight = "385";
    }else if(vidTitleURL.split("|")[1]=="m"){
      vidContainerWidth = "469";vidContainerHeight = "295";
    }else{
     vidContainerWidth = "690";vidContainerHeight = "530";
    }

    if(vidTitleURL.split("|")[1] == "s" || vidTitleURL.split("|")[1] == "m"){
        $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/photo-video/prVideoViewFrame.htm?p="+encodeURIComponent(vidTitleURL)+"' width='"+vidContainerWidth+"' height='"+vidContainerHeight+"' frameborder='0' scrolling='no'></iframe>")
    }else{
        $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/2014/prVideoPlayerKit.htm?p="+encodeURIComponent(vidTitleURL)+"' width='"+vidContainerWidth+"' height='"+vidContainerHeight+"' frameborder='0' scrolling='no'></iframe>")
        }
  })

  $j(".prVideoViewerContainerTall").each(function(){
    vidTitleURL = $j(this).attr("title");
    vidContainerWidth = "469";vidContainerHeight = "394";

    $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/photo-video/prVideoViewFrame_tall.htm?p="+encodeURIComponent(vidTitleURL)+"' width='"+vidContainerWidth+"' height='"+vidContainerHeight+"' frameborder='0' scrolling='no'></iframe>")
  })

  $j(".prVideoViewerContainerSpecial").each(function(){
    vidTitleURL = $j(this).attr("title");
    vidContainerWidth = "680";vidContainerHeight = "410";

    $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/photo-video/prVideoViewFrame_special.htm?p="+encodeURIComponent(vidTitleURL)+"' width='"+vidContainerWidth+"' height='"+vidContainerHeight+"' frameborder='0' scrolling='no'></iframe>")
  })

  $j(".prVideoPlayerKit").each(function(){
    vidTitleURL = $j(this).attr("title");
    vidContainerWidth = "690";vidContainerHeight = "530";

    $j(this).html("<iframe src='http://www.intel.com/newsroom/assets/2014/prVideoPlayerKit.htm?p="+encodeURIComponent(vidTitleURL)+"' width='"+vidContainerWidth+"' height='"+vidContainerHeight+"' frameborder='0' scrolling='no'></iframe>")
  })

}

//expanding text
if($j(".prExpandedText").length){
// Hide the extra content initially, using JS so that if JS is disabled, no problemo:
$j('.prExpandedTextHide').addClass('prHide')

// Set up a link to expand the hidden content:
$j(".prExpandedText p:first").append('<span class="prExpandedText_show"> more &darr;</span>');

// Set up a link to hide the expanded content.
$j(".prExpandedTextHide p:last").append('<span class="prExpandedText_hide"> less &uarr;</span>');

// Set up the toggle effect:
  $j(".prExpandedText_show").click(function(e){
  $j('.prExpandedTextHide').removeClass('prHide');
  $j(this).addClass('prHide');
  e.preventDefault();
});

  $j(".prExpandedText_hide").click(function(e){
  $j('.prExpandedTextHide').addClass('prHide');
  $j('.prExpandedText_show').removeClass('prHide');
  e.preventDefault();
});

}



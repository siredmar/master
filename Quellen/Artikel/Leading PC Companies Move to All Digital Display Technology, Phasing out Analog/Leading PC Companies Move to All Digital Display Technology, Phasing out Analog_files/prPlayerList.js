
//-------------------------------------------------
//		youtube playlist jquery plugin
//		Created by dan@geckonm.com
//		www.geckonewmedia.com
//
//		v1.1 - updated to allow fullscreen
//			 - thanks Ashraf for the request
//-------------------------------------------------

jQuery.fn.ytmultiplaylist = function(options) {

if($j(".playlistplayer").length){theVidSize = $j(".playlistplayer").attr("title").split("|")[1];}
var ytplSize = theVidSize;

if(theVidSize=="s"){
 var vidHeight = "235";
 var vidWidth = "293";
}
else{
  var vidHeight = "325";
  var vidWidth = "405";
}

  // default settings
  var options = jQuery.extend( {
    holderId: 'mainvideo',
	playerHeight: vidHeight,
	playerWidth: vidWidth,
	addThumbs: false,
	thumbSize: 'small',
	showInline: false,
	autoPlay: true,
	showRelated: true,
	allowFullScreen: false,
	wmode: true
  },options);

  return this.each(function() {
   		var selector = $j(this);
		var autoPlay = "";
		var showRelated = "&rel=0";
		var fullScreen = "";
		if(options.autoPlay) autoPlay = "&autoplay=1";
		if(options.showRelated) showRelated = "&rel=1";
		if(options.allowFullScreen) fullScreen = "&fs=1";
		if(options.wmode) wmode = "?wmode=transparent";

		//throw a youtube player in
		function play(id)
		{
		  //alert("then here?")
		   var html  = '';
           html += "<iframe height='"+options.playerHeight+"' src='http://www.youtube.com/embed/"+id+wmode+"' frameborder='0' width='"+options.playerWidth+"' id='apiplayer' name='apiplayer'></iframe>"
		   return html;

		};

		//grab a youtube id from a (clean, no querystring) url (thanks to http://jquery-howto.blogspot.com/2009/05/jyoutube-jquery-youtube-thumbnail.html)
		function youtubeid(url) {
	   //	 alert(url)
			var ytid = url.match("[\\?&]v=([^&#]*)");
			ytid = ytid[1];
			return ytid;
		};

		//load inital video
		var firstVid = selector.children("li:first-child").addClass("currentvideo").children("a").attr("href");
	   	$j("#"+options.holderId+"").html(play(youtubeid(firstVid)));

		//load video on request
		selector.children("li").children("a").click(function() {
            // alert("click me! + "+options.holderId)
			if(options.showInline) {
				$j("li.currentvideo").removeClass("currentvideo");
				$j(this).parent("li").addClass("currentvideo").html(play(youtubeid($j(this).attr("href"))));
			}
			else {
				$j("#"+options.holderId+"").html(play(youtubeid($j(this).attr("href"))));
				$j(this).parent().parent("ul").find("li.currentvideo").removeClass("currentvideo");
				$j(this).parent("li").addClass("currentvideo");
			}

			return false;
		});

		//do we want thumns with that?
		if(options.addThumbs) {

			selector.children().each(function(i){
			var vidID = youtubeid($j(this).children("a").attr("href"));
			var replacedText = $j(this).text();

				if(options.thumbSize == 'small') {
				  	var thumbUrl = "http://img.youtube.com/vi/"+vidID+"/2.jpg";
				}
				else {
				  	var thumbUrl = "http://img.youtube.com/vi/"+vidID+"/0.jpg";
				}
			   	$j(this).children("a").empty().html('<img class="hoverME" id="'+vidID+'" src="'+thumbUrl+'" alt="'+replacedText+'" width="92"/><span class="videothumbcaption">'+replacedText.substring(0,11)+'...</span>').attr("title", replacedText);

			});

		}
  });
};
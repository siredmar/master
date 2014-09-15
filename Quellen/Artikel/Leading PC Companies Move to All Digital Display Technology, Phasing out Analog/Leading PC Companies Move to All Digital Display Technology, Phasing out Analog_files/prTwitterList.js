var str = "";
if($j("#at_intelnews").length||$j("#at_intel").length){
	$j.getJSON("http://search.twitter.com/search.json?callback=?&rpp=5&q=from:intelnews",
	    function(json) {
			$j.each(json, function(i, tweets){
					if (tweets.length != undefined){
						if (tweets[0] != undefined){
							if (tweets[0].created_at != undefined){
				 				for(var i = 0; i < tweets.length; i++){
									$j("#at_intelnews").append(returnFormat(tweets[i].text,tweets[i].created_at,'intelnews'));
								}
							}
						}
					}
			});
	    }
	);
	$j.getJSON("http://search.twitter.com/search.json?callback=?&rpp=5&q=from:intel",
	    function(json) {
			$j.each(json, function(i, tweets){
				if (tweets.length != undefined){
					if (tweets[0] != undefined){
						if (tweets[0].created_at != undefined){
			 				for(var i = 0; i < tweets.length; i++){
								$j("#at_intel").append(returnFormat(tweets[i].text,tweets[i].created_at,'intel'));
							}
						}
					}
				}
			});
	    }
	);
}

var tabInfo = "";
var curTab  = "content1"; //default
var wURL = location.href; //get current url;
if($j("#prTabDynamic").length){
  tabInfo = $j("#prTabDynamic").attr("title").split("|");
  var tabNum = Number(tabInfo[0]); //num of tabs
  var tabTitle = tabInfo[1].split(","); //tab title
  var tabName  ="";
  var mainStr = "";
  for(i=1;i<tabNum;i++){
      tabName += "<div id='nav_content"+i
              + "' class='subnav' onclick='hideShow(\"tab_content"+i
              + "\",\"nav_content"+i+"\");'><a href='javascript:void(0);' style='text-decoration:none; cursor:pointer;'><div id='desc_content"+i
              + "' class='desc_on'>"+tabTitle[i-1]
              + "</div></a></div>";
  }
  //pass in tab default
  curTab = "content"+tabInfo[2];//"content"+i;
  mainStr = "<table cellspacing='0' cellpadding='0' border='0' width='100%'>"
            + "<tbody><tr><td><div id='nav' class='one-line' style='margin-left:0px;margin-top:0px;'>"+tabName
            + "</div></td></tr></tbody></table><br/>";

  $j("#prTabDynamic").html(mainStr); $j("#prTabDynamic").attr("title","");
}

function hideShow(id,navid) {
        var e = document.getElementById(id);
        var i;
        for (i=1;i<tabNum;i++)
            {
                $j("#tab_content"+i).hide();
                $j("#desc_content"+i).removeClass("desc_on").addClass("desc_off");
                $j("#nav_content"+i).removeClass('subnav').addClass("subnav_off");
            }

       if(e.style.display == 'none'){
               e.style.display = 'block';
  	            $j("#"+navid).removeClass("subnav_off").addClass("subnav");
                $j("#desc_"+navid.split("nav_")[1]).removeClass("desc_off").addClass("desc_on");
         }
      }
    //control tabs
    if($j("#prTabDynamic").length){hideShow("tab_"+curTab,"nav_"+curTab);}


       var prImgBase     = "http://www.intel.com/newsroom/assets/images/";
 //follow us section
  if($j("#prFollowUs").length){
    var prYouTube = "<a href='http://www.youtube.com/channelintel' target='_blank'>"
                    + "<img src='" + prImgBase + "nr-side-icon-youtube.png' style='margin-right:20px;' border='0'/></a>";
    var prTwitter = "<a href='http://www.twitter.com/intelnews' target='_blank'>"
                    + "<img src='" + prImgBase + "nr-side-icon-twitter.png' style='margin-right:20px;' border='0'/></a>";
    var prFacebook  = "<a href='http://www.facebook/intel' target='_blank'>"
                    + "<img src='" + prImgBase + "nr-side-icon-facebook.png' style='margin-right:20px;' border='0'/></a>";
    var prRSS     = "<a href='http://newsroom.intel.com/community/feeds/blogs?community=2016' target='_blank'>"
                    + "<img src='" + prImgBase + "nr-side-icon-rss.png' border='0'/></a>";

    $j("#prFollowUs").html('<div style="margin:5px 0px 5px 25px;">'+prYouTube+prTwitter+prFacebook+prRSS+'<br/><a href="http://newsroom.intel.com/docs/DOC-3311" target=_blank style="position:absolute;bottom:10px;left:50px">Get Intel News by Email</a></div>');

  }

  function getParameterByName( name )
{
  name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
  var regexS = "[\\?&]"+name+"=([^&#]*)";
  var regex = new RegExp( regexS );
  var results = regex.exec( window.location.href );
  if( results == null )
    return "";
  else
    return decodeURIComponent(results[1].replace(/\+/g, " "));
}

  $j(document).ready(function() {
    if($j("#jive-post-title").length){
        if(getParameterByName("isEventType").length > 0){
           if($j("#jive-post-title").length){
            $j("#subject01").val(getParameterByName("isEventType"));
           }
        }
    }

  })

   $j(window).load(function() {
    if(getParameterByName("isEventType").length > 0){
      tinyMCE.activeEditor.setContent("<p id='myEditorEvent'>start here</p>");

      var dom = tinymce.activeEditor.dom;
      var Event = tinymce.dom.Event;
      var pElements = dom.select('#myEditorEvent'); // Default scope is the editor document.

      //dom.setStyle(pElements, 'color', 'red');

      Event.add(pElements, 'click', function(e) {
         // alert('Do something with ' + e.target);
      });
    }
    })















var theVidID = "";
if($j(".playlistplayer").length){theVidID = $j(".playlistplayer").attr("title").split("|")[0];}
var ytfeed = "http://gdata.youtube.com/feeds/api/playlists/"+theVidID;
var ytplURL = "http://www.youtube.com/playlist?p=PL"+theVidID;

$j(document).ready(function () {
  if($j(".playlistplayer").length){
    if($j(".playlistplayer").attr("title").length){
    $j('.playlistthumbs').rssfeed(ytfeed, {limit: 24});
    $j('.playlistid').html("<a href="+ytplURL+" title='Current Playlist'>"+ytplURL+"</a>")
    setTimeout("showThumb()",1000);
    }
  }
});

function showThumb(){
  $j(".playlistthumbs").ytmultiplaylist({addThumbs:true, autoPlay: false, holderId: 'mainvideo'});
  setTimeout("showPagination()", 1000)
}

function ScrollToTop(){document.body.scrollTop = 0;}

var pager_self = ""
function showPagination(){
 // alert("here")
    pager_self = new Pager('playlistthumbs',8);
    pager_self.init();
    pager_self.showPageNav('pager_self');
    pager_self.showPage(1);
    $j(".playlistthumbs").show();
    $j(".loadingvid").hide();
    $j(".playlistplayer").show();
    $j(".playlistplayer_right").show();
    $j(".playlistplayer_left").show();


   // $j(".playlistplayer").attr("title","");


}

function youtubeid(url) {
	var ytid = url.match("[\\?&]v=([^&#]*)");
	ytid = ytid[1];
	return ytid;
};

function playFirstVideo(){
 $j("#apiplayer").attr("src","http://www.youtube.com/embed/"+youtubeid(topVideo));
}

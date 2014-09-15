
if($j("#pr2IDF").length || $j("#prContent").length){
  //setup each section iframe
  var offSections    = ["containerHighlights",
                       "containerBackstage"];
  var offAnchors     = ["a_hightlights:IDF Highlights",
                       "a_backstage:IDF Backstage"];
  var highAdjustment = ["iframeNewsReleases:155",
                        "iframePresentations:120",
                        "iframeFactsheets:120",
                        "iframeBlogs:115",
                        "iframeRelatedLinks:120",
                        "iframeTranscripts:120",
                        "iframeWebcasts:135",
                        "iframePhoto:420"];

  var a = document.getElementsByTagName( 'div' ), i = a.length;
  while( i-- ) {
    if( a[i].id && a[i].id.indexOf( 'iframe' )===0 ) {
      var iSrc = document.getElementById(a[i].id).title.split("|");
      var newHeight  = iSrc[2];
      //adjust height
      for(k=0;k<highAdjustment.length;k++){
         if(a[i].id == highAdjustment[k].split(":")[0]){
             newHeight = highAdjustment[k].split(":")[1];
         }
      }

      $j("#"+a[i].id).html("<iframe src='" + iSrc[0]
      + "' width='" + iSrc[1]
      + "' height='" + newHeight
      + "' class='" + iSrc[3]
      + "' marginheight='0' marginwidth='0' frameborder='" + iSrc[4]
      + "' scrolling='" + iSrc[5]
      + "' style='overflow:auto;'/>");

      $j("#"+a[i].id).show();$j("#"+a[i].id).attr("title","");//remove title
    }
    //turn-off sections
    for(j=0;j<offSections.length;j++){
       if(a[i].id == offSections[j]){
           $j("#"+a[i].id).hide();
           $j("#"+offAnchors[j].split(":")[0]).html("<span style='color:#e6e6e6'>"+offAnchors[j].split(":")[1]+"</span>");
       }
    }
    //temp remove highlight & backstage anchor
    $j("#"+offAnchors[0].split(":")[0]).hide();$j("#"+offAnchors[1].split(":")[0]).hide();
    //override press releases text
    if($j("#a_pressreleases").length){$j("#a_pressreleases").html('<a class="jive-link-anchor-small" href="#pressreleases">News Releases</a>');}
  }
}
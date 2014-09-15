/*
$(document).ready(function() {
  $.fn.simpleTabs = function(){
  	//Default Action
  	$(this).find(".tab_content").hide(); //Hide all content

  	//On Click Event
  	$("ul.tabs li").click(function() {
  		$(this).parent().parent().find("ul.tabs li").removeClass("tab_active"); //Remove any "tab_active" class
  		$(this).addClass("tab_active"); //Add "tab_active" class to selected tab
  		$(this).parent().parent().find(".tab_content").hide(); //Hide all tab content
          var tab_activeTab = $(this).attr("id"); //Find the rel attribute value to identify the tab_active tab + content
  		$("#"+tab_activeTab+"-content").show(); //Fade in the tab_active content
  		return false;
  	});

      //make me tab_active
      var is_tab_active = "";
      var is_id = "";
      if($(".simpleTabs").length){
       $("div.simpleTabs > ul > li").each(function(){
           is_tab_active = $(this).attr("class");
           is_id = $(this).attr("id");
           if(is_tab_active=="tab_active"){
             $(this).addClass("tab_active").show();
             $("#"+is_id+"-content").show();
           };
        })
       }
  };//end function
$("div[class^='simpleTabs']").simpleTabs(); //Run function on any div with class name of "Simple Tabs"
});
*/

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


$j(document).ready(function() {


$j('.activateNewTab').click(function(){
var paramTab = $j(this).attr("title");
getActive(paramTab);
return false;
})


function getActive(v){

$j("ul.tabs li").removeClass("tab_active"); //Remove any "tab_active" class
$j("#"+v).addClass("tab_active"); //Add "tab_active" class to selected tab
$j(".tab_content").hide(); //Hide all tab content
var tab_activeTab = $j("#"+v).attr("id"); //Find the rel attribute value to identify the tab_active tab + content
$j("#"+tab_activeTab+"-content").show(); //Fade in the tab_active content

//make me tab_active
  var is_tab_active = "";
  var is_id = "";
   $j("div.simpleTabs > ul > li").each(function(){
     if($j(this).attr("id")!=v){
         $j(this).removeClass("tab_active");
     }
       is_tab_active = $j("#"+v).attr("class");
       is_id = $j("#"+v).attr("id");
       if(is_tab_active=="tab_active"){

         $j("#"+v).addClass("tab_active").show();
         $j("#"+is_id+"-content").show();
       };
    })
                return false;
}


  $j.fn.simpleTabs = function(){

                //Default Action
    $j("div.tab_container > div").each(function(){
      $j("div.tab_container > div").hide();
    })
                //$j(this).find(".tab_content").hide(); //Hide all content

                //On Click Event
  	$j("div.simpleTabs > ul > li").click(function() {
  		$j(this).parent().find("li").removeClass("tab_active"); //Remove any "tab_active" class
  		$j(this).addClass("tab_active"); //Add "tab_active" class to selected tab
  	   	$j(this).parent().parent().find(".tab_content").hide(); //Hide all tab content
          var tab_activeTab = $j(this).attr("id"); //Find the rel attribute value to identify the tab_active tab + content

  		$j("#"+tab_activeTab+"-content").show(); //Fade in the tab_active content

          //var showThisContent =   $("#"+tab_activeTab+"-content > div > ul").child().find(".tab_active").attr("id");
          $j("#"+tab_activeTab+"-content > div > ul").find(".tab_active").each(function(){
            var nestedID = $(this).attr("id");
                $j("#"+nestedID+"-content").show()
          })
         // $("#"+showThisContent+"-content").show()

  		return false;
  	});
    //On Click Event Images
                $j(".prTabImgLink").click(function(){
      var paramImageTab = $j(this).attr("id");
                  $j("li#"+paramImageTab).parent().parent().find("ul.tabs li").removeClass("tab_active"); //Remove any "tab_active" class
                  $j("li#"+paramImageTab).addClass("tab_active"); //Add "tab_active" class to selected tab
                                $j("li#"+paramImageTab).parent().parent().find(".tab_content").hide(); //Hide all tab content
          var tab_activeTab = $j("li#"+paramImageTab).attr("id"); //Find the rel attribute value to identify the tab_active tab + content
                                $j("#"+tab_activeTab+"-content").show(); //Fade in the tab_active content
                                return false;
  });

                      //make me tab_active
      var is_tab_active = "";
      var is_id = "";
      if($j(".simpleTabs").length) {
       $j("div.simpleTabs > ul > li").each(function(){
           is_tab_active = $j(this).attr("class");
           is_id = $j(this).attr("id");
           if(is_tab_active=="tab_active"){
             $j(this).addClass("tab_active").show();
             $j("#"+is_id+"-content").show();
           };
        })
       }

  };//end function
$j("div[class^='simpleTabs']").simpleTabs(); //Run function on any div with class name of "Simple Tabs"

});

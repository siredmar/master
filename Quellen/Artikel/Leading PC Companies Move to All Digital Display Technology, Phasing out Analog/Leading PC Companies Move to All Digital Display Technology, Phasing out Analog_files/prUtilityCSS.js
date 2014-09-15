document.write('<link rel="stylesheet" href="http://www.intel.com/newsroom/assets/css/newsroom2.css" type="text/css"/>');
document.write('<link rel="stylesheet" href="http://www.intel.com/newsroom/assets/2014/css/nr-presskit.css" type="text/css"/>');
var prMarqueeImgBase = "http://www.intel.com/newsroom/";

if(typeof documentID != "undefined"){
    switch(documentID)
      {
        case 1406:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/atom2/css/superkit.css" type="text/css"/>');
        break;
        case 2321:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2011_fall/css/idf2011.css" type="text/css"/>');
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2011_fall/css/prTab.css" type="text/css"/>');
        break;
        case 2411:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/ultrabook/css/superkit.css" type="text/css"/>');
        break;
        case 2925:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2012_fall/css/idf2012.css" type="text/css"/>');
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2012_fall/css/prTab.css" type="text/css"/>');
        break;
        case 4181:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2013_fall/css/idf2013.css" type="text/css"/>');
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2013_fall/css/prTab.css" type="text/css"/>');
        break;
        case 4579:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/pcrefresh/holiday2013/css/holiday2013.css" type="text/css"/>');
        break;
        case 4943:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/bigdata/css/prKit.css" type="text/css"/>');
        break;
        case 5192:
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/education/css/prKit.css" type="text/css"/>');
        break;
        case 5224: //Intel and the Internet of Things
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/iot/css/prKit.css" type="text/css"/>');
        break;
        case 5256: //Intel Developer Forum - Shenzhen 2014
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2014_spring/css/idf.css" type="text/css"/>');
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2014_spring/css/prTab.css" type="text/css"/>');
        break;
        case 5285: //Intel Making it 'Clear'
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/intelclear/css/prKit.css" type="text/css"/>');
        break;
        case 5470: //Intel Mobile and Communications
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/atom/comms/css/prKit.css" type="text/css"/>');
        break;
        case 5628: //Software and Service for a Mobile World
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/atom/software/css/prKit.css" type="text/css"/>');
        break;
        case 5727: //Intel Developer Forum San Francisco 2014
        document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'kits/idf/2014_fall/css/prKit.css" type="text/css"/>');
        break;
        
        default:
        //document.write('<link rel="stylesheet" href="'+ prMarqueeImgBase +'assets/tab/prTab.css" type="text/css"/>');
      }
    }

//space marquee related code start here 11/15/2012 [SL]
$j(document).ready(function(){

   var url_space = location.pathname;

            //remove search bar and other header alignments
    if($j("meta[property='og:site_name']").attr("content") == "Intel Newsroom" || $j("meta[property='og:site_name']").attr("content") == "Contact Intel PR" || $j("meta[property='og:site_name']").attr("content") == "Intel Executive Biographies" || $j("meta[property='og:site_name']").attr("content") == "Analyst Zone" || $j("meta[property='og:site_name']").attr("content") == "GCG Hub" || url_space.indexOf('analyst_zone/fileshare') > -1 || url_space.indexOf('/groups/') > -1){

            var fileref=document.createElement("link");
            fileref.setAttribute("rel", "stylesheet");
            fileref.setAttribute("type", "text/css");
            fileref.setAttribute("href", "http://www.intel.com/newsroom/assets/css/newsroom_header.css");

            document.getElementsByTagName("head")[0].appendChild(fileref);

    }

       //is newsroom
     if($j("meta[property='og:site_name']").attr("content") == "Intel Newsroom") {
        //Replace text for homepage top menu section
        $j("#jive-place-link-overview-tab").html("<a class='' href='/community/intel_newsroom' id='jive-place-link-overview'>Home</a>");
        $j("#jive-place-link-overview-tab .j-tab-selected .active .j-ui-elem").html("<a class='j-ui-elem' href='/community/intel_newsroom' id='jive-place-link-overview'>Home</a>");

        $j("#jive-place-link-content-tab").html("<a id='jive-place-link-content' class='' href='/community/intel_newsroom/content'>Search News Content</a>");
        $j("#jive-place-link-content-tab .j-tab-selected .active .j-ui-elem").html("<a id='jive-place-link-content' class='j-ui-elem' href='/community/intel_newsroom/content'>Search News Content</a>");

        $j("#jive-place-link-people-tab").html("<a id='jive-place-link-people' class='' href='/community/intel_newsroom/bios'>Executive Bios</a>");
        $j("#jive-place-link-people-tab .j-tab-selected .active .j-ui-elem").html("<a id='jive-place-link-people' class='j-ui-elem' href='/community/intel_newsroom/bios'>Executive Bios</a>");

          //search content page
          if(url_space.indexOf('intel_newsroom/content') > 1 ){

            $j("#js-browse-controls span.js-shrinkable").text(function(index, text) {
                return text.replace("Documents", "Press Kits");
            });

            document.getElementsByName('query')[0].placeholder='Enter search keyword';

            if ($j.browser.msie && parseInt($j.browser.version, 10) < 10) {
                document.getElementsByName('query')[0].value='Enter search keyword';
                }
          if ($j('div.j-browse-filter-row ul.j-content-types li:contains("Executive Bios")').length === 0) {

            $j("div.j-browse-filter-row ul.j-content-types").find(" > li:nth-last-child(1)").before("<li><a class='jive-link-custom' title='Executive Bios' href='/community/intel_newsroom/bios'>Executive Bios</a></li><li><a class='jive-link-custom' title='Photography Search' href='http://newsroom.intel.com/docs/DOC-4631'>Photography Search</a></li>");

            $j("div.j-browse-filter-row ul.j-content-types li:contains('Polls')").remove();
          }

          }
        }


   //adding page marquee GCG Hub
   if(url_space.indexOf('/gcg_hub/') > -1
          || ($j("meta[property='og:site_name']").attr("content") == "GCG Hub"
            || $j("meta[property='og:site_name']").attr("content") == "F5 Refresh"
                || $j("meta[property='og:site_name']").attr("content") == "Media Clip Archive"
                    || $j("meta[property='og:site_name']").attr("content") == "First Time Buyer Collaboration Space"
                    	|| $j("meta[property='og:site_name']").attr("content") == "APAC PR Collaboration Space"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Southern Cone LAR Space"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "SC Speakers"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Templates"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Media Map"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Q&A"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Key Messages and Country Briefings"
 	                   	|| $j("meta[property='og:site_name']").attr("content") == "Dashboard"
		                    	|| $j("meta[property='og:site_name']").attr("content") == "Intel Campus Photo Archive"
	 		                       	|| $j("meta[property='og:site_name']").attr("content") == "Big Data Collaboration Space" )){

        //color text
        if($j(".font-color-meta-light").length){
          $j(".font-color-meta-light").css('color','rgb(5,112,184)');
          $j(".jive-username-link").css('color','rgb(5,112,184)');
        }
        //marquee link
        if($j("#j-header-wrap").length){
            $j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'gcg_hub/images/gcg-mrq-hub-bg.png)');
            $j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'gcg_hub/images/gcg-mrq-hub-txt.png" border="0"/>');
            $j("#j-header > a").attr("href", "/community/intel_newsroom/gcg_hub");
        }
    }

       //is bio
   if(url_space.indexOf('bios') > -1){
        //marquee
        if($j("#j-header-wrap").length){
            //$j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'assets/images/nrJ5-mrq-bios-bg.png)');
            //$j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'assets/images/nrJ5-mrq-bios-txt.png" border="0"/>');
            $j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'assets/images/nr-mrq-li-biographies-bg.jpg)');
        }
    }

         //is contact PR
   if(url_space.indexOf('contact_pr') > -1){
        //marquee
        if($j("#j-header-wrap").length){
           $j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'assets/images/nr-mrq-contact-press-relations.jpg)');
           //$j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'assets/images/nrJ5-mrq-contactPR-bg.png)');
           //$j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'assets/images/nrJ5-mrq-contactPR-txt.png" border="0"/>');
        }
    }

          //is analyst_zone
   if(url_space.indexOf('analyst_zone') > -1){
        //marquee
        if($j("#j-header-wrap").length){
            $j('#j-header-wrap').css('background', 'url('+ prMarqueeImgBase +'assets/images/nr-mrq-analyst-zone-bg.jpg)');

        }
    }

     //is APAC PR Collaboration Space
        if($j("meta[property='og:site_name']").attr("content") == "APAC PR Collaboration Space"){
          $j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'gcg_hub/images/gcg-mrq-hub-apc-pr-txt.png" border="0"/>');
            $j("#j-header > a").attr("href", "/community/intel_newsroom/gcg_hub/apac-pr");
             $j("a.logo img").css( "height", "120px" );
             $j("a.logo img").css( "margin-bottom", "-40px" );  

        }

        //is LAR PR Collaboration Space
        if($j("meta[property='og:site_name']").attr("content") == "LAR PR Collaboration Space"){
              $j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'gcg_hub/images/gcg-hub-mrq-LAR-PR-collab-txt.png" border="0"/>');
                $j("#j-header > a").attr("href", "/community/intel_newsroom/gcg_hub/lar-pr");
                $j("a.logo img").css( "height", "120px" );
                $j("a.logo img").css( "margin-bottom", "-40px" );


            }


        //is Southern Cone LAR Space and sub-spaces
        if(url_space.indexOf('/intel_newsroom/gcg_hub/lar-pr/sc') > -1){
            if($j("meta[property='og:site_name']").attr("content") == "Southern Cone LAR Space"
                || $j("meta[property='og:site_name']").attr("content") == "SC Speakers"
                || $j("meta[property='og:site_name']").attr("content") == "Templates"
                || $j("meta[property='og:site_name']").attr("content") == "Media Map"
                || $j("meta[property='og:site_name']").attr("content") == "Q&A"
                || $j("meta[property='og:site_name']").attr("content") == "Key Messages and Country Briefings"
                || $j("meta[property='og:site_name']").attr("content") == "Dashboard"){
              $j("#j-header > a").html('<img src="'+ prMarqueeImgBase +'gcg_hub/images/gcg-mrq-hub-south-cone-txt.png" border="0"/>');
                $j("#j-header > a").attr("href", "/community/intel_newsroom/gcg_hub/lar-pr/sc");
                $j("a.logo img").css( "height", "120px" );
                $j("a.logo img").css( "margin-bottom", "-40px" );


            }
        }

    });



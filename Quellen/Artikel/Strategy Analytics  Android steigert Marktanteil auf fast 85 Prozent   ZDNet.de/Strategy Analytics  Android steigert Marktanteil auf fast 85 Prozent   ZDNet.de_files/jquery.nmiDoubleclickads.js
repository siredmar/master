// $Id: jquery.nmiDoubleclickads.js 15791 2011-10-20 07:50:37Z tullmann $
/**
 * @file
 * jQuery Plug-in for DoubleClick Advertisements. 
 * 
 * Inserts DoubleClick Advertisements into the page.
 *
 * @access public
 * @version $Rev: 15791 $ 
 * @author Christopher Hopper <christopher.jf.hopper@gmail.com>
 * 
 */

(function(jQuery) {
  // Static DoubleClick properties and methods. 
  jQuery.nmiDoubleClickAds = {
    // Configure the DoubleClick ads for a site. 
    config : function (name, value) {
      var options = name;
      
      // Deal with individual settings values. 
      if ( name.constructor == String) {
        options = { };
        options[name] = value;
      }
      
      // option is an object with name value pairs. 
      for (name in options) {
        // If the property exists and we have a value, then set it. 
        if (this[name] && options[name]) this[name] = options[name];
      }
      
      // Allow chaining. 
      return this;
    },
    
    
    data : {  },
    
    waitForIframe : function (iframeId, count) {
      var iframe = document.getElementById(iframeId);
      var actualHeight = iframe.contentWindow.document.height 
        || iframe.contentWindow.window.document.body && iframe.contentWindow.window.document.body.scrollHeight
        || -1;
      if (iframe.height != actualHeight || count++ < 1000) {
        if (actualHeight > 0) iframe.height = actualHeight;
        setTimeout("jQuery.nmiDoubleclickad.waitForIframe('"+iframeId+"', "+ count +");", 250);
      }
    }
  };

	// jQuery appendDoubleClickAd method. 
	jQuery.fn.appendNMIDoubleClickAd = function(data, settings) {
		// Default attributes for <IFRAME/> Ad container
		var iframeAttr = { 'frameBorder': 0, 'scrolling': "no", 'vspace': 0
		, 'marginWidth': 0, 'marginHeight': 0, 'hspace': 0
		, 'allowTransparency': true, 'width': (settings.width ? 
		settings.width : "100%"), 'height': (settings.height ? 
		settings.height : "1") },
		// default configuration options. 
		config = { 'iframeCSS' : false }; 

		// Extend the default configuration with any given settings. 
		if (settings){
			jQuery.extend(config, settings);
		}
    	
    	
    	
    	
	    this.each(function(){

			// Set the IFRAME "id" and "name" for each advertisement. 
			iframeAttr.id = iframeAttr.name = "ads-iframe-"+ jQuery.nmiDoubleClickAds._tile;
	  
			// Create the IFRAME element. 
			jQuery('<IFRAME/>').attr(iframeAttr).appendTo(this).each(function(){
				// Write the DoubleClick Advertisement into the IFRAME. 
	
				// Get a pointer to the IFRAME document object and DOM. 
				var doc = this.contentWindow.document || this.contentWindow.window.document || this.contentDocument;
	  
				// Open the IFRAME document. 
				if (!jQuery.browser.msie && doc.open) doc.open();
	  
				// Ensure anchors do not open in the IFRAME by default. 
				doc.write('<BASE target="_top" />');
	  
				// If a specific advert CSS file is defined, use it. 
				if (settings.iframeCSS){ 
					doc.write('<STYLE type="text/css" media="all">@import "'+ settings.iframeCSS +'";</STYLE>');
				}
				
				if(typeof(data.categories) != "object")
					data.categories = [];
				
				if(typeof(data.keywords) != "object")
					data.keywords = [];
				
				if(typeof(data.id) != "string")
					data.id = "0";
				
				// Add the advertisement from DoubleClick. 
				doc.write('<SCR'+'IPT type="text/javascript" src="http://www.googletagservices.com/tag/js/gpt.js"></SCR'+'IPT>');
				doc.writeln('<SCR'+'IPT type="text/javascript">');
				
				switch(config.type){
					case 'leaderboard':
						doc.writeln("nmi_ads_leaderboard_top = googletag.defineSlot('/16255858/zdnet/detail/leaderboard_top', [728, 90], 'div-gpt-ad-1342091490696-4').addService(googletag.pubads());");
						break;
					case 'billboard':
						doc.writeln("nmi_ads_billboard_top = googletag.defineSlot('/16255858/zdnet/detail/billboard', [728, 90], 'div-gpt-ad-billboard-1').addService(googletag.pubads());");
						break;
					case 'gallerympu':
						doc.writeln("nmi_ads_mpu_top = googletag.defineSlot('/16255858/zdnet/detail/mpu_top', [[300, 250], [300, 600], [336, 90], [336, 280]], 'div-gpt-ad-1342091490696-6').addService(googletag.pubads());");	
						break;
				}
				doc.writeln('googletag.pubads().setTargeting("cat", ["default", "in_gallery", "' + data.categories.join('", "') + '"]);');
				doc.writeln('googletag.pubads().setTargeting("keyword", ["default", "in_gallery", "' + data.keywords.join('", "') + '"]);');
				doc.writeln('googletag.pubads().setTargeting("id", "'+ data.id +'");');
				doc.writeln('googletag.pubads().enableSyncRendering();');
				doc.writeln('googletag.pubads().enableSingleRequest();');
				doc.writeln('googletag.enableServices();');
				doc.writeln('</SCR'+'IPT>');
	  			doc.writeln('<BODY>');
	  			
	  			switch(config.type){
		    		case 'billboard':
		      			doc.writeln("<div id='div-gpt-ad-billboard-1'>");
						doc.writeln("<script type='text/javascript'>");
						doc.writeln("googletag.display('div-gpt-ad-billboard-1');");
						doc.writeln("</script>");
						doc.writeln("</div>");
		      			break;
		    		case 'leaderboard':
		      			doc.writeln("<div id='div-gpt-ad-1342091490696-4'>");
						doc.writeln("<script type='text/javascript'>");
						doc.writeln("googletag.display('div-gpt-ad-1342091490696-4');");
						doc.writeln("</script>");
						doc.writeln("</div>");
		      			break;
		      		case 'gallerympu':
		      			doc.writeln("<div id='div-gpt-ad-1342091490696-6'>");
						doc.writeln("<script type='text/javascript'>");
						doc.writeln("googletag.display('div-gpt-ad-1342091490696-6');");
						doc.writeln("</script>");
						doc.writeln("</div>");
		      			break;
		      		default:
		      			return this;
		      			break;
				}
				
	  			doc.writeln('</BODY>');
				// Close the IFRAME document. 
				if (!jQuery.browser.msie && doc.close) doc.close();
			}).load(function(){
				if (!settings.height) this.height = this.contentWindow.document.height || this.contentWindow.window.document.body && this.contentWindow.window.document.body.scrollHeight;
			});
	
			// If MSIE, Resize the IFRAME using a looping setTimeout call.
			if (jQuery.browser.msie && !settings.height){
				jQuery.nmiDoubleClickAds.waitForIframe(iframeAttr.id, 1);
			}
		});
		// Support jQuery chaining.
		return this;
	};
})(jQuery);

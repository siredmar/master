
/**
 * jQuery nmiGallery
 * Adds on page navigation for the gallery without reloading the site every time  
 *
 * v0.0.1 - 12.05.2011
 *
 * Copyright (c) 2012 Torsten Ullmann (Torsten.Ullmann@netmediainteractive.com)
 *
 * Plugin is looking for class="ajaxGallery" by default within the Element(s) given 
 * 
 * @params object options
 * 
 * Available Options
 * 
 * gallery_object: JS Object with the gallery Data
 * 
 * transition : Type of Transition (currently supported 'blend')
 * transition_time : Time the transition takes (default 1500)
 * 
 * selector_nav_next: Selector for the Next Image buttons
 * selector_nav_prev: Selector for the Prev Image buttons
 * 
 * sleep : Time till the next transition takes place on autoplay only
 * autostart : autoplay the galleries
 * stop_on_mouseover: stops the autoplay on mouseover
 * stop_on_browser_blur: stops the autoplay on browser blur
 **/
(function($){
	
    var options = {
    	'gallery_object' : null,
    	'rotate_ads' : false,
		'transition' : 'blend',
		'transition_time' : 500,
		'sleep' : 10000,
		'selector_nav_next' : '.ajaxNavNext',
		'selector_nav_prev' : '.ajaxNavBack',
		'selector_num_element': '.ajaxCurrentImageNum',
		'selector_image_element': '.ajaxSemiImage',
		'selector_caption_element': '.ajaxCaption', 
		'selector_loading_indicator': '.ajaxLoader',
		
		'selector_back_to_article': '.ajaxBackToArticle',
		
		'firstAd': 5,
		'nextAd': 8, 
		
		'selector_slider': '.ajaxSlider',
		'selector_slider_nav_next': '.ajaxSliderNavNext',
		'selector_slider_nav_prev': '.ajaxSliderNavPrev',
		'selector_zoom': '.ajaxZoom',
		'slider_size_per_image': 90,
		
		'initialZoomWidth': 600,
		'initialZoomHeight': 450,
		
		'autostart' : false,
		'running': false,
		'stop_on_mouseover': true,
		'stop_on_browser_blur': true, 
		
		'gallery' : 'nmiGallery',
		
		'previous': "previous",
		'next': "next",
		'close': "close",
		
		// do not edit below here !
		'navElementsNext' : [],
		'navElementsPrev' : [],
		'imageElement': [],
		'captionElement': [],
		'numElement': [],
		'sid': 0,
		'waitingFor': null,
		'clickCounter': 0
		
	};
	var images = null;
	var isIE = $.browser.msie && !$.support.opacity;
	var isIE6 = isIE && $.browser.version < 7;
	
	var nme_adv_dfp_path = "/gallery/kategorie/galerien" + window.location.pathname; 
	
	var publicMethods = {
		// Init the gallerys on the page	
		init: function(newOptions){ 
			this.each(function(){
				
				var currentGallery = this;
				
				options = $.extend({}, options, newOptions);
				
				if(options.gallery_object === null || options.gallery_object === undefined){
					return this;
				}
				
				if($.url().fparam('sid') !== undefined){
					var storyID = $.url().fparam('sid');
					if(!isNaN(parseInt(storyID, 10))){
						if(parseInt(storyID, 10) > 0){
							$(options.selector_back_to_article).css('display', '').find('a').first().attr('href', '/' + parseInt(storyID, 10) + '/');
							options.sid = parseInt(storyID, 10);
						}
					}
				}
				
				var colorBoxElements = $('<div/>', {'style': 'display: none'});
				
				$.each(options.gallery_object['images'], function(num, currentImageData){
					try{
						if(currentImageData['IMAGE'] !== null && currentImageData['IMAGE'] !== undefined){
							var currentImage = $('<a/>', {
									'href': currentImageData['IMAGE'],
									'rel': 'galleryImages', 
									'title': currentImageData['CAPTION']
							});
							colorBoxElements.append(currentImage);	
						}
					}
					catch(e){
						// console.log(e);
					}
				});
				$('body').append(colorBoxElements);
				
				
				var height = $(window).height()-20;
				var width = $(window).width()-20;
				
				var zdnetLogo = $('<img/>', {
					'src': '/wp-content/themes/zdnet-de/images/colorbox/zdnet-logo.png',
					'alt': 'ZDNet.de',
					'style': 'margin: 10px 5px;'
				});
				
				var adContainer = $('<div/>', {
							'id': 'cboxAdContainer',
							'style' : 'position: relative; padding: 0 0 0 0; left: 50%; margin: 0px -364px; top: -40px; width: 728px; height: 90px; background: url(/wp-content/themes/zdnet-de/images/anzeige.png) 50% 5px no-repeat;'
							});
				
				options.imageColorBoxElements = colorBoxElements.find('a').colorbox({
									'scalePhotos': true,
									'scrolling': true,
									'photo': true,
									'current': 'Bild {current} von {total}',
									'previous': 'zurück',
									'next': 'weiter',
									'close': 'schließen',
									'width': '90%',
									'height': '90%',
									'minWidth': 800,
									'initialWidth': 800,
									'initialHeight': 400,
									onOpen: function(){
										$('#cboxTopLeft, #cboxTopCenter, #cboxTopRight, #cboxBottomLeft, #cboxBottomCenter, #cboxBottomRight, #cboxClose').addClass('gallery');
									},
									onClosed: function(){
										$('#cboxTopLeft, #cboxTopCenter, #cboxTopRight, #cboxBottomLeft, #cboxBottomCenter, #cboxBottomRight, #cboxClose').removeClass('gallery');
										publicMethods.zoomImageClose.apply(currentGallery);     			
									},
									onLoad: function(){
										var currentAdContainer = adContainer.clone();
										$('#cboxTopCenter').empty();
										$('#cboxTopCenter').append(zdnetLogo.clone());
										if(nme_adv_dfp_path != ''){
											$('#cboxTopCenter').append(currentAdContainer);
											currentAdContainer.appendNMIDoubleClickAd({'id': nme_adv_dfp_id, 'keywords': nme_adv_dfp_keywords, 'categories': nme_adv_dfp_categories},{'type': 'billboard', 'width': 728, 'height': 90});
										}
										publicMethods.doTracking.apply(currentGallery);
									}
								});
				
				
				// Set Nav Elements to listen on clicks
				options.navElementsNext = $(this).find(options.selector_nav_next);
				options.navElementsPrev = $(this).find(options.selector_nav_prev);
				
				options.navElementsZoom = $(this).find(options.selector_zoom);
				
				options.preventNonZoomClicks = false;
				
				options.navElementsNext.click(function(event){publicMethods.nextImage.apply(currentGallery, arguments);});
				options.navElementsPrev.click(function(event){publicMethods.prevImage.apply(currentGallery, arguments);});
				
				options.navElementsSliderNext = $(this).find(options.selector_slider_nav_next);
				options.navElementsSliderPrev = $(this).find(options.selector_slider_nav_prev);
				
				options.navElementsSliderNext.click(function(event){publicMethods.sliderNext.apply(currentGallery, arguments);});
				options.navElementsSliderPrev.click(function(event){publicMethods.sliderPrev.apply(currentGallery, arguments);});
				
				options.navElementsZoom.click(function(event){publicMethods.zoomImage.apply(currentGallery, arguments);});
				
				// Find the base elements once
				options.imageElement = $(this).find(options.selector_image_element).first();
				options.captionElement = $(this).find(options.selector_caption_element).first();
				options.numElement = $(this).find(options.selector_num_element).first();
				options.loaderElement = $(this).find(options.selector_loading_indicator).first();
				
				options.slider = $(this).find(options.selector_slider).first();
				
				options.sliderItems = $(this).find(options.selector_slider).find('li');
				
				options.slider_size_per_image = $(this).find(options.selector_slider).find('li').first().outerWidth(true);
				options.sliderMax = (parseInt(options.slider_size_per_image, 10) * ( parseInt(options.gallery_object['images'].length, 10) - 7 ) * -1);
				options.sliderMin = 0;
				
				// Try to find the current page number
				try{
					options.currentActiveNumber = parseInt(options.numElement.text(), 10);
					if(options.gallery_object.length < options.currentActiveNumber){
						options.currentActiveNumber = 1;
					}
					else if(options.currentActiveNumber < 1){
						options.currentActiveNumber = 1;
					}
				}
				catch(e){
					options.currentActiveNumber = 1;
				}
				
				options.imageSize = 'SEMI';
				options.firstrun = true;
				options.firstrunAds = true;
				$.data(this, options.gallery, $.extend({}, $.data(this, options.gallery) || options) );
				
				
				
				$(this).find(options.selector_slider).find('li.new').find('img').each(function(){
					$(this).removeClass('new');
					$(this).attr('src', $(this).attr('rel'));
					$(this).attr('rel', 'galleryImages');
				});
				
				
				// Bind the cursor keys
				$(document).bind('keydown.gallery', function(event){
					var key = event.keyCode;
					if (key === 37) {
						event.preventDefault();
						publicMethods.prevImage.apply(currentGallery, [event]);
					} 
					else if (key === 39) {
						event.preventDefault();
						publicMethods.nextImage.apply(currentGallery, [event]);
					}
				});
				
				var counter = 1;
				options.sliderItems.each(function(){
					var currentCounter = counter;
					$(this).click(function(event){publicMethods.prepare.apply(currentGallery, [event, currentCounter]);});
					counter++;
				});
				
				
				
				if($.url().fparam('image') !== undefined){
					publicMethods.prepare.apply(this, [false, $.url().fparam('image')]);
				}
				else
				{
					if(options.autostart === true){
						return publicMethods.run.apply(this);		
					}
					$.data(this, options.gallery).firstrun = false;
					publicMethods.prepare.apply(this);
				}
			});
			
			if( window.isActive === undefined && (options.stop_on_browser_blur === true) ){
				window.isActive = true;
    			$(window).focus(function(){
					this.isActive = true; 
				});
	    		$(window).blur(function(){
	    			this.isActive = false;
	    		});
			}
			return this;
		},
		
		
		showOverlayAd: function(){
			var currentGallery = this;
			/*
			$.data(this, options.gallery).preventNonZoomClicks = true;
			$.data(this, options.gallery).navElementsPrev.find('a').first().css('opacity', 0);
			$.data(this, options.gallery).navElementsNext.find('a').first().css('opacity', 0);
			*/
			$('#ad_msgplus-gallery-1').remove();
			var offset = $(this).find('div.galleryImage').position();
			
			var adContainer = $('<div/>', {
					'id': 'ad_msgplus-gallery-1',
					'style' : 'position: absolute; top:' + offset.top + 'px; left:' + offset.left + 'px; background:#FFFFFF; width: 590px; height: 570px; margin-left:15px; z-index: 500;'
					});
			
			var closeButton = $('<div/>', {
					'style': 'float:right; margin-right:20px; cursor:pointer', 
					'html': '<a><b>zum Bild</b></a>'
					});
			
			var innerAdContainer = $('<div/>', {
					'style': 'margin:auto; background: url("/wp-content/themes/zdnet-de/images/anzeige.png") no-repeat scroll 50% 0 transparent; padding-top: 10px;width: 336px; height: 300px; margin-top: 120px;'
					});
			
			adContainer.append(closeButton, innerAdContainer);
			closeButton.click(function(event){publicMethods.closeOverlayAd.apply(currentGallery, arguments);});
			
			$(this).append(adContainer);
			
			innerAdContainer.appendNMIDoubleClickAd({'id': nme_adv_dfp_id, 'keywords': nme_adv_dfp_keywords, 'categories': nme_adv_dfp_categories},{'type': 'gallerympu', 'width': 336, 'height': 280});
		},
		
		closeOverlayAd: function(){
			$('#ad_msgplus-gallery-1').remove();
			/*
			$.data(this, options.gallery).preventNonZoomClicks = false;
			$.data(this, options.gallery).navElementsPrev.find('a').first().css('opacity', 1);
			$.data(this, options.gallery).navElementsNext.find('a').first().css('opacity', 1);
			*/
		},
		
		zoomImage: function(event){
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				publicMethods.stop.apply(this);
			}
			$.data(this, options.gallery).preventNonZoomClicks = true;
			
			$.data(this, options.gallery).imageColorBoxElements.eq($.data(this, options.gallery).currentActiveNumber-1).click();
		},
		
		zoomImageClose: function(event){
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				publicMethods.stop.apply(this);
			}
			$.data(this, options.gallery).preventNonZoomClicks = false;
			
			/*
			$.data(this, options.gallery).imageColorBoxElements.eq($.data(this, options.gallery).currentActiveNumber).click();
			*/
		},
		
		prepare: function(event, number){
			var currentGallery = this;
				
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				if($.data(this, options.gallery).preventNonZoomClicks === true){
					return false;
				}
				publicMethods.stop.apply(this);
			}
			
			
			
			var preloadImages = [];
			
			if($.data(this, options.gallery).gallery_object.length < $.data(this, options.gallery).currentActiveNumber){
				$.data(this, options.gallery).currentActiveNumber = $.data(this, options.gallery).gallery_object.length-1;
			}
			else if($.data(this, options.gallery).currentActiveNumber < 1){
				$.data(this, options.gallery).currentActiveNumber = 1;
			}
			
			// Set the caption and number
			var currImage = $.data(this, options.gallery).gallery_object.images[$.data(this, options.gallery).currentActiveNumber];
			
			$.data(this, options.gallery).captionElement.html(currImage.CAPTION);
			
			var strippedCaption = $.strip_tags(currImage.CAPTION);
			
			$.data(this, options.gallery).imageElement.attr('rel', strippedCaption);
			$.data(this, options.gallery).imageElement.attr('title', strippedCaption);
			
			$.data(this, options.gallery).numElement.text($.data(this, options.gallery).currentActiveNumber);
			
			$.data(this, options.gallery).sliderItems.removeClass('active');
			$($.data(this, options.gallery).sliderItems[$.data(this, options.gallery).currentActiveNumber-1]).addClass('active');
			
			if(number !== undefined && number > 0 && number < $.data(this, options.gallery).gallery_object.images.length){
				$.data(this, options.gallery).currentActiveNumber = number-1;
				$.data(this, options.gallery).waitingFor = 'next';
			}
			else{
				// Set the new URL so we are able to reload the last image if url copy pasted / reloaded etc
				if($.url().attr('query') != ''){
					var newURL = $.url().attr('base')+$.url().attr('path')+'?'+$.url().attr('query')+'#';	
				}
				else{
					var newURL = $.url().attr('base')+$.url().attr('path')+'#';
				}
				
				if($.data(this, options.gallery).sid > 0){
					newURL = newURL + 'sid=' + $.data(this, options.gallery).sid + '&';
				}
				newURL = newURL + 'image='+$.data(this, options.gallery).currentActiveNumber;
				window.location.href = newURL;
				
				var sliderOffset = ($.data(this, options.gallery).currentActiveNumber - 2) * $.data(this, options.gallery).slider_size_per_image * -1;
				if(sliderOffset > 0){
					sliderOffset = 0;
				}
				else if(sliderOffset < $.data(this, options.gallery).sliderMax){
					sliderOffset = $.data(this, options.gallery).sliderMax
				}
				$.data(this, options.gallery).slider.animate({'left' : sliderOffset}, 500);
			}
			
			var prevImage = null;
			if($.data(this, options.gallery).currentActiveNumber > 1){
				prevImage = $.data(this, options.gallery).gallery_object.images[( $.data(this, options.gallery).currentActiveNumber-1 )];
				$.data(this, options.gallery).navElementsPrev.find('a').removeClass('off');
				if(prevImage !== undefined){
					preloadImages.push(prevImage[$.data(this, options.gallery).imageSize]);
				}
			}
			else{
				$.data(this, options.gallery).navElementsPrev.find('a').addClass('off');
			}			
			
			var nextImage = null;
			if($.data(this, options.gallery).currentActiveNumber < $.data(this, options.gallery).gallery_object.total ){
				nextImage = $.data(this, options.gallery).gallery_object.images[( $.data(this, options.gallery).currentActiveNumber+1 )];
				$.data(this, options.gallery).navElementsNext.find('a').removeClass('off');
				if(nextImage !== undefined){
					preloadImages.push(nextImage[$.data(this, options.gallery).imageSize]);	
				}
			}
			else{
				$.data(this, options.gallery).navElementsNext.find('a').addClass('off');
			}
			
			// Preload the next and previous big images .. 
			$.data(this, options.gallery).preloading = true;
			$.data(this, options.gallery).nextImage = nextImage;
			$.data(this, options.gallery).prevImage = prevImage;
			$.preLoadImages(preloadImages, { 
							onSuccess: function(){
								publicMethods.preloadDone.apply(currentGallery);
							},
							onError: function(){
								publicMethods.preloadDone.apply(currentGallery);
							}
						}
			);
			
			
		},
		
		preloadDone: function(){
			var currentGallery = this;
			$.data(this, options.gallery).preloading = false;
			if($.data(this, options.gallery).waitingFor === null){
				return true;
			}
			
			switch($.data(this, options.gallery).waitingFor){
				case 'next':
					$.data(this, options.gallery).waitingFor = null;
					return publicMethods.nextImage.apply(this);
					break;
				case 'prev':
					$.data(this, options.gallery).waitingFor = null;
					return publicMethods.nextImage.apply(this);
					break;
				default:
					$.data(this, options.gallery).waitingFor = null;
					return true;
			}
			return true;
		},
		
		
		nextImage: function(event){
			var currentGallery = this;
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				
				if($.data(this, options.gallery).preventNonZoomClicks === true){
					return false;
				}
				publicMethods.stop.apply(this);
			}
			
			if($.data(this, options.gallery).navElementsNext.find('a').hasClass('off')){
				return false;
			}
			
			
			if($.data(this, options.gallery).animating === true){
				return true;
			}
			
			
			if($.data(this, options.gallery).preloading === true){
				$.data(this, options.gallery).loaderElement.css('display', '');
				$.data(this, options.gallery).waitingFor = 'next';
			}
			else{
				$.data(this, options.gallery).animating = true;
				$.data(this, options.gallery).loaderElement.css('display', 'none');
				$.data(this, options.gallery).imageElement.parent('div').css('background', '#ffffff url('+$.data(this, options.gallery).imageElement.attr('src')+') center center no-repeat');
				$.data(this, options.gallery).imageElement.css('opacity', 0).attr('src', $.data(this, options.gallery).nextImage[$.data(this, options.gallery).imageSize]);
				
				// Do Ad Stuff
				$.data(currentGallery, options.gallery).clickCounter++;
				if($.data(currentGallery, options.gallery).clickCounter % $.data(currentGallery, options.gallery).nextAd == $.data(currentGallery, options.gallery).firstAd){
					publicMethods.showOverlayAd.apply(currentGallery);
				}

				$.data(this, options.gallery).imageElement.animate({'opacity': 1}, 500, 
																		function(){
																		$.data(currentGallery, options.gallery).currentActiveNumber++;
																		$.data(currentGallery, options.gallery).animating = false;
																		publicMethods.doTracking.apply(currentGallery);
																		publicMethods.changeAds.apply(currentGallery);
																		publicMethods.prepare.apply(currentGallery);
																	});
			}
		},
		
		doTracking: function(){
			if($.data(this, options.gallery).firstrun === true)
			{
				$.data(this, options.gallery).firstrun = false;
			}
			else
			{
				/*
				// IVW Call
				$('#ivw_tracking_image').remove();
				var IVW="http://zdnet.ivwbox.de/cgi-bin/ivw/CP"+ nme_adv_dfp_path +";/" + Math.random() + "/" + $.url().segment().join('/');
				var ivwImage = $('<img/>',{
					'src': IVW,
					'id': 'ivw_tracking_image',
					'width': 1,
					'height': 1
				});
				$(this).append(ivwImage);
				*/
				
                // trigger Google Universal Analytics
                if ((typeof ga !== "undefined") && (ga !== null)) {
                    ga('send', 'pageview');
            
                    if ((typeof nme_ga_events !== "undefined") && (nme_ga_events !== null)) {
                        for (i = 0; i < nme_ga_events.length; i++) {
                            ga('send', 'event', nme_ga_events[i].name, nme_ga_events[i].value);
                        }        
                    }
                }
    		}
		},
		
		changeAds: function(){
			if($.data(this, options.gallery).firstrunAds === true)
			{
				$.data(this, options.gallery).firstrunAds = false;
			}
			else
			{
				if( typeof($.data(this, options.gallery).rotate_ads) == 'object' &&  $.data(this, options.gallery).rotate_ads.length > 0 && typeof(googletag) == 'object'){
					googletag.pubads().refresh($.data(this, options.gallery).rotate_ads);
				}
			}
		},
		
		prevImage: function(event){
			var currentGallery = this;
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();

				if($.data(this, options.gallery).preventNonZoomClicks === true){
					return true;
				}				

				publicMethods.stop.apply(this);
			}
			
			if($.data(this, options.gallery).navElementsPrev.find('a').hasClass('off')){
				return false;
			}
			
			if($.data(this, options.gallery).animating === true){
				return true;
			}
			
			if($.data(this, options.gallery).preloading === true){
				$.data(this, options.gallery).loaderElement.css('display', '');
				$.data(this, options.gallery).waitingFor = 'prev';
			}
			else{
				$.data(this, options.gallery).animating = true;
				$.data(this, options.gallery).loaderElement.css('display', 'none');
				$.data(this, options.gallery).imageElement.parent('div').css('background', '#ffffff url('+$.data(this, options.gallery).imageElement.attr('src')+') center center no-repeat');
				$.data(this, options.gallery).imageElement.css('opacity', 0).attr('src', $.data(this, options.gallery).prevImage[$.data(this, options.gallery).imageSize]);
				
				// Do Ad Stuff
				$.data(currentGallery, options.gallery).clickCounter++;
				if($.data(currentGallery, options.gallery).clickCounter % $.data(currentGallery, options.gallery).nextAd == $.data(currentGallery, options.gallery).firstAd){
					publicMethods.showOverlayAd.apply(currentGallery);
				}
				
				$.data(this, options.gallery).imageElement.animate({'opacity': 1}, $.data(this, options.gallery).transition_time, 
																		function(){
																		$.data(currentGallery, options.gallery).currentActiveNumber--;
																		$.data(currentGallery, options.gallery).animating = false;	
																		publicMethods.doTracking.apply(currentGallery);
																		publicMethods.changeAds.apply(currentGallery);
																		publicMethods.prepare.apply(currentGallery);
																	});
			}
		},
		
		
		sliderNext: function(event){
			var currentGallery = this;
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				publicMethods.stop.apply(this);
			}
			
			var currentPosition = $.data(this, options.gallery).slider.css('left');
			var nextPosition = parseInt(currentPosition, 10) + (parseInt(options.slider_size_per_image, 10) * -6);
			
			if(nextPosition < $.data(this, options.gallery).sliderMax){
				nextPosition = $.data(this, options.gallery).sliderMax;
			}
			
			$.data(this, options.gallery).slider.animate({'left': nextPosition}, 500);
			
		},
		
		sliderPrev: function(event){
			var currentGallery = this;
			if(typeof event == 'object'){
				event.preventDefault();
				event.stopPropagation();
				publicMethods.stop.apply(this);
			}
			
			var currentPosition = $.data(this, options.gallery).slider.css('left');
			var nextPosition = parseInt(currentPosition, 10) - (parseInt(options.slider_size_per_image, 10) * -6);
			
			if(nextPosition > $.data(this, options.gallery).sliderMin){
				nextPosition = $.data(this, options.gallery).sliderMin;
			}
			
			$.data(this, options.gallery).slider.animate({'left': nextPosition}, 500);
		},
		
		// Run the gallerys 
		run: function(){
			var currentGalery = this;
			$.data(this, options.gallery).running = true;
			$.data(this, options.gallery).timeOut = window.setInterval(
											function(){
												publicMethods.nextImage.apply(currentGalery);
											}, $.data(currentGalery, options.gallery).sleep); 
		},
		
		stop: function(){
			$.data(this, options.gallery).running = false;
			window.clearInterval($.data(this, options.gallery).timeOut);
		}
	}
	
	$.fn.nmiGallery = function(method){
		var currentObjects = $(this);
		
		// No animation for IE6 
		if(isIE6){
			return this;
		}
		
		// Method calling logic
		if ( publicMethods[method] ) {
			return publicMethods[method].apply( this, Array.prototype.slice.call( arguments, 1 ));
		} else if ( typeof method === 'object' || ! method ) {
			return publicMethods.init.apply( this, arguments );
		} else {
			$.error( 'Method ' +  method + ' does not exist on jQuery.nmiGallery' );
		}
	}
	
})(jQuery);
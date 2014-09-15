var zdnet = {
	loaded: function(){
		 // Nothin for post load currently
       
	},
	domready: function(){
		// Search forms cleanup
		jQuery('input.clear').each(function(){
			this.defaultValue = jQuery(this).attr('value');
		});
		jQuery('input.clear').focus(function(){
			if(this.value === this.defaultValue){  
				this.value = '';  
			}
		});
		jQuery('input.clear').blur(function(){
			if(jQuery.trim(this.value) === ''){
				this.value = (this.defaultValue ? this.defaultValue : '');
			}
		});
		jQuery('form').submit(function(){
			jQuery(this).find('input.clear').each(function(){
				if(this.value === this.defaultValue){  
					this.value = '';  
				}
			});
		});

		
		// Setup edition
		jQuery('#edition li').hoverIntent(
			function(event){
				jQuery(this).find('.countries').css('display', 'block');
			},
			function(event){
				jQuery(this).find('.countries').css('display', '');
			}
		);
		
		// Fix Navigation mouse over
		jQuery('#navigation .dropdown').css('left', '-999em');
		jQuery('#navigation li').hoverIntent(
			function(event){
				jQuery(this).find('.dropdown').css('left', 'auto');
			},
			function(event){
				jQuery(this).find('.dropdown').css('left', '-999em');
			}
		);
		
		// Add Colorbox around commentform
		if(jQuery('#commentform').length > 0){
			jQuery('a.allComments').colorbox({
							title: function(){return ''},
							href:"#comment-form-box",
							inline: true,
							innerWidth:700,
							innerHeight:'80%',
							rel:'nofollow',
				
							onOpen: function(){
								jQuery('#comment-form-box').css('display', '');
								jQuery('#comment_parent').attr('value', 0);
							},
							onLoad: function(){
								jQuery('#cboxTopCenter').empty().append(jQuery('<h1/>', {html: 'Neuen Kommentar verfassen'}));
							},
							onComplete: function(){
								jQuery('#cboxTitle').text('');
							},
							onCleanup: function(){
								jQuery('#comment-form-box').css('display', 'none');
								jQuery('#comment_parent').attr('value', 0);
							}
				
			});

			jQuery('a.comment-reply-link').colorbox({
							href:"#comment-form-box",
							title: function(){return ''},
							inline: true,
							innerWidth:700,
							innerHeight:'80%',
							rel:'nofollow',
							onOpen: function(){
								jQuery('#comment-form-box').css('display', '');
							},
							onLoad: function(){
								jQuery('#cboxTopCenter').empty().append(jQuery('<h1/>', {html: 'Auf einen Kommentar antworten'}));
							},
							onComplete: function(){
								jQuery('#cboxTitle').text('');
							},
							onCleanup: function(){
								jQuery('#comment-form-box').css('display', 'none');
								jQuery('#comment_parent').attr('value', 0);
							}
			});
					
			jQuery('#reply-title').css('display', 'none');
			
			$('#commentform').validate();
			
		}
		
		// Simple ImageSwap for Reviews 
		jQuery(".productbutton").mouseover(function() {
			var thispic = jQuery(this).attr('rel');
			jQuery(".largeShot").css('background-image', 'url(' + thispic + ') ');
		});
			
		 // Tooltips
		jQuery('.ajaxToolTip').each(function(){
			jQuery(this).attr('title', '');
          	var offset = jQuery(this).position();
          	jQuery(this).simpletip({
				content: jQuery(this).attr('rel'),
				fixed: true,
				position: [(offset.left+120), offset.top]
			});
		});
		
		jQuery('.hideOnJS').css('display', 'none');
		
		// ZoomImages
		var zdnetLogo = jQuery('<img/>', {
			'src': '/wp-content/themes/zdnet-de/images/colorbox/zdnet-logo.png',
			'alt': 'ZDNet.de',
			'style': 'margin: 10px 5px;'
		});
		if(jQuery('a.zoomImagePro').size() > 0){
			jQuery('a.zoomImagePro').each(function(){
				var bu = jQuery(this).attr('rel');
				
				if(bu !== undefined && bu.length > 200){
					bu = bu.substr(0, 200);
					var pos = bu.lastIndexOf(" ");
					if(pos != -1 && pos > 100){
						bu = bu.substr(0, pos) + ' ...';
					}
				}
				
				if(bu === undefined){ 
					var bu = jQuery(this).find('img').first().attr('alt');
				}

				jQuery(this).attr('rel', 'imageGroup2');
			});

			jQuery("a.zoomImagePro").colorbox({
									'scalePhotos': false,
									'photo': true,
									'current': 'Bild {current} von {total}',
									'previous': 'zurück',
									'next': 'weiter',
									'close': 'schließen',
									 onOpen: function(){
        									jQuery('#cboxTopCenter').empty().append(zdnetLogo);
        								}
								}
							);
		}
		if(jQuery('a.zoomImage').size() > 0){
			jQuery('a.zoomImage').each(function(){
				var bu = jQuery(this).attr('rel');
				
				if(bu !== undefined && bu.length > 200){
					bu = bu.substr(0, 200);
					var pos = bu.lastIndexOf(" ");
					if(pos != -1 && pos > 100){
						bu = bu.substr(0, pos) + ' ...';
					}
				}
				
				if(bu === undefined){ 
					var bu = jQuery(this).find('img').first().attr('alt');
				}
				jQuery(this).attr('title', bu);
				
				jQuery(this).attr('rel', 'imageGroup1');

			});

			jQuery("a.zoomImage").colorbox({
									'scalePhotos': false,
									'photo': true,
									'current': 'Bild {current} von {total}',
									'previous': 'zurück',
									'next': 'weiter',
									'close': 'schließen',
									 onOpen: function(){
        									jQuery('#cboxTopCenter').empty().append(zdnetLogo);
        								}
								}
							);
		}
		
		// Galleries
		if(typeof zdnetGalleryStory != "undefined" && $('.ajaxGallery').length > 0 ){
			//$('div.ajaxGallery').nmiGallery({'gallery_object': zdnetGalleryStory, 'rotate_ads': [nmi_ads_leaderboard_top, nmi_ads_button_top, nmi_ads_sky_right, nmi_ads_mpu_top] });	
			$('div.ajaxGallery').nmiGallery({'gallery_object': zdnetGalleryStory });
		}
		
		if($('div.multimediaBox').length > 0){
			$('div.multimediaBox').nmiSliderBox();
		}
		
		// Ajax Tab Box
		if($('.ajaxTabBox').length > 0){
    		$('.ajaxTabBox').each(function(){
    			var ajaxTabBox = $(this);
    			var contentBox = ajaxTabBox.find('.boxWrapper');
    			var currentTabs = ajaxTabBox.find('.ajaxTabs li');
    			contentBox.find('.ajaxContent').css('display', '');
				$(contentBox).css('height', (contentBox.find('.ajaxContent').maxHeight()+10) );
    			
    			currentTabs.each(function(){
    				var currentTab = $(this);
    				if(ajaxTabBox.find('#'+currentTab.attr('rel'))){
    					ajaxTabBox.find('#'+currentTab.attr('rel')).css('display', '');
    					currentTab.data('posts', ajaxTabBox.find('#'+currentTab.attr('rel')).first());
    						
    					if(currentTab.hasClass('on') === false){
    						ajaxTabBox.find('#'+currentTab.attr('rel')).remove();
    					}
    					
	    				$(this).find('a').bind('click', function(event){
	    					event.stopPropagation();
	    					contentBox.empty();
	    					currentTabs.removeClass('on');
	    					currentTab.addClass('on');
	    					contentBox.append(currentTab.data('posts'));
	    					return false;
	    				});
    				}
    			});
    		});
		}
		
		jQuery("#slider").easySlider({
			auto: true,
			speed: 		800,
			continuous: true 
		});
	
		// Load comment waiting for approval
		if($('#comments').length > 0){
			if( ($.url().attr('fragment').indexOf('comment-') !== -1) && $('#' + $.url().attr('fragment')).length == 0 ){
				if($.url().attr('query') != ''){
					var newURL = $.url().attr('base')+$.url().attr('path')+'?'+$.url().attr('query')+'#newcomment';	
				}
				else{
					var newURL = $.url().attr('base')+$.url().attr('path')+'#newcomment';
				}
				$('#newcomment').css('display', '');
				window.location.href = newURL;
			}	
		}
		
		// Load ratings
		if($('#ajaxVote').length > 0 && typeof wp_post_id != "undefined"){
			$.ajax({
			  url: '/wp-admin/admin-ajax.php?action=showratings&id='+wp_post_id,
			  success: function(data){
			  	$('#ajaxVote').empty().append(data);
			  }
			});	
		}
		
		if(typeof(zdnetStickyAds) === "object"){
			var stickyOffsets = {};
			$.each(zdnetStickyAds, function(junk, currID){
				if($('#'+currID).length > 0){
					stickyOffsets[currID] = $('#'+currID).offset();
				}
			});
			
			if($(stickyOffsets).length > 0){
				
				$(window).scroll(function () {  
			   		var scrollTop = $(window).scrollTop();
			   		
			   		$.each(stickyOffsets, function(currID, currOffset){
				   		if (currOffset.top<scrollTop){
				   			$('#'+currID).css({
				   				'position' : 'fixed',
				   				'top': 20
				   			}); 
				   		}
				   		else{ 
				   			$('#'+currID).css({
				   				'position' : 'absolute',
				   				'top': 'auto'
				   			});
				   		}  
				   	});	
				});
			}
		}
	}
};


zdnet.loadExternal = function(){
	zdnet.loaded();
}

if (window.addEventListener){
	window.addEventListener("load", zdnet.loadExternal, false); 
} 
else if (window.attachEvent) { 
	window.attachEvent("onload", zdnet.loadExternal); 
}

if($('#fb-root').length > 0){
	zdnet.domready();
}
else{
	$(document).ready(function(){zdnet.domready();});
}

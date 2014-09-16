jQuery(document).ready(function($) {
	$('.ad-tpl').css('visibility','visible').fadeIn('slow',function(){$('.ad-tpl-inner').animate({bottom:0},"slow");});

	// Sticky Banner ...
	if (!cssdisabled && $('.lay-content-header .ad-sticky-true').length > 0) {
		$(window).scroll(function(e){
			for(var x = 0; x < $('.ad-sticky-true').length; x++) {
				$('.lay-content-header .ad-sticky-true:eq(' + x + ')').css({'top': ($(window).scrollTop() + 10) + 'px'}); 
			}
		});
	}

        //Popover schliessen
        window.popover_close = function ()
        {          
          $('.lay-nav-main ul li:first').removeAttr('style');
          $('.lay-nav-main ul li.active').css({'background-color':'#fff'});          
          $('.lay-overlay, #main-nav-popover').hide();
        };

        //Popover
        var cc = '';  
        window.popover = function ()
        {
            if ($(this).children (':first-child').hasClass ('external') === false)
            {                     
               //Dropdown Container 
               if ($('.lay-content #main-nav-popover').length == 0)
               {               
                 $('.lay-content').prepend ('<section class="ym-grid" id="main-nav-popover" style="display:none;"><div class="main-nav-popover-inner"><div class="ym-g960-9 ym-gl"><div class="ym-gbox lay-clp"><h1 class="ym-hideme">Rubriken</h1></div></div></div></section><div class="lay-overlay" style="display: none;"></div>');
                 $('.main-nav-popover-inner .ym-g960-9 .ym-gbox').append ($('.lay-nav-main li.first div ul'));
               }               
               
               //Artikel holen              
               $.ajax({
               url : "index.php",
               type : "POST",
               data : {eID : "dropdown", cc : cc},
               success : function (response, status)
               {                                 
                 if (response.length > 0)
                 {    
                    $(".main-nav-popover-inner .ym-g960-15").remove ();
                    $(".main-nav-popover-inner .ym-g960-24").remove ();  
                    $(".main-nav-popover-inner").append (response);
                 }
               }
            });                                               
            }
        };

	//Popover-Event- Rubriken Dropdown anzeigen
        $('.lay-nav-main li.first, #main-nav-popover').live('mouseover', function (event){					
            $('.lay-nav-main ul li:first').css({'background-color':'#fff'});            
            if (event.currentTarget.tagName === 'LI')
            {
              cc = ($('.lay-nav-main li.first div ul').length > 0) ?
              $('.lay-nav-main li.first li.first a span').text () :
              $('.main-nav-popover-inner .ym-g960-9 li.first a span').text ();
              window.popover ();
            }                                    
            $('.lay-overlay, #main-nav-popover').show();                                        					
        });
        
        //Popover-Event-Rubriken Dropdown verbergen
        $('.lay-nav-main li.first, #main-nav-popover').live('mouseout', function(){					
            $('.lay-nav-main ul li:first:not(.active)').css({'background-color':'transparent'});
            $('.lay-overlay, #main-nav-popover').hide();                                        
	});                           
                        
        //Popover-Event: Popover anzeigen 
        var timer;
        $('.lay-nav-main ul li ul li, #main-nav-popover ul li').on ("mouseover", function (event)
        {                         
            cc = $(this).children (":first-child").text ();
            window.clearTimeout (timer);
            timer = window.setTimeout (function () {window.popover (); }, 200);                        
        });
        
	// sync height ...
	if (0 < (c = $('.lay-sh').length) && !cssdisabled) {
		for (var i=0;i<c;i++) {
			$('.lay-sh:eq(' + i + ') .lay-shi').syncHeight({'updateOnResize':false});
			$('.lay-sh:eq(' + i + ') .lay-shi').css('height',$('.lay-sh:eq(' + i + ') .lay-shi:eq(0)').css('min-height'));// 
		}
	}

	// cc-boxen ...
	if (!cssdisabled && Modernizr.localstorage && !(true == $.browser.msie && 9 > parseInt($.browser.version))) {
		window.loadusersettings = true;

		window.setIconStatuses = function() {
			if (!window.loadusersettings) {
				for (var x = 0; x < $('section.lay-cc-js').length; x++) {
					localStorage.removeItem('section#' + $('section.lay-cc-js')[x].id + ' position');
					localStorage.setItem('section#' + $('section.lay-cc-js')[x].id + ' position', x);
				}
			}

			$('.lay-cc-js .btn-cc-moveup').replaceWith('<a class="btn-cc-moveup icon icon-cc-moveup" href="javascript:void(0);" title="Rubrik nach oben verschieben"><img src="fileadmin/img/icon-cc-moveup.gif" height="21" width="22" alt="" /></a>');
			$('.lay-cc-js .btn-cc-movedown').replaceWith('<a class="btn-cc-movedown icon icon-cc-movedown" href="javascript:void(0);" title="Rubrik nach unten verschieben"><img src="fileadmin/img/icon-cc-movedown.gif" height="21" width="22" alt="" /></a>');
			$('.lay-cc-opened:first .btn-cc-moveup, .lay-cc-closed:first .btn-cc-moveup').replaceWith('<span class="btn-cc-moveup icon icon-cc-moveup"><img src="fileadmin/img/icon-cc-moveup-disabled.gif" height="21" width="22" alt="" /></span>');
			$('.lay-cc-opened:last .btn-cc-movedown, .lay-cc-closed:last .btn-cc-movedown').replaceWith('<span class="btn-cc-movedown icon icon-cc-movedown"><img src="fileadmin/img/icon-cc-movedown-disabled.gif" height="21" width="22" alt="" /></span>');
		}

		window.ccSetPosition = function(selector, newposition) {
			if (0 <= newposition && $('section.lay-cc-js').length > newposition) {
				for (var y = 0; y < $('section.lay-cc-js').length; y++) {
					if (selector == 'section#' + $('section.lay-cc-js')[y].id) {
						currentposition = y;
					}
				}

				if (currentposition == newposition) { return; };

				if (currentposition < newposition) {
					for (var y = currentposition; y < newposition; y++) {
						$($('#' + $('section.lay-cc-js:eq('+y+')')[0].id)).insertAfter($('section#' + $('section.lay-cc-js:eq('+(y+1)+')')[0].id));
					}
				} else {
					for (var y = currentposition; y > newposition; y--) {
						$($('#' + $('section.lay-cc-js:eq('+y+')')[0].id)).insertBefore($('section#' + $('section.lay-cc-js:eq('+(y-1)+')')[0].id));
					}
				}

				/*** if (currentposition > newposition) {
					$($('#' + $('section.lay-cc')[y+1].id)).insertBefore($('#' + $('section.lay-cc')[y].id));
				} else {
					$($('#' + $('section.lay-cc')[y].id)).insertAfter($('#' + $('section.lay-cc')[y+1].id));
				} ***/
			}

			window.setIconStatuses();
		}

		window.ccSetClose = function(selector, closed) {
			if (closed) {
				$(selector).addClass('lay-cc-closed');
				$(selector).removeClass('lay-cc-opened');
				$(selector + ' .btn-cc-close').replaceWith('<a class="btn-cc-open icon icon-cc-open" href="javascript:void(0);" title="Rubrik einblenden"><img src="fileadmin/img/icon-cc-open.gif" height="21" width="22" alt="" /></a>');

				window.ccSetPosition(selector, $('section.lay-cc').length - 1);
				localStorage.setItem(selector + ' closed', true);
			} else {
				$(selector).removeClass('lay-cc-closed');
				$(selector).addClass('lay-cc-opened');
				$(selector + ' .btn-cc-open').replaceWith('<a class="btn-cc-close icon icon-cc-close" href="javascript:void(0);" title="Rubrik ausblenden"><img src="fileadmin/img/icon-cc-close.gif" height="21" width="22" alt="" /></a>');

				window.ccSetPosition(selector, $('section.lay-cc-opened').length - 1);
				localStorage.setItem(selector + ' closed', false);
			}

			window.setIconStatuses();
		}

		window.ccMoveUp = function(events, data, eventObject) {
			var selector = 'section#' + events.currentTarget.parentElement.parentElement.parentElement.id;
			
			for (var z = 0; z < $('section.lay-cc-js').length; z++) {
				if (selector == 'section#' + $('section.lay-cc-js')[z].id) {
					window.ccSetPosition(selector, z - 1);
					return;
				}
			}
		}

		window.ccMoveDown = function(events, data, eventObject) {
			var selector = 'section#' + events.currentTarget.parentElement.parentElement.parentElement.id;

			for (var o = 0; o < $('section.lay-cc-js').length; o++) {
				if (selector == 'section#' + $('section.lay-cc-js')[o].id) {
					window.ccSetPosition(selector, o + 1);
					return;
				}
			}
		}

		window.ccOpen = function(events, data, eventObject) {
			var selector = 'section#' + events.currentTarget.parentElement.parentElement.parentElement.id;
			localStorage.setItem(selector + ' closed', false);

			window.ccSetClose(selector, false);
		}

		window.ccClose = function(events, data, eventObject) {
			var selector = 'section#' + events.currentTarget.parentElement.parentElement.parentElement.id;
			localStorage.setItem(selector + ' closed', true);

			window.ccSetClose(selector, true);
		}

		// Icons plazieren ...
		$('section.lay-cc-js header').append(
			'<div class="cc-icons"><a class="btn-cc-moveup icon icon-cc-moveup" href="javascript:void(0);" title="Rubrik nach oben verschieben"><img src="fileadmin/img/icon-cc-moveup.gif" height="21" width="22" alt="" /></a>'
			+ '<a class="btn-cc-movedown icon icon-cc-movedown" href="javascript:void(0);" title="Rubrik nach unten verschieben"><img src="fileadmin/img/icon-cc-movedown.gif"w height="21" width="22" alt="" /></a>'
			+ '<a class="btn-cc-close icon icon-cc-close" href="javascript:void(0);" title="Rubrik ausblenden"><img src="fileadmin/img/icon-cc-close.gif" height="21" width="22" alt="" /></a></div>'
		);

		$('a.btn-cc-moveup').live('click', window.ccMoveUp);
		$('a.btn-cc-movedown').live('click', window.ccMoveDown);
		$('a.btn-cc-open').live('click', window.ccOpen);
		$('a.btn-cc-close').live('click', window.ccClose);

		// Benutzereinstellungen laden ...
		var selectorArray = Array();
		var ccClosedArray = Array();
		var ccPositionArray = Array();

		for (var i = 0; i < $('section.lay-cc-js').length; i++) {
			selectorArray[i] = 'section#' + $('section.lay-cc-js')[i].id;
		}

		for (var i = 0; i < selectorArray.length; i++) {
			if ("true" == localStorage.getItem(selectorArray[i] + ' closed')) {
				window.ccSetClose(selectorArray[i], true);
			} else {
				window.ccSetClose(selectorArray[i], false);
			}

			if (localStorage.getItem(selectorArray[i] + ' position')) {
				ccPositionArray[localStorage.getItem(selectorArray[i] + ' position')] = selectorArray[i];
			}
		}

		for (var i = 0; i < ccPositionArray.length; i++) {
			window.ccSetPosition(ccPositionArray[i], i);
		}

		window.loadusersettings = false;
	}
	
	// Media slider ...
	if (!cssdisabled && !($.browser == 'msie' && 7 > parseInt($.browser.version))) {
		$('.lay-media .lay-media-list ul').css({'width': '20000em', 'position': 'absolute'});
		$('.lay-media div ul li').removeClass('ym-hideme');

		// Hoehe ...
		for (var x = 0; x < $('.lay-media').length; x++) {
			var itemHeight = 0;

			for (i = 0; i < $('.lay-media:eq(' + x + ') div ul li').length; i++) {
				var oh = $('.lay-media:eq(' + x + ') div ul li:eq(' + i + ')').height();

				if (itemHeight < oh) { 
					itemHeight = oh;
				}
			}

			if (oh) {
				$('.lay-media:eq(' + x + ') div.lay-media-list').css('height', (itemHeight + 10) + 'px');
			}
		}

		// Pager ...
		$('.lay-media h1').before('<div class="lay-media-pagebrowser"><a class="btn-media-prev icon" href="javascript:void(0);" title="zurückblättern"><img src="fileadmin/img/icon-left.gif" width="26" height="26" alt="" /></a><span></span><a class="btn-media-next icon" href="javascript:void(0);" title="vorblättern"><img src="fileadmin/img/icon-right.gif" width="26" height="26" alt="" /></a></div>');

		// Slider ...
		var mediaSlider = Array();
		var mediaSliderTrenner = ' | ';
		var mediaSliderVisibleItems = 4;

		window.mediaSliderItemPosition = Array();

		for (var i = 0; i < $('.lay-media').length; i++) {
			mediaSlider[i] = $('.lay-media:eq(' + i + ') .lay-media-list').jcarousel({
				'wrap': 'both',
				'visible': mediaSliderVisibleItems,
				'animation': 0,
				'identifier':  i
			});
			
			window.mediaSliderItemPosition[i] = 0;

			html = '';
			y = 0;

			for (var x = 0; x < $('.lay-media:eq(' + i + ') .lay-media-list ul li').length; x++) {
				if (x % mediaSliderVisibleItems == 0) {
					html += '<a href="javascript:void(0);">' + (y+1) + '</a>' + mediaSliderTrenner;

					$('.lay-media:eq(' + i + ') .lay-media-pagebrowser span a:eq(' + y + ')').jcarouselControl({carousel: mediaSlider[i], target: x});
					y++;
				}
			}

			$('.lay-media:eq(' + i + ') .lay-media-pagebrowser .btn-media-prev').jcarouselControl({carousel: mediaSlider[i], target: '-=' + mediaSliderVisibleItems});
			$('.lay-media:eq(' + i + ') .lay-media-pagebrowser .btn-media-next').jcarouselControl({carousel: mediaSlider[i], target: '+=' + mediaSliderVisibleItems});

			html = html.substr(0, html.length - mediaSliderTrenner.length);
			$('.lay-media:eq(' + i + ') .lay-media-pagebrowser span').html(html);

			for (var x = 0; x < $('.lay-media:eq(' + i + ') .lay-media-pagebrowser span a').length; x++) {
				$('.lay-media:eq(' + i + ') .lay-media-pagebrowser span a:eq(' + x + ')').jcarouselControl({carousel: mediaSlider[i], target: '' + (x*mediaSliderVisibleItems)});
			}

			$('.lay-media:eq(' + i + ') .lay-media-pagebrowser span a:eq(0)').addClass('active');
		}

		$('.lay-media-pagebrowser a').live('click', function(events, data, eventObject){events.currentTarget.blur();});		
                $('.lay-media').on('jcarousel:scroll', function(event, carousel, target, animate) {    
                    
			switch(target.substr(0,2)) {
				case '+=':
					newpos = window.mediaSliderItemPosition[carousel.options().identifier] + parseInt(target.substr(2,1));
					newpos = (newpos >= carousel.items().length && (carousel.items().length < newpos < (carousel.items().length + 4))) ? 0 : newpos;

					break;
				case '-=':
					newpos = window.mediaSliderItemPosition[carousel.options().identifier] - parseInt(target.substr(2,1));
					newpos = (newpos < 0) ? (Math.ceil(carousel.items().length / parseInt(carousel.options().visible)) * parseInt(carousel.options().visible) - parseInt(carousel.options().visible)) : newpos;

					break;
				default:
					newpos = target;

					break;
			}

			window.mediaSliderItemPosition[carousel.options().identifier] = parseInt(newpos);

			$('.lay-media:eq(' + carousel.options().identifier + ') .lay-media-pagebrowser span a').removeClass('active');		
                        $('.lay-media:eq(' + carousel.options().identifier + ') .lay-media-pagebrowser span a:eq(' + (window.mediaSliderItemPosition[carousel.options().identifier] / parseInt(carousel.options().visible)) + ')').addClass('active');                        
		});
	}

	if (!cssdisabled && !($.browser == 'msie' && 7 > parseInt($.browser.version))) {
		// More dropdown ...
		for(var i = 0; i < $('ul.lay-list-more').length; i++) {
			var html = '';
			var htmlMore = '';
			var moreListCutPosition = 0;
			var classList = $('ul.lay-list-more:eq('+i+')').attr('class').split(/\s+/);

			$.each(classList, function(index, item) {
				if (0 < parseInt(item.substr(14,1))) {
					moreListCutPosition = parseInt(item.substr(14,1));
				}
			});

			if ($('ul.lay-list-more:eq('+i+') li').length > moreListCutPosition) {
				for(var x = 0; x < $('ul.lay-list-more:eq('+i+') li').length; x++) {
					if (x < moreListCutPosition) {
						html += $('ul.lay-list-more:eq('+i+') li:eq('+x+')')[0].outerHTML;
					} else {
						if (x == moreListCutPosition) {
							html += '<li class="last"><span><a class="btn-more" href="javascript:void(0);">mehr</a><!-- MORE_LIST --></span></li>';
						}

						htmlMore += $('ul.lay-list-more:eq('+i+') li:eq('+x+')')[0].outerHTML;
					}
				}
				
				html = html.replace(/<!-- MORE_LIST -->/, '<ul>'+htmlMore+'</ul>');
				
				$('ul.lay-list-more:eq('+i+')').html(html);
				$('ul.lay-list-more:eq('+i+') li ul li:first').addClass('first');
	
			}
		}

		if ('' != htmlMore) {
			$('.lay-list-more li.last span').css({'padding-right': '18px', 'background': 'transparent url(fileadmin/img/arrow-more.gif) no-repeat bottom right'});
			$('.lay-list-more li.last span span').css({'background': '0', 'padding': '0'});
		}

		// accordion ...
		if (!cssdisabled) {
			window.accordionInterval = 3500;
			window.accordionIntervalTimer = Array();
			
			window.accordionNextSlide = function(index) {
				for(var x = 0; x < $('.lay-inbox-accordion:eq('+index+') ul li').length; x++) {
					if ($('.lay-inbox-accordion:eq('+index+') ul li:eq('+x+')').hasClass('current')) {
						if ($('.lay-inbox-accordion:eq('+index+') ul li').length - 1 == x) {
							$('.lay-inbox-accordion:eq('+index+') ul li:eq(0)').addClass('current');
						} else {
							$('.lay-inbox-accordion:eq('+index+') ul li:eq('+(x+1)+')').addClass('current');
						}

						$('.lay-inbox-accordion:eq('+index+') ul li:eq('+x+')').removeClass('current');
						return;
					}
				}
			}

			for (var i = 0; i < $('.lay-inbox-accordion').length; i++) {
				$('.lay-inbox-accordion:eq('+i+')').attr('accordion-identifier', i);
				window.accordionIntervalTimer[i] = window.setInterval('window.accordionNextSlide('+i+')', window.accordionInterval);

				$('.lay-inbox-accordion:eq('+i+')').on('mouseover', function(event){
					var accordion_identifier = $(this).attr('accordion-identifier');
					window.clearInterval(window.accordionIntervalTimer[accordion_identifier]);
				});

				$('.lay-inbox-accordion:eq('+i+')').on('mouseout', function(event){
					var accordion_identifier = $(this).attr('accordion-identifier');
					window.accordionIntervalTimer[accordion_identifier] = window.setInterval('window.accordionNextSlide('+accordion_identifier+')', window.accordionInterval);
				});

				$('.lay-inbox-accordion:eq('+i+') ul li').on('mouseover', function(event){
					var accordion_identifier = $(this).parent().parent().attr('accordion-identifier');

					$('.lay-inbox-accordion:eq('+accordion_identifier+') ul li').removeClass('current');
					$('.lay-inbox-accordion:eq('+accordion_identifier+') ul li:eq('+$(this).index()+')').addClass('current');
				});
			}
		}
	}

	if (0 < $('article#article-detail').length && !cssdisabled) {
		// article image floating ...
		var articleOffset = $('article#article-detail').offset();
		var articleWidth = $('article#article-detail').width();

		$('article#article-detail figure, article#article-detail aside.lay-article-download').each(function(index){
			var figureOffset = $(this).offset();
			if(80 < articleWidth - $(this).width() + articleOffset.left - figureOffset.left) {
				
				if (!$(this).hasClass('last')) {
					$(this).addClass('float-left');	
				}
			}
		});

		var html = '<a class="icon icon-printer" href="javascript:window.print();" title="Artikel drucken"><img src="fileadmin/img/icon-printer.gif" height="19" width="18" alt=""></a>';
		$('article#article-detail').append(html);
	}

	// social icons ...
	if (0 < $('.lay-social-anchor').length && !cssdisabled && typeof ELNET_SHARE_URL != 'undefined') {
		var html = '<div><a class="icon" href="https://plus.google.com/share?url=' + ELNET_SHARE_URL + '" onclick="javascript:window.open(this.href,\'\',\'menubar=no,toolbar=no,resizable=yes,scrollbars=yes,height=600,width=600\');return false;" title="Artikel auf Google+ teilen"><img src="fileadmin/img/social-google.gif" height="19" width="20" alt="Google+"></a>'
				 + '<a class="icon" href="http://www.facebook.com/share.php?u=' + ELNET_SHARE_URL + '" onclick="javascript:window.open(this.href,\'\',\'menubar=no,toolbar=no,resizable=yes,scrollbars=yes,height=600,width=600\');return false;" title="Artikel auf Facebook teilen"><img src="fileadmin/img/social-facebook.gif" height="19" width="20" alt="Facebook"></a>'
				 + '<a class="icon" href="http://twitter.com/home?status=' + ELNET_SHARE_URL + '" onclick="javascript:window.open(this.href,\'\',\'menubar=no,toolbar=no,resizable=yes,scrollbars=yes,height=600,width=600\');return false;" title="Artikel auf Twitter teilen"><img src="fileadmin/img/social-twitter.gif" height="19" width="20" alt="Twitter"></a></div>';
		$('.lay-social-anchor').append(html);
	}
	
	// social icons page ...
	if (!cssdisabled) {
		var html = '<div class="lay-social-page"><a class="icon" href="http://www.facebook.com/elektroniknet.de"><img src="fileadmin/img/social-facebook.gif" height="19" width="20" alt="Facebook"></a>'
				 + '<a class="icon" href="https://twitter.com/elektroniknet"><img src="fileadmin/img/social-twitter.gif" height="19" width="20" alt="Twitter"></a>'
				 + '<a class="icon" href="http://www.elektroniknet.de/rss/"><img src="fileadmin/img/social-rss.gif" height="19" width="20" alt="RSS-Feed"></a>'
				 + '</div>';
		$('.lay-nav-row2 .ym-g960-17').append(html);

	}
	
	// anbieter ...
	if (0 < $('.lay-anbieter-products').length && !cssdisabled) {
		$('.lay-anbieter-products ul li ul').hide();
		$('.lay-anbieter-products span').css({'cursor': 'pointer'});
		
		$('.lay-anbieter-products ul li span').css({'padding-right': '18px', 'background': 'transparent url(fileadmin/img/arrow-more.gif) no-repeat bottom right'});
		$('.lay-anbieter-products ul li li span').css({'background': '0', 'padding': '0'});
		
		$('.lay-anbieter-products span').on('click', function(event){
			var fr = $(this).attr('ref');
			
			
			$('.lay-anbieter-products ul li ul').each(function(){
				if (fr.indexOf($(this).attr('id')) == -1) {
					$('.lay-anbieter-products span[ref=' + $(this).attr('id') + ']').removeClass('active');
					$(this).hide();
				}
			});
			
			if($('#' + $(this).attr('ref')).is(":hidden"))
			{
				var fl = $(this).attr('ref').substring(0, -2);
				$('#' + $(this).attr('ref')).show();
				$(this).addClass('active');
			}
		});
	}
	
	// anbieter detail ...
	$('.lay-anbieter-detail-profileitem').truncate({max_length: 820, more: 'alles anzeigen'});
	$('.lay-anbieter-detail-buitem').truncate({max_length: 100, more: 'mehr'});

	// android video support ...
	var isAndroid = navigator.userAgent.toLowerCase().indexOf("android") > -1;
	if (isAndroid) {
		$('video').live('click', function(){
			video.play();
		});
	}
	
	// anbieterverzeichnis ...
	if ($('a.termin').length > 0 && 'undefined' != typeof $('a.termin').fancybox) {
		$('a.termin').fancybox({
			'width'		: 600,
			'height'	: 600,
			'autoScale'	: true,
			'type'		: 'iframe',
			'titleShow'	: false
		});
	}
});


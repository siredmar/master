// Fixing Array IndexOF for IE
if (!Array.prototype.indexOf) {
    Array.prototype.indexOf = function (searchElement /*, fromIndex */ ) {
        "use strict";
        if (this == null) {
            throw new TypeError();
        }
        var t = Object(this);
        var len = t.length >>> 0;
        if (len === 0) {
            return -1;
        }
        var n = 0;
        if (arguments.length > 0) {
            n = Number(arguments[1]);
            if (n != n) { // shortcut for verifying if it's NaN
                n = 0;
            } else if (n != 0 && n != Infinity && n != -Infinity) {
                n = (n > 0 || -1) * Math.floor(Math.abs(n));
            }
        }
        if (n >= len) {
            return -1;
        }
        var k = n >= 0 ? n : Math.max(len - Math.abs(n), 0);
        for (; k < len; k++) {
            if (k in t && t[k] === searchElement) {
                return k;
            }
        }
        return -1;
    }
}


// JQuery URL Parser plugin - https://github.com/allmarkedup/jQuery-URL-Parser
// Written by Mark Perkins, mark@allmarkedup.com
// License: http://unlicense.org/ (i.e. do what you want with it!)

;(function($, undefined) {
    
    var tag2attr = {
        a       : 'href',
        img     : 'src',
        form    : 'action',
        base    : 'href',
        script  : 'src',
        iframe  : 'src',
        link    : 'href'
    },
    
	key = ["source","protocol","authority","userInfo","user","password","host","port","relative","path","directory","file","query","fragment"], // keys available to query
	
	aliases = { "anchor" : "fragment" }, // aliases for backwards compatability

	parser = {
		strict  : /^(?:([^:\/?#]+):)?(?:\/\/((?:(([^:@]*):?([^:@]*))?@)?([^:\/?#]*)(?::(\d*))?))?((((?:[^?#\/]*\/)*)([^?#]*))(?:\?([^#]*))?(?:#(.*))?)/,  //less intuitive, more accurate to the specs
		loose   :  /^(?:(?![^:@]+:[^:@\/]*@)([^:\/?#.]+):)?(?:\/\/)?((?:(([^:@]*):?([^:@]*))?@)?([^:\/?#]*)(?::(\d*))?)(((\/(?:[^?#](?![^?#\/]*\.[^?#\/.]+(?:[?#]|$)))*\/?)?([^?#\/]*))(?:\?([^#]*))?(?:#(.*))?)/ // more intuitive, fails on relative paths and deviates from specs
	},
	
	querystring_parser = /(?:^|&|;)([^&=;]*)=?([^&;]*)/g, // supports both ampersand and semicolon-delimted query string key/value pairs
	
	fragment_parser = /(?:^|&|;)([^&=;]*)=?([^&;]*)/g; // supports both ampersand and semicolon-delimted fragment key/value pairs
	
	function parseUri( url, strictMode )
	{
		var str = decodeURI( url ),
		    res   = parser[ strictMode || false ? "strict" : "loose" ].exec( str ),
		    uri = { attr : {}, param : {}, seg : {} },
		    i   = 14;
		
		while ( i-- )
		{
			uri.attr[ key[i] ] = res[i] || "";
		}
		
		// build query and fragment parameters
		
		uri.param['query'] = {};
		uri.param['fragment'] = {};
		
		uri.attr['query'].replace( querystring_parser, function ( $0, $1, $2 ){
			if ($1)
			{
				uri.param['query'][$1] = $2;
			}
		});
		
		uri.attr['fragment'].replace( fragment_parser, function ( $0, $1, $2 ){
			if ($1)
			{
				uri.param['fragment'][$1] = $2;
			}
		});
				
		// split path and fragement into segments
		
        uri.seg['path'] = uri.attr.path.replace(/^\/+|\/+$/g,'').split('/');
        
        uri.seg['fragment'] = uri.attr.fragment.replace(/^\/+|\/+$/g,'').split('/');
        
        // compile a 'base' domain attribute
        
        uri.attr['base'] = uri.attr.host ? uri.attr.protocol+"://"+uri.attr.host + (uri.attr.port ? ":"+uri.attr.port : '') : '';
        
		return uri;
	}
	
	function getAttrName( elm )
	{
		var tn = elm.tagName;
		if ( tn !== undefined ){ 
			return tag2attr[tn.toLowerCase()];
		}
		return tn;
	}
	
	$.fn.url = function( strictMode )
	{
	    var url = '';
	    
	    if ( this.length )
	    {
	        url = $(this).attr( getAttrName(this[0]) ) || '';
	    }
	    
        return $.url({ url : url, strict : strictMode });
	};
	
	$.url = function( opts )
	{
	    var url     = '',
	        strict  = false;

	    if ( typeof opts === 'string' )
	    {
	        url = opts;
	    }
	    else
	    {
	        opts = opts || {};
	        strict = opts.strict || strict;
            url = opts.url === undefined ? window.location.toString() : opts.url;
	    }

        return {
            
            data : parseUri(url, strict),
            
            // get various attributes from the URI
            attr : function( attr )
            {
                attr = aliases[attr] || attr;
                return attr !== undefined ? this.data.attr[attr] : this.data.attr;
            },
            
            // return query string parameters
            param : function( param )
            {
                return param !== undefined ? this.data.param.query[param] : this.data.param.query;
            },
            
            // return fragment parameters
            fparam : function( param )
            {
                return param !== undefined ? this.data.param.fragment[param] : this.data.param.fragment;
            },
            
            // return path segments
            segment : function( seg )
            {
                if ( seg === undefined )
                {
                    return this.data.seg.path;                    
                }
                else
                {
                    seg = seg < 0 ? this.data.seg.path.length + seg : seg - 1; // negative segments count from the end
                    return this.data.seg.path[seg];                    
                }
            },
            
            // return fragment segments
            fsegment : function( seg )
            {
                if ( seg === undefined )
                {
                    return this.data.seg.fragment;                    
                }
                else
                {
                    seg = seg < 0 ? this.data.seg.fragment.length + seg : seg - 1; // negative segments count from the end
                    return this.data.seg.fragment[seg];                    
                }
            }
            
        };
        
	};
	
})(jQuery);


/**
 * jQuery isHovered (http://mktgdept.com/jquery-ishovered)
 * A jQuery plugin to test if an element is currently hovered
 *
 * v0.0.1 - 11 June 2010
 *
 * Copyright (c) 2010 Chad Smith (http://twitter.com/chadsmith)
 * Dual licensed under the MIT and GPL licenses.
 * http://www.opensource.org/licenses/mit-license.php
 * http://www.opensource.org/licenses/gpl-license.php
 *
 * Test if an element is hovered using: $(selector).isHovered() or $.isHovered(selector)
 *
 **/
(function(b,c) {
	b('*').hover( function() {
		b(this).data(c,1);
	}, 
	function() {
		b(this).data(c,0);
	}).data(c,0);

	b[c]= function(a) {
		return b(a)[c]();
	};
	
	b.fn[c]= function(a){
		a=0;
		b(this).each(function(){
			a+=b(this).data(c);
		});
		return a>0;
	};
})(jQuery,'isHovered');


/**
 * jQuery MaxHeight
 * A jQuery plugin to get the max height of all elements 
 *
 * v0.0.1 - 12.05.2011
 *
 * Copyright (c) 2011 Torsten Ullmann (Torsten.Ullmann@cbsinteractive.com)
 *
 * Get the max height of multiple elements 
 *
 **/
;(function( $ ){
  	$.fn.maxHeight = function() {
		var max = 0;
	
		this.each(function() {
			max = Math.max( max, $(this).height() );
	   	});
	   	
		return max;
	};
})(jQuery);



/**
 * jQuery strip_tags
 * A jQuery plugin strip the tags from a string
 *
 * v0.0.1 - 12.05.2011
 *
 * Copyright (c) 2011 
 *
 * Strip the tags like in php 
 *
 **/
;(function($, undefined) {
	$.fn.strip_tags = function(arguments){
		this.each(function() {
			$(this).html($.strip_tags($(this).html, arguments));
	   	});	
	}
	$.strip_tags = function(html, arguments){
		//PROCESS STRING
		if(arguments === undefined){
			arguments = [];
		}
		if(arguments.length < 3) {
			html=html.replace(/<\/?(?!\!)[^>]*>/gi, '');
		} 
		else 
		{
			var allowed = arguments[1];
			var specified = eval("["+arguments[2]+"]");
			if(allowed){
				var regex='</?(?!(' + specified.join('|') + '))\b[^>]*>';
				html=html.replace(new RegExp(regex, 'gi'), '');
			} else{
				var regex='</?(' + specified.join('|') + ')\b[^>]*>';
				html=html.replace(new RegExp(regex, 'gi'), '');
			}
		}
		//CHANGE NAME TO CLEAN JUST BECAUSE 
		var clean_string = html;
 
		//RETURN THE CLEAN STRING
		return clean_string;
	};
})(jQuery);


/**
 * jQuery log
 * A jQuery plugin to log some stuff to console.log
 *
 * v0.0.1 - 12.05.2011
 *
 * Copyright (c) 2011 Torsten Ullmann (Torsten.Ullmann@cbsinteractive.com)
 *
 * Loggin on console.log
 *
 **/
;(function($){
	$.fn.log = function(msg) {
		if(!console || !console.log) {
			console = {};
			console.log = {};
		}
		console.log("%s: %o", msg, this);
		
		return this;
	};
})(jQuery);


/**
 * jQuery fixOpacity
 * fixes the Opacity for IE Browsers  
 *
 * v0.0.1 - 12.05.2011
 *
 * Copyright (c) 2011 Torsten Ullmann (Torsten.Ullmann@cbsinteractive.com)
 *
 * Removed the opacity / alpha filter for IE browsers 
 *
 **/
;(function($){
	$.fn.fixOpacity = function() {  

		return this.each(function() {
			var currentObject = $(this);
			
			if (jQuery.browser.msie){
				currentObject.css('filter', '');
			}
		});
	};
})( jQuery );


/* jQuery.preloader - v0.95 - K Reeve aka BinaryKitten
*
* v0.95
* 	# Note - keeping below v1 as really not sure that I consider it public usable.
* 	# But it saying that it does the job it was intended to do.
* 	Added Completion of loading callback.
* 	Main Reworking With Thanks to Remy Sharp of jQuery for Designers
*
*
* v0.9
* 	Fixed .toString being .toSteing
*
* v0.8
*		Fixed sheet.href being null error (was causing issues in FF3RC1)
*
* v0.7
*		Remade the preLoadImages from jQuery to DOM
*
* v0.6
* 		Fixed IE6 Compatability!
*		Moved from jQuery to DOM
*
* v0.5
* 		Shifted the additionalimages loader in the preLoadAllImages so it wasn't called multiple times
* 		Created secondary .preLoadImages to handle additionalimages and so it can be called on itself
*/

(function ($) {
	$.preLoadImages = function(imageList,callback) {
		var pic = [], i, total, loaded = 0;
		if (typeof imageList != 'undefined') {
			if ($.isArray(imageList)) {
				total = imageList.length; // used later
					for (i=0; i < total; i++) {
						pic[i] = new Image();
						pic[i].onload = function() {
							loaded++; // should never hit a race condition due to JS's non-threaded nature
							if (loaded == total) {
								if ($.isFunction(callback.onSuccess)) {
									callback.onSuccess();
								}
							}
						};
						pic[i].onerror = function() {
							loaded++; // should never hit a race condition due to JS's non-threaded nature
							if (loaded == total) {
								if ($.isFunction(callback.onError)) {
									callback.onError();
								}
							}
						}
						pic[i].src = imageList[i];
					}
			}
			else {
				pic[0] = new Image();
				pic[0].onload = function() {
					if ($.isFunction(callback.onSuccess)) {
						callback.onSuccess();
					}
				}
				pic[0].onerror = function() {
					if ($.isFunction(callback.onError)) {
						callback.onError();
					}
				}
				pic[0].src = imageList;
			}
		}
		pic = undefined;
	};

	$.preLoadCSSImages = function(callback) {
		var pic = [], i, imageList = [], loaded = 0, total, regex = new RegExp("url\((.*)\)",'i'),spl;
		var cssSheets = document.styleSheets, path,myRules,Rule,match,txt,img,sheetIdx,ruleIdx;
		for (sheetIdx=0;sheetIdx < cssSheets.length;sheetIdx++){
			var sheet = cssSheets[sheetIdx];
			if (typeof sheet.href == 'string' && sheet.href.length > 0) {
				spl = sheet.href.split('/');spl.pop();path = spl.join('/')+'/';
			}
			else {
				path = './';
			}
			myRules = sheet.cssRules ? sheet.cssRules : sheet.rules;
			for (ruleIdx=0;ruleIdx < myRules.length;ruleIdx++) {
				Rule = myRules[ruleIdx];
				txt = Rule.cssText ? Rule.cssText : Rule.style.cssText;
				match = regex.exec(txt);
				if (match != null) {
					img = match[1].substring(1,match[1].indexOf(')',1));
					if (img.substring(0,4) == 'http') {
						imageList[imageList.length] = img;
					}
					else if ( match[1].substring(1,2) == '/') {
						var p2 = path.split('/');p2.pop();p2.pop();p2x = p2.join("/");
						imageList[imageList.length] = p2x+img;
					}
					else {
						imageList[imageList.length] = path+img;
					}
				}
			};
		};

		total = imageList.length; // used later
		for (i=0; i < total; i++) {
			pic[i] = new Image();
			pic[i].onload = function() {
				loaded++; // should never hit a race condition due to JS's non-threaded nature
				if (loaded == total) {
					if ($.isFunction(callback)) {
						callback();
					}
				}
			};
			pic[i].src = imageList[i];
		}

	};
	$.preLoadAllImages = function(imageList,callback) {
		if (typeof imageList != 'undefined') {
			if ($.isFunction(imageList)) {
				callback = imageList;
			}
			else if (!$.isArray(imageList)) {
				imageList = [imageList];
			}
		}
		$.preLoadCSSImages(function(){
			if (imageList.length > 0) {
				$.preLoadImages(imageList,function(){
					callback();
				});
			}
			else {
				callback();
			}
		});
	}
})(jQuery);

/*! Copyright 2011, Ben Lin (http://dreamerslab.com/)
* Licensed under the MIT License (LICENSE.txt).
*
* Version: 1.0.0
*
* Requires: jQuery 1.2.6+
*/

$.fn.center = function( options ){
  
  // cache gobal
  var $w = $( window ),

  scrollTop = $w.scrollTop();

  return this.each( function(){
    
    // cache $( this )
    var $this = $( this ),
    
    // merge user options with default settings
    settings = $.extend({
      against : 'window',
      top : false,
      topPercentage : 0.5
    }, options ),
    
    centerize = function(){
      var $against, x, y;
      
      if( settings.against === 'window' ){
        $against = $w;
      }else if( settings.against === 'parent' ){
        $against = $this.parent();
        scrollTop = 0;
      }else{
        $against = $this.parents( against );
        scrollTop = 0;
      }
      
      x = (( $against.width()) - ( $this.outerWidth())) * 0.5;
      y = (( $against.height()) - ( $this.outerHeight())) * settings.topPercentage + scrollTop;

      if( settings.top ) y = settings.top + scrollTop;

      $this.css({
        'left' : x,
        'top' : y
      });
    };
    
    // apply centerization
    centerize();
    $w.resize( centerize );
  });
};
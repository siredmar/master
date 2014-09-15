/**
 * Probance WebTracking
 * adaptation for NME
 * @version 1.00 Probance/ADH 20120327
 * @version 1.01 Probance/ADH 20120328
 * @version 1.02 Probance/ADH 20130108
 */


var PROBANCE_browser_detect = {
	init: function () {
		this.browser = this.searchString(this.dataBrowser) || "UNKNOWN";
		this.version = this.searchVersion(navigator.userAgent)
			|| this.searchVersion(navigator.appVersion)
			|| "UNKNOWN";
		this.OS = this.searchString(this.dataOS) || "UNKNOWN";
	},
	searchString: function (data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
	},
	dataBrowser: [
		{
			string: navigator.userAgent,
			subString: "Chrome",
			identity: "Chrome"
		},
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari",
			versionSearch: "Version"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{	
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			   string: navigator.userAgent,
			   subString: "iPhone",
			   identity: "iPhone/iPod"
	    },
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]

};
PROBANCE_browser_detect.init();

var PROBANCE_tracker = {
	tracking_param:"id_prob", 
	cookie_name:"webtrack_probance",
	cookie_timeout:365, 
	listener:"t2.probance.com/webtrax/listener.action", 
	siteid:"3yQjCsQPTmrwZDK",
	informer:"probance_informer_3yQjCsQPTmrwZDK",
	request:"",

	getParams: function(url) {
		var params = { }; 
		if (url.indexOf("?") == -1) return params; 
		paramarray = (url.split("?")[1]).split("&"); 
		for(i=0;i<paramarray.length;i++) {
			next = paramarray[i].split("=");
			params[next[0]] = next[1]; 
		}
		return params;
	},

	deleteParam: function(url, param) {
		if (url.indexOf("?") == -1) return url; 
		result=url.split("?")[0];
		paramarray = (url.split("?")[1]).split("&");
		count = 0;  
		for(i=0;i<paramarray.length;i++) {
			if (paramarray[i].split("=")[0] != param) {
				if ( count==0 ) result = result + "?";
				else result = result + "&";
				result = result + paramarray[i];
				count++;  
			}
		}
		return result; 
	},

	getProtocol: function(url) {
		proto = url.split(":")[0];
		if (proto == null || proto == "") {
			proto = "http"; 
		}
		return proto;  
	}, 
	
	Base64 : {
		_keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",
 
		encode : function (input) {
			var output = "";
				var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
				var i = 0;
		 
				input = this._utf8_encode(input);
		 
				while (i < input.length) {
		 
					chr1 = input.charCodeAt(i++);
					chr2 = input.charCodeAt(i++);
					chr3 = input.charCodeAt(i++);
		 
					enc1 = chr1 >> 2;
					enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
					enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
					enc4 = chr3 & 63;
		 
					if (isNaN(chr2)) {
						enc3 = enc4 = 64;
					} else if (isNaN(chr3)) {
						enc4 = 64;
					}
		 
					output = output +
					this._keyStr.charAt(enc1) + this._keyStr.charAt(enc2) +
					this._keyStr.charAt(enc3) + this._keyStr.charAt(enc4);
		 
				}
		 
				return output;
		},
	
		_utf8_encode : function (string) {
			string = string.replace(/\r\n/g,"\n");
			var utftext = "";
	 
			for (var n = 0; n < string.length; n++) {
	 
				var c = string.charCodeAt(n);
	 
				if (c < 128) {
					utftext += String.fromCharCode(c);
				}
				else if((c > 127) && (c < 2048)) {
					utftext += String.fromCharCode((c >> 6) | 192);
					utftext += String.fromCharCode((c & 63) | 128);
				}
				else {
					utftext += String.fromCharCode((c >> 12) | 224);
					utftext += String.fromCharCode(((c >> 6) & 63) | 128);
					utftext += String.fromCharCode((c & 63) | 128);
				}
	 
			}
			return utftext;
		}
	},

	saveCookie: function (name, value, day) { 
		if (day) { 
     		var date = new Date();
			date.setTime(date.getTime()+(day*24*60*60*1000));
			var expires = "; expires="+date.toGMTString();
		}
		else expires = ""; 
		document.cookie = name + "=" + escape(value) + expires+"; path=/";
	}, 
	
	loadCookie: function (name) {
		current_cookie = document.cookie + ""; 
		deb = current_cookie.indexOf( name + "=" );
		if (deb >= 0) {
			deb += name.length + 1; 
			fin = current_cookie.indexOf(";",deb);
			if (fin < 0) fin = current_cookie.length; 
			return unescape(current_cookie.substring(deb,fin)); 
		}
		return null; 
	}, 
	
	getRandomString: function (){ 
		return String((new Date()).getTime()).replace(/\D/gi,'') 
	}, 

	track: function () {
		informer = document.getElementById(this.informer); 	
		if (informer != null) {
			informer.src = this.request + "&r=" + this.getRandomString(); 
		}
	}, 
	
	run: function(SectionId, TagId, CatId) { // CG20130701
		url = document.location + ""; 
		params = this.getParams(url);
		protocol = this.getProtocol(url) + "://";  
		tracking_mode = 1; 
		
		tracking_value = null;
		if ( params[this.tracking_param] != null) {
			tracking_value =  params[this.tracking_param]; 
			this.saveCookie(this.cookie_name, tracking_value, this.cookie_timeout) ;
			tracking_mode = 0;
		}
		else if (tracking_value == null) {
			tracking_value = this.loadCookie(this.cookie_name);
		}

		if (tracking_value != null) {
			document.write("<img id='" + this.informer + "' src='' style='display:none;'/>");
			url = this.deleteParam( url , this.tracking_param );
			
			/* ================================================
			NME add CG20130701
			/!\ To use with nme-webtracking plugin
			================================================ */			
			url = url + "&SectionId=" + SectionId;
			url = url + "&TagId=" + TagId;
			url = url + "&CatId=" + CatId;
			/* ================================================
			/end NME add
			================================================ */
			
			this.request = protocol + this.listener + 
						"?id="+ tracking_value + 
						"&purl=" + this.Base64.encode(url) + 
						"&site=" + this.siteid +
						"&os=" + PROBANCE_browser_detect.OS +
						"&br=" + PROBANCE_browser_detect.browser + 
						"&ver=" + PROBANCE_browser_detect.version +
						"&mod=" + tracking_mode;

			setTimeout("PROBANCE_tracker.track()", 100);
		}
	}
};

/* ==================================================================
The PROBANCE_tracker.run is now called by the nme-webtracking plugin
=================================================================== */ 
// PROBANCE_tracker.run();
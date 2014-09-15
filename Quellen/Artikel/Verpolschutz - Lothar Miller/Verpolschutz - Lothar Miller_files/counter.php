		if (!fcupimg1195084) {
			document.write('<img id="fcup1195084" />');
			var fcupimg1195084 = document.getElementById('fcup1195084');
		}
			
	fcupimg1195084.style.width = "1px";
	fcupimg1195084.style.height = "1px";
	fcupimg1195084.style.position = "absolute";
	fcupimg1195084.style.left = "-1000px";
	
	var ref = document.referrer;
try {
	if ( self != top && top &&  top.document && top.document.referrer) {
		var ref = top.document.referrer;
	}
}
catch (e) {
	var ref = 'frameerror';
}

var fcflv = 0;
var fcpl = new Array();
	for(var x=0;x < navigator.plugins.length;x++)
	{
		var nn = navigator.plugins[x].name.toLowerCase();
		if(nn.indexOf("shockwave flash") != -1)		fcflv = navigator.plugins[x].description.substr(16,1);
		if(nn.indexOf("windows media") != -1)		fcpl["wm"] = 1;
		if(nn.indexOf("quicktime") != -1)		fcpl["qt"] = 1;
		if(nn.indexOf("real") != -1)			fcpl["rp"] = 1;
		if(nn.indexOf("svg") != -1)			fcpl["sv"] = 1;
		if(nn.indexOf("acrobat") != -1)			fcpl["pd"] = 1;
		if(nn.indexOf("pdf") != -1)			fcpl["pd"] = 1;
		if(nn.indexOf("reader") != -1)			fcpl["pd"] = 1;
		if(nn.indexOf("djvu") != -1)			fcpl["djvu"] = 1;
		if(nn.indexOf("vlc") != -1)			fcpl["vlc"] = 1;
		if(nn.indexOf("mplayer") != -1)			fcpl["mplayer"] = 1;
		if(nn.indexOf("silverlight") != -1 || nn.indexOf("moonlight") != -1)		 {
            fcpl["ag"] = 1;
            if(navigator.plugins[x].description.substring(0,1)=="2")
                fcpl["ag2"] = 1;
        }
	}
		fcpl["sv"] = 1;
	var fcnow = new Date();
var fcutime = Math.round(fcnow.getTime()/1000); // Math.round(fcnow.getTime()/1000);
var fctimezone = fcnow.getTimezoneOffset();


var fcAsSrc = 'http://fc.webmasterpro.de/as.php?id=1195084&visitorId=8563533&stime=1410777303';
fcAsSrc += "&ref=" + escape(ref) + "&p_ja=" + navigator.javaEnabled() + "&resx=" + screen.width + "&resy=" + screen.height + "&utime=" + fcutime + "&timezone=" + fctimezone;
for (var x in fcpl) {
	fcAsSrc += "&p_" + x + "=1";
}
fcAsSrc += "&p_fl=" + fcflv;



fcupimg1195084.src = fcAsSrc;
	var fctimer=60;
	function updateTime1195084() {
		var randomNumber = Math.round( Math.random()*999999 );
		fcupimg1195084.src = 'http://fc.webmasterpro.de/counter_time.php?visitorId=8563533&id=1195084&rnd=' + randomNumber;
		fctimer=fctimer+30;
		window.setTimeout("updateTime1195084()", 1000 * fctimer );
	}
	window.setTimeout("updateTime1195084()", 1000 * 60);
	
document.write('<object type="application/x-shockwave-flash" data="http://fc.webmasterpro.de/styles/bac0.swf?name=lothar.miller&amp;c=549043&amp;w=200&amp;h=30&amp;l=US&amp;p=0.0.false,03.50.0.0.0.100.20.0x003366,18.-100.-15,03.8&amp;bgUrl=" style="width:200px; height:30px;"><param name="movie" value="http://fc.webmasterpro.de/styles/bac0.swf?name=lothar.miller&amp;c=549043&amp;w=200&amp;h=30&amp;l=US&amp;p=0.0.false,03.50.0.0.0.100.20.0x003366,18.-100.-15,03.8&amp;bgUrl=" />  <param name="wmode" value="transparent" /> <param name="bgcolor" value="#ffffff" /> <param name="scale" value="exactfit" /> 549043 </object>');
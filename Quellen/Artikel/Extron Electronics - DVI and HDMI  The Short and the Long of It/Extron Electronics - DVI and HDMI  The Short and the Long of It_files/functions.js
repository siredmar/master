var aTabDesc = new Array(6);
var galleryThumbIdxSelected;

var httpsArray = [];
httpsArray.push("/password.asp");
httpsArray.push("/login");
httpsArray.push("/checkout");
httpsArray.push("/processorder.asp");
httpsArray.push("/confirm.asp");
httpsArray.push("/secure");
httpsArray.push("/product/configlogin.aspx"); // This script includes /login/login.ascx which enforces https inside. Without this entry, an endless redirect between http/https occurs.
var loc = window.location.href;
var desiredProtocol = "http:";
for (var i in httpsArray) {
	if (loc.indexOf(httpsArray[i]) > 0)
		desiredProtocol = "https:";
}
if (window.location.protocol != desiredProtocol)
	window.location.href = desiredProtocol + loc.substring(loc.indexOf(":")+1, loc.length);

function MM_swapImgRestore(){
	var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}
function MM_preloadImages(){
	var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
	var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
	if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}
function MM_findObj(n,d){
	var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
	d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
	if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
	for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document); return x;
}
function MM_swapImage(){
	var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   	if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}
function PopupWindow(url,name,width,height,prop){
	var winl = (screen.width - width) / 2;
	var wint = (screen.height - height) / 2;
	if(prop==null){prop = 'toolbar=no,menubar=no,location=no,directories=no,status=no,scrollbars=yes,resizable=yes';}
	//alert(winl + ' ' + wint);
	win = window.open(url, name, prop+',width='+width+',height='+height+',top='+wint+',left='+winl);
	if(parseInt(navigator.appVersion) >= 4){win.focus();} 
}
function PopupFlashWindow(url,name,width,height,prop)
{
	//PopupFlashWindow(sURL, sName, 780,560,"resizable=no,toolbar=no,menubar=no,location=no,directories=no,status=no,scrollbars=no")
	if(width==null){width = 780; height = 560;}
	var winl = (screen.width - width) / 2;
	var wint = (screen.height - height) / 2;
	if(prop==null){prop = 'toolbar=no,menubar=no,location=no,directories=no,status=no,scrollbars=no,resizable=yes';}
	var newWin = window.open('', 'FlashWindow', prop+',width='+width+',height='+height+',top='+wint+',left='+winl);
	if(parseInt(navigator.appVersion) >= 4){newWin.focus();} 
	var tmp = newWin.document;
	var flashfix = "<script type='text/javascript' src='/inc/flashfix.js'></"+"script>";
	tmp.write("<html><head><title>Extron Electronics</title>");
	tmp.write("</head><body topmargin='0' leftmargin='0' rightmargin='0' bottommargin='0'>")
	tmp.write("<object classid='clsid:d27cdb6e-ae6d-11cf-96b8-444553540000' codebase='http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=7,0,0,0' width='100%' height='100%' scale='showall'><param name='movie' value='" + url + "' /><param name='quality' value='high' /><embed src='" + url + "' quality='high' width='100%' height='100%' name='" + url + "' scale='showall' align='middle' type='application/x-shockwave-flash' pluginspage='http://www.macromedia.com/go/getflashplayer' /></object>");
	tmp.write(flashfix + "</body></html>");
	tmp.close();
}
function EDemoPopupWindow(sURL, sName, bResize){
	if (bResize)
		window.open(sURL, sName, "toolbar=no,menubar=no,location=no,directories=no,status=no,scrollbars=no,resizable=yes,width=400,height=300")
	else
		window.open(sURL, sName, "toolbar=no,menubar=no,location=no,directories=no,status=no,scrollbars=no,resizable=no,width=400,height=300")
}
function displaybar(nWhich){
	var oTabDescription = document.getElementById("TabDescription")
	if(oTabDescription){
		oTabDescription.innerHTML = aTabDesc[nWhich];
	}
}

function FormatNumberDisplay(value, useDecimalPoint, showThousandsSep) {
    if (arguments.length == 2) { showThousandsSep = true; }     // if thousandsSep parameter not supplied it is assumed that it should be added
    var formattedResult = "";
    var thousandsSeparator = ",";   // default English notation
    var decimalSeparator = ".";     // default English notation

    if (useDecimalPoint == false) {
        thousandsSeparator = ".";
        decimalSeparator = ",";
    }

    value = String(value);  // convert to string

    // separate whole number and fraction
    x = value.split('.');
    x1 = x[0];  // whole number
    x2 = x.length > 1 ? decimalSeparator + x[1] : "";   // fraction

    // add thousands separator to whole number
    if (showThousandsSep == true) {
        var myRegExp = /(-?\d+)(\d{3})/;
        while (myRegExp.test(x1)) {
            x1 = x1.replace(myRegExp, '$1' + thousandsSeparator + '$2');
        }
    }

    formattedResult = x1 + x2;  // re-combine whole number and fraction
    return formattedResult;
}

function FormatNumberInput(value, useDecimalPoint) {
    // format number so that it can be used for calculations (remove thousands separator and format decimal mark)
    var formattedResult = "";
    value = String(value);
    if (useDecimalPoint == false) {
        formattedResult = value.replace(/[.]/g, "").replace(/[,]/g, ".");
    }
    else {
        formattedResult = value.replace(/[,]/g, "")
    }
    return formattedResult;
}

function FormatPage(){
	var nCurrWindowHeight; nHeaderFooterHeight = 236;
	if (document.all) {nCurrWindowHeight = document.body.offsetHeight;}
  	else{nCurrWindowHeight = document.body.clientHeight;}
  	if (nCurrWindowHeight > nHeaderFooterHeight) { document.getElementById("MainBodyContent").style.height = nCurrWindowHeight - nHeaderFooterHeight + 'px';}
  	
}
function GetQueryString(key){
    var query = window.location.search.substring(1);
    var vars = query.split("&");
    for (var i=0;i<vars.length;i++){
        var pair = vars[i].split("=");
        if (pair[0] == key){
            return pair[1];
        }
    }
    return "";
} 
function IsValidEmail(sEmailAdd){
	var sPattern = unescape("%5E%28%5B%5Ct%5Cx20%5D*%5B%21%23-%27%5C*%5C+%5C-%2F-9%3D%5C%3FA-Z%5C%5E-%7E%5D+%28%5C.%5B%21%23-%27%5C*%5C+%5C-%2F-9%3D%5C%3FA-Z%5C%5E-%7E%5D+%29*%7C%22%5B%5Cx01-%5Cx09%5Cx0B%5Cx0C%5Cx0E-%5Cx21%5Cx23-%5Cx5B%5Cx5D-%5Cx7F%5D*%22%29%40%28%28%5B-a-zA-Z0-9%5D*%5Ba-zA-Z0-9%5D%5C.%29+%5Ba-zA-Z%5D%7B2%2C%7D%7C%5C%5B%28%28%5B0-9%5D%3F%5B0-9%5D%7C1%5B0-9%5D%5B0-9%5D%7C2%5B0-4%5D%5B0-9%5D%7C25%5B0-5%5D%29%5C.%29%7B3%7D%28%5B0-9%5D%3F%5B0-9%5D%7C1%5B0-9%5D%5B0-9%5D%7C2%5B0-4%5D%5B0-9%5D%7C25%5B0-5%5D%29%5C%5D%29%24") //Notes: this string is equivalent to "^([\t\x20]*[!#-'\*\+\-/-9=\?A-Z\^-~]+(\.[!#-'\*\+\-/-9=\?A-Z\^-~]+)*|"[\x01-\x09\x0B\x0C\x0E-\x21\x23-\x5B\x5D-\x7F]*")@(([-a-zA-Z0-9]*[a-zA-Z0-9]\.)+[a-zA-Z]{2,}|\[(([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3}([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\])$". Had a problem using this string in FireFox. Using unescape() seems to fix the problem.
	var oRegEx = new RegExp(sPattern);
	if(oRegEx.test(sEmailAdd)){return true;}
	return false;
}

function HighlightItem(pageType, prodType, subType, prodID, multiTitleProdIndex, prodCatID){
} // this version is only used when a product section page is loaded without the product sidebar


function initModal(selector, social, modal, width, height, horizPaddg) {
    // selector has to be set in order for the modal to show
    var socialTools = false;    // can be true/false/html
    var modalClose = true;       // if set to true, only the close button will close the window
    var w = 500;    // is default in Prettyphoto
    var h = 344;    // is default in Prettyphoto    
    var horizontalPaddg = 20;   // is default in Prettyphoto
    if (social != undefined) { socialTools = social; }
    if (modal != undefined) { modalClose = modal; }
    if (width != undefined) { w = width; }
    if (height != undefined) { h = height; }
    if (horizPaddg != undefined) { horizontalPaddg = horizPaddg; }
    $(selector).prettyPhoto({
        default_width: w,
        default_height: h,
        social_tools: social,
        opacity: 0.70,
        gallery_markup: '',
        horizontal_padding: horizontalPaddg
    });
}

function galleryThumbClick(img) {
    galleryThumbIdxSelected = img.index();  //  get index of selected thumb
    var largePath = img.attr('href');
    var caption = img.attr('title');    
    $('#photoLarge').attr({ src: largePath })
    	.stop()
    	.css({ opacity: 0.0 })
    	.animate({ opacity: 1.0 }, 1000);
    $('#photoCaption').text(caption);    
}

function openGallery(gallery) {
    // open gallery
    images = new Array();
    captions = new Array();
    titles = ['', '', ''];    // show no titles
    gallery.each(function () {
        images.push($(this).attr('href'));
        captions.push($(this).attr('title'));
    });
    $.prettyPhoto.open(images, titles, captions);

    // select image depending on which thumb is selected
    if (galleryThumbIdxSelected != undefined) {
        for (i = 0; i < galleryThumbIdxSelected; i++) { $.prettyPhoto.changePage('next'); }
    }
}

setTimeout(function () { $(document).ready(replaceFoxII); }, 1000);
function replaceFoxII() {
    var nbspTextChar = String.fromCharCode(160);
    var nonBreakingHyphen = '\u2011'; // these chars need a Unicode font to display properly in IE
    var isIE = (navigator.appVersion.indexOf("MSIE") != -1) ? true : false;
    $("#EntireBody *").each(function () {
        if ($(this).children().length == 0) {
            try {
                var thishtml = $(this).html();
                if (thishtml.indexOf('FOX II') > -1 || thishtml.indexOf('FOX&nbsp;II') > -1 || thishtml.indexOf('FOX' + nbspTextChar + 'II') > -1) {
                    $(this).html(thishtml
                                 .replace(new RegExp('FOX II', 'g'), 'FOX <span class="is-serif">II</span>')
                                 .replace(new RegExp('FOX&nbsp;II', 'g'), 'FOX&nbsp;<span class="is-serif">II</span>')
                                 .replace(new RegExp('FOX' + nbspTextChar + 'II', 'g'), 'FOX' + nbspTextChar + '<span class="is-serif">II</span>')
                                );
                }
                if (isIE) {
                    if (thishtml.indexOf(nonBreakingHyphen) > -1) {
                        $(this).html(thishtml
                                     .replace(new RegExp(nonBreakingHyphen, 'g'), '<span style="font-family: Arial Unicode MS;">&#8209;</span>')
                                    );
                    }
                }
            }
            catch (err) {
                //IE will throw "unexpected call to method or property access" in cases where there are tags it doesn't recognise.
            }
        } else {
            // Non-html-wrapped text inside a DOM element must be upgraded to an html element so we can add the is-serif span class.
            $(this).contents().filter(function () {
                return this.nodeType == 3 && ($(this).text().indexOf('FOX II') > -1 || $(this).text().indexOf('FOX' + nbspTextChar + 'II') > -1);
            }).each(function () {
                $(this).wrap('<foxii></foxii>');
            });
            if (isIE) {
                $(this).contents().filter(function () {
                    return this.nodeType == 3 && ($(this).text().indexOf(nonBreakingHyphen) > -1);
                }).each(function () {
                    $(this).wrap('<nobrhyphen></nobrhyphen>');
                });
            }
        }
    });

    $('foxii').each(function () {
        $(this).html(
                $(this).html()
                    .replace(new RegExp('FOX II', 'g'), 'FOX <span class="is-serif">II</span>')
                    .replace(new RegExp('FOX&nbsp;II', 'g'), 'FOX&nbsp;<span class="is-serif">II</span>')
                    .replace(new RegExp('FOX' + nbspTextChar + 'II', 'g'), 'FOX' + nbspTextChar + '<span class="is-serif">II</span>')
            );
    });

    if (isIE) {
        $('nobrhyphen').each(function () {
            $(this).html(
                    $(this).html()
                        .replace(new RegExp(nonBreakingHyphen, 'g'), '<span style="font-family: Arial Unicode MS;">&#8209;</span>')
                );
        });
    }
}

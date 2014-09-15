// 50recode v1.5.83, 09/12/2014 
// FILE_CHANGED_ON = "2014-09-12 13:20:43"
// This source code is property of Intel Corporation. Do not duplicate, copy or modify this file or any subfile referenced by this file. Please contact the IGHF team if you have issues.

// =====================================================
INTELNAV = window.INTELNAV || {};

INTELNAV.browserParameters = {
	str: null, 
	bN: null, 
	bM: null, 
	bV: null, 
	OS: null,
	dT: null,
	wk: null,
	bvClass: null
};

// get queryString value by name
var getParameterByName = function(name) {
	name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
	var regexS = "[\\?&]" + name + "=([^&#]*)";
	var regex = new RegExp(regexS);
	var results = regex.exec(window.location.href);
	if (results == null)
		return "";
	else
		return decodeURIComponent(results[1].replace(/\+/g, " "));
}; 

// SEGMENTED HEADER/FOOTER BEHAVIORS
var segment = "", searchFilter="", isRWD="false";

var isdebug = getParameterByName("isdebug") || false;
var isdebug_bundle_counter = 0;
hf_debug("info","START HEADERCHOOSER", "",  isdebug);

var IE11 = getParameterByName("IE11") || false;

var forcetest = getParameterByName("forcetest");

var RWD = getParameterByName("RWD") || "";
if (RWD!="") {isRWD = RWD;}
hf_debug("log","RWD="+RWD, "",  isdebug);

var isMin = ".min";
if (getParameterByName("isMin")) {isMin = ""}

// Capture jQuery before we load new so that we can restore page version
if (typeof $ != 'undefined') {	
	var tmpjQuery = $; 
	hf_debug("log","1. $.fn.jquery="+$.fn.jquery, "",  isdebug);
	hf_debug("log","1. tmpjQuery.fn.jquery="+tmpjQuery.fn.jquery, "",  isdebug);
}
if (typeof jQuery != 'undefined') {	
	var tmpjQuery2 = jQuery; 
	hf_debug("log","1. jQuery.fn.jquery="+jQuery.fn.jquery, "",  isdebug);
	hf_debug("log","1. tmpjQuery2.fn.jquery="+tmpjQuery2.fn.jquery, "",  isdebug);
}


// assign the render setting to GAAT40 to support widgets already using this object
var GAAT40 = GAAT40 || {};
GAAT40.renderSettings = {};
GAAT40.renderSettings = INTELNAV.renderSettings;
var HeaderType = "";

// If you set this to "true" it will force the header to render the Legacy Design.
var RollBack = false;

// These are the LOCALES that framework supports. These can be more than a specific design supports
var ActiveCulturesFramework = "ar_AE|ar_EG|ar_SA|ar_XR|cs_CZ|de_DE|en_AP|en_AU|en_EU|en_HK|en_IE|en_IN|en_IR|en_MY|en_NZ|en_PH|en_PK|en_SA|en_SG|en_UK|en_US|en_XA|en_XE|en_XR|en_ZA|es_ES|es_LA|es_MX|es_XL|fr_FR|he_IL|hi_IN|hu_HU|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ro_RO|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW";

// These are the LOCALES the legacy desgin supports
var ActiveGeosLegacy = "ar_AE|ar_EG|ar_SA|ar_XR|cs_CZ|de_DE|en_AP|en_AU|en_EU|en_HK|en_IE|en_IN|en_IR|en_MY|en_NZ|en_PH|en_PK|en_SA|en_SG|en_UK|en_US|en_XA|en_XE|en_XR|en_ZA|es_ES|es_LA|es_MX|es_XL|fr_FR|he_IL|hi_IN|hu_HU|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ro_RO|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW";

// These are the LOCALES the uNav1 design supports
var ActiveCultures = "de_DE|en_AP|en_AU|en_EU|en_HK|en_IE|en_IN|en_IR|en_MY|en_SA|en_NZ|en_PH|en_PK|en_SG|en_UK|en_US|en_XA|en_XE|en_XR|en_ZA|es_ES|es_LA|es_MX|es_XL|fr_FR|hi_IN|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW";

// These are the LOCALES the Menu2 (uNav2) design supports
var ActiveCulturesMenu2 = "ar_XR|de_CH|de_DE|en_AU|en_CA|en_EU|en_IE|en_IN|en_MY|en_PH|en_UK|en_US|en_XA|en_XR|en_ZA|es_ES|es_XL|fr_CA|fr_FR|hi_IN|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW|en_AP|en_HK|en_IR|en_NZ|en_PK|en_SA|en_XE|es_LA|es_MX|hu_HU|ro_RO|cs_CZ";
// en_AP|en_HK|en_IR|en_NZ|en_PK|en_SA|en_XE|es_LA|es_MX|hu_HU|ro_RO|cs_CZ get replaced during load.Listed because they get replaced by active locals.
// de_CH|en_CA|ca_FR  -these are not yet supported by the framework.
// en_SG - not supported by Menu2 but is in framework
// ar_XR is being made ready

// These are the LOCALES the Menu3 (uNav3) design supports
var ActiveCulturesMenu3 = "ar_XR|de_DE|en_AU|en_CA|en_EU|en_IE|en_IN|en_MY|en_PH|en_UK|en_US|en_XA|en_XR|en_ZA|es_ES|es_XL|fr_CA|fr_FR|he_IL|hi_IN|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW|en_AP|en_HK|en_IR|en_NZ|en_PK|en_SA|en_XE|es_LA|es_MX|hu_HU|ro_RO|cs_CZ"; 
// en_AP|en_HK|en_IR|en_NZ|en_PK|en_SA|en_XE|es_LA|es_MX|hu_HU|ro_RO|cs_CZ get replaced during load.Listed because they get replaced by active locals.
// de_CH|en_CA|ca_FR  -these are not yet supported by the framework.
// en_SG - not supported by Menu2 but is in framework

// Only These GEOS can render RWD, else fall back to base desktop mode.
var ActiveCulturesMenu3_RWD = "en_US"; 


// Paths to the entrypoint JavaScript to start the loading of the respective header
INTELNAV.headerPaths = {
	recode40 : {
				devfilePath: "/sites/sitewide/hat/40recode-data/assets/js/assetLoader.js",
				devDomain: "http://proto-cps.cps.intel.com",
				prodfilePath: "/sites/sitewide/hat/40recode-data/assets/js/assetLoader.js",
				prodDomain: "http://www.intel.com"
	},
	recode50 : {
				unavdevPathHttp: "http://pfcqcontent.cps.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavtestPathHttp: "http://tstcqcontent.cps.intel.com/etc/designs/intel/us/en/js/recode50.js", 
				unavtestPathHttps: "https://tstcqcontent-ssl.cps.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavdayonePathHttp: "http://gctpcqapp01.ed.cps.intel.com:4503/etc/designs/intel/us/en/js/recode50.js", 
				unavdayonePathHttps: "https://gctpadisp2w01.cps.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavhotfixPathHttp: "http://qfcqcontent.cps.intel.com:4503/etc/designs/intel/us/en/js/recode50.js", 
				unavhotfixPathHttps: "https://qfcqcontent.cps.intel.com:4503 /etc/designs/intel/us/en/js/recode50.js",
				unavpreviewPathHttp: "http://cqpreview.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavpreviewPathHttps: "https://cqpreview-ssl.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavprodPathHttp: "http://www.intel.com/etc/designs/intel/us/en/js/recode50.js",
				unavprodPathHttps: "https://www-ssl.intel.com/etc/designs/intel/us/en/js/recode50.js",
				GAtest1PathHttp: "http://www.intel.com/etc/designs/intel/us/en/js/recode50.www.js",
				GAtest1PathHttps: "https://www-ssl.intel.com/etc/designs/intel/us/en/js/recode50.www.js",
				GAtest2PathHttp: "http://gwo-dev.intel.com/jgarber/CQ/recode50.js",
				GAtest2PathHttps: "https://gwo-dev.intel.com/jgarber/CQ/recode50.js",
				GAtest3PathHttp: "http://trphippx-desk2:4503/etc/designs/intel/us/en/js/recode50.js",
				GAtest3PathHttps: "http://trphippx-desk2:4503/etc/designs/intel/us/en/js/recode50.js",
				GAtest4PathHttp: "http://incqcontent.cps.intel.com:4503/etc/designs/intel/us/en/js/recode50.dev.js",
				GAtest4PathHttps: "http://incqcontent.cps.intel.com:4503/etc/designs/intel/us/en/js/recode50.dev.js",
				//unav2devPathHttp: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master"+segment+".js",
				//unav2devPathHttps: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master"+segment+".js",
				unav2devPathHttp: "http://gadev/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2devPathHttps: "https://gadev/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				//unav2devPathHttp: "http://gadev/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				//unav2devPathHttps: "https://gadev/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				//unav2qaPathHttp: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				//unav2qaPathHttps: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				//unav2qaPathHttp: "http://gadev/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				//unav2qaPathHttps: "https://gadev/sites/sitewide/HAT/50recode/designs/unav2-newsearch/assets/js/assetLoader-master.js",
				unav2qaPathHttp: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2qaPathHttps: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2prevPathHttp: "http://preview-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2prevPathHttps: "http://preview-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2prodPathHttp: "http://www.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2prodPathHttps: "https://www-ssl.intel.com/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2stagePathHttp: "http://int-gaat.mrmpweb.co.uk/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2stagePathHttps: "https://int-gaat.mrmpweb.co.uk/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader-master.js",
				unav2relPathHttp: "http://int-gaat-dev/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader.js",
				unav2relPathHttps: "https://int-gaat-dev/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader.js",
				unav2testPathHttp: "http://int-unav2-testharness.mrmpweb.co.uk/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader.js",
				unav2testPathHttps: "https://int-unav2-testharness.mrmpweb.co.uk/sites/sitewide/HAT/50recode/designs/unav2/assets/js/assetLoader.js",				
				footer1devPathHttp: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				footer1devPathHttps: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				footer1prevPathHttp: "http://preview-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				footer1prevPathHttps: "http://preview-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				footer1prodPathHttp: "http://www.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				footer1prodPathHttps: "https://www-ssl.intel.com/sites/sitewide/HAT/50recode/designs/footer1/assets/js/assetLoader-master.js",
				unav3devPathHttp: "http://gadev/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js",
				unav3devPathHttps: "https://gadev/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js",
				unav3qaPathHttp: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js",
				unav3qaPathHttps: "http://proto-cps.cps.intel.com/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js",
				unav3prodPathHttp: "http://www.intel.com/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js",
				unav3prodPathHttps: "https://www-ssl.intel.com/sites/sitewide/HAT/50recode/designs/unav3/assets/js/assetLoader"+isMin+".js"
	}
};

INTELNAV.assetPathHttp = "http://www.intel.com";
INTELNAV.assetPathHttps = "https://www-ssl.intel.com";
INTELNAV.assetServer = "http://www.intel.com";


// Look Up Table of URLs to take the 50recode (uNav)
// regex = regular expression to define path
// defaultPathHttp = override of default path.
// isVisible:"footeronly" = show footer only and hide the header - field is optional
// searchFilter:[{sf_var:"topicarea", sf_val:"Support"}] = add the hidden field to the search form for filtered search results - field is optional and can pass more than one
INTELNAV.UrlLookUpTable = {
	urls: [
// ACADEMY.INTEL.COM and REGALEME.INTEL.COM
		// (academy.intel.com|)
		{
			site: "academy.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		// (regalame.intel.com|)
		{
			site: "regalame.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// ARABSCIENCE.INTEL.COM
		// (arabscience.intel.com|hazmieh.net/ISCArab)
		{
			site: "arabscience.intel.com|arabscience-cdn.intel.com|nata3alam.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "hazmieh.net/ISCArab",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// ARK.INTEL.COM
		// (ark.intel.com|tst1ark.sym.cps.intel.com|mark.intel.com|tst1mark.sym.cps.intel.com|odata.intel.com|odata-dev.intel.com|odata-tst.intel.com)
		{
			site: "(ark|mark|spark|ark-care|odata).intel.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(ark-dev|ark-tst|mark-dev).intel.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(mark-tst|care-tst|spark-qa|spark-dev2|odata-dev|odata-tst).intel.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// BLOGS
		// (staging-blogs.intel.com|blogs.intel.com)
		//			site: "(staging-blogs.intel.com|blogs.intel.com(?!/embedded))",
		{
			site: "blogs.intel.com|scoop.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "staging-blogs.intel.com|staging-scoop.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// CORPREDIRECT
		{
			site: "corpredirect.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttp2, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// C2A.INTEL.COM
		// (c2a.intel.com|c2a-dev.intel.com|c2a-tst.intel.com)
		{
			site: "(c2a-dev|c2a-tst).intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// GAMEHERO.INTEL.COM 
		// (gamehero.intel.com|gamehero.intel.com.br|gamehero.intel.la|gamehero.intel.mx|gamehero.intel.co )
		{
			site: "gamehero.intel.com|www.gamehero.intel.com|intel.753258.hive.com.br|192.168.1.15",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// DOWNLOADCENTER.INTEL.COM (de_DE|en_US|es_MX|fr_FR|it_IT|ja_JP|ko_KR|pt_BR|ru_RU|zh_CN|zh_TW)
		// (downloadcenter.intel.com|downloadcenter-dev.intel.com|downloadcenter-tst.intel.com|downloadcenter-prd.intel.com)
		{
			site: "(downloadcenter|downloadcenter-prd|Downloadcenter01-prd|Downloadcenter02-prd).intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(downloadcenter-dev|downloadcenter-tst).intel.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// EDC.INTEL.COM (en_us, zh_cn, ja_jp, zh_tw, ru_ru, ko_kr, es_xl, pt_br)
		// Menu2 for preview-edc.intel.com and edc.intel.com
		{
			site: "edc.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "preview-edc.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// ICLP SERVICE PROVIDER
// INTELTECHNOLOGYPROVIDER.COM
		// Menu2 for (emea-points.inteltechnologyprovider.com|asmolar-points.inteltechnologyprovider.com|asmonar-points.inteltechnologyprovider.com|ijkk-points.inteltechnologyprovider.com|apac-points.inteltechnologyprovider.com|prc-points.inteltechnologyprovider.com)
		/*{
			site: "(ijkk)-points.inteltechnologyprovider.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		{
			site: "uat-flex-membersite-asmo-na.intelflexplus.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// IKNOW.INTEL.COM
		{
			site: "iknow.intel.com|iknow-tst.intel.com|iknow-psp.intel.com|iknow-dev.intel.com|iknow-poc.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// INTELSTUDIOS.EDGESUITE.NET (en_us)
		// Menu2 for intelstudios.edgesuite.net
		{
			site: "intelstudios.edgesuite.net",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// INTC.INTEL.COM (en_US)
		// (www.intc.com|www.intc.com/?mode=7-14-11-RML-495550-25089112-9DC40AC04F3739D33C903C23FFF8C5A48F6BC0BF0DA612B2D2A3D00A29CA545E)
		{
			site: "(www.|)intc.com|intc.client.shareholder.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		/*{
			site: "(www.|)intc.com/?mode=7-14-11-RML-495550-25089112-9DC40AC04F3739D33C903C23FFF8C5A48F6BC0BF0DA612B2D2A3D00A29CA545E",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
//IMCT.INTEL.COM (en_US)
		/* imct.intel.com|imct01.sym.cps.intel.com|imct02.sym.cps.intel.com|imct03.sym.cps.intel.com|imct04.sym.cps.intel.com */
		/* tst1imct.sym.cps.intel.com|tst1imct01.sym.cps.intel.com|tst1imct02.sym.cps.intel.com| */
		/* dev1imct01.sym.cps.intel.com */
		{
			site: "imct.intel.com|imctlocal|imct-stg.intel.com|imct-tst.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
//INTHMEMSW01
		{
			site: "inthmemsw01|inthmemsw01.ccr.corp.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
//ITOASIA.INTEL.COM
		{
			site: "iotasia.intel.com/mobile|iotasia.emtgup.com/mobile",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", RWD:"true"
		},
		{
			site: "iotasia.intel.com|iotasia.emtgup.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// (EOL'd 4/29/2014 - Confirmed by Caroline Spooner) *.LIVEPERSON.NET (FAQ for Corporate and Education) (en_US)
		// (server.iad.liveperson.net|sales.liveperson.net)
		/*{
			site: "(server.iad|sales).liveperson.net",
			goodCulture: ActiveCultures,
			design: "2",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
// LOCATE.INTEL.COM ()
		// (locate.intel.com|premiertest.apnet.ru)
		{
			site: "locate.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "premiertest.apnet.ru|premierstaging.apnet.ru",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// MCAFEE ()
		// (http://dskarnaw-desk1.amr.corp.intel.com/MRenSite/ | https://stg1mcafeew01.cps.intel.com/submission | https://mcafeerevenueshare-tst1.intel.com/ | https://mcafeerevenueshare.intel.com/)
		{
			site: "mcafeerevenueshare.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "dskarnaw-desk1.amr.corp.intel.com/MRenSite|stg1mcafeew01.cps.intel.com/submission|mcafeerevenueshare-tst1.intel.com|stg1mcafeew01.cps.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
//NEXTGENERATIONCENTER.COM
		// (www.nextgenerationcenter.com|innfy.azurewebsites.net)
/*		{
			site: "(innfy.azurewebsites.net)",
			goodCulture: ActiveCultures,
			design: "1",
			devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
// OAKTREE SITES (en_US)
		// (ipt.intel.com|staging.ipt.intel.oaktree.com)
		{
			site: "ipt.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "staging.ipt.intel.oaktree.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "iptprd.odce.intel.net|172.25.111.202",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// OLE.INTEL.COM
		//(ole.intel.com)
		{
			site: "(ole.intel.com|larapps2.intel.com|ole.intel.com.br|ole.intel.com.mx|ole.intel.com.co|ole.intel.com.cl|ole.intel.com|ole.intel.la)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// PLATFORM.INTEL.COM
		//(platformsw-dev.fm.intel.com|platformsw.intel.com)
		{
			site: "(platformsw.intel.com|platformsw-preprod.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(platformsw-dev.fm.intel.com|fm6sdev009.fm.intel.com:701|platformsw-intnew.intel.com|platformsw-devnew.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// PROCESSORMATCH.INTEL.COM
		//(stg1compdbw01.cps.intel.com|stg1compdbvenw01.cps.intel.com|tst1compdb2w.cps.intel.com|tst1compdb2w01.cps.intel.com|tst1compdb2w02.cps.intel.com|tst1cdbmbv2w.cps.intel.com)
		//(processormatch.intel.com|prd1compdb2w.cps.intel.com|prd1compdb2w01.cps.intel.com|prd1compdb2w02.cps.intel.com)
		//(CompDBMBVendor.intel.com|prd1cdbmbv2w.cps.intel.com|prd1cdbmbv2w02.cps.intel.com)
		{
			site: "(processormatch.intel.com|prd1cdb2w.cps.intel.com|prd1compdb2w01.cps.intel.com|prd1compdb2w02.cps.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(CompDBMBVendor.intel.com|prd1cdbmbv2w.cps.intel.com|prd1cdbmbv2w02.cps.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(stg1compdbw01.cps.intel.com|stg1compdbvenw01.cps.intel.com|tst1cdb2w.cps.intel.com|tst1compdb2w01.cps.intel.com|tst1compdb2w02.cps.intel.com|tst1cdbmbv2w.cps.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// PRCAPPZONE.INTEL.COM (zh_CN)
		{
			site: "prcappzone.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		/*{
			site: "www.intel.com/cn/ultrabook",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
// REGISTRATIONCENTER.INTEL.COM (zh_CN)
		//(registrationcenter.intel.com|qs2-registrationcenter.cps.intel.com|dev-registrationcenter04.cps.intel.com|nnvappsea02.ccr.corp.intel.com)
		{
			site: "registrationcenter.intel.com|qs2-registrationcenter.cps.intel.com|dev-registrationcenter04.cps.intel.com|ps-registrationcenter01.cps.intel.com|nnvappsea02.ccr.corp.intel.com|tst1regcenw01.cps.intel.com|tst1regcenw02.cps.intel.com|tst1regceniw01.cps.intel.com|tst1regceniw02.cps.intel.com|stg1regcenw01.cps.intel.com|stg1regcenw02.cps.intel.com|stg1regcenw03.cps.intel.com|stg1regcenw04.cps.intel.com|stg1regcenw05.cps.intel.com|stg1regcenw06.cps.intel.com|stg1regceniw01.cps.intel.com|stg1regcenin.cps.intel.com|stg1regcenps.cps.intel.com|prd1regcenw01.cps.intel.com|prd1regcenw02.cps.intel.com|prd1regceniws01.cps.intel.com|prd1regceniws02.cps.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// THE NEW GROUP SITES
		//(staging.intelintelligentsystemsalliance.tngdev.com|production.intelintelligentsystemsalliance.tngdev.com|intelintelligentsystemsalliance.com)
		{
			site: "(www.|)intelintelligentsystemsalliance.com|iotsolutionsalliance.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(staging.intelintelligentsystemsalliance.tngdev.com|production.intelintelligentsystemsalliance.tngdev.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// SALES FORCE SITES
		//Intel Customer Support
		//(isvcdev05-intelcustomersupport.cs11.force.com/intelcustomersupport|isvcqa-intelcustomersupport.cs11.force.com/intelcustomersupport|http://intelcustomersupport.force.com/)
		{
			site: "(isvcdev05-intelcustomersupport.cs11.force.com/intelcustomersupport|isvcqa-intelcustomersupport.cs11.force.com/intelcustomersupport|intelcustomersupport.force.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// SEARCH.INTEL.COM
		// (search.intel.com|tst1intelsearch.sym.cps.intel.com)
		/*{
			site: "(search.intel.com|tst1intelsearch.sym.cps.intel.com)",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
// MRM Sites
		{
			site: "(shop.mrmpweb|shop.mrmdev).co.uk",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "shoporigin.site-assets.co.uk",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(www.|www-ssl.|thailand.|)intel(.com|.ca|.cn|.de|.com.br|.co.id|.co.jp|.co.kr|.com.tr|.com.tw|.co.uk|.co.za|.com.au|.es|.eu|.fr|.hk|.ie|.in|.it|.la|.my|.nl|.nz|.ph|.pk|.pl|.ru|.se|.sg|.ua|.vn)/buy",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		/*{
			site: "itmanager3.intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		{
			site: "solutionbuilder.intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(solutionbuilder-dev.intel.com|int-sb.mrmpweb.co.uk)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(asiareseller.intel.com|www.m1a.com.au)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
//SOLUTIONFINDER
		{
			site: "solutionfinder.intel.com|intel.webtradecenter.com|newwtc.cloudapp.net",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// SOLUTIONSET	
		{
			site: "(intelnewsroom|intelopenport|inteleducation|intelembedded|intelchannel|intel-openport-v7)(|.uat5).hosted.jivesoftware.com|jive7.intel.d.solutionset.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		/*{
			site: "itcentercommunity.intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		/*{
			site: "preview-itcentercommunity.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		/*{
			site: "(devjava129|devjava170|devjava171|devjava172|devjava173|devjava174).solutionset.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		//Channel
		/*{
			site: "channelcommunities.intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		//Communities		
		{
			site: "(communities.intel.com|communities.intel.co.jp)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "preview-communities.intel.com|origin-communities.intel.co.jp",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		//Education
		{
			site: "(engage.intel.com|origin-engage.intel.com|preview-engage.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		//Embedded		
		{
			site: "(embedded.communities.intel.com|origin-embedded.communities.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		//Newsroom
		{
			site: "(newsroom.intel.com|newsroom.intel.co.jp|freepress.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "preview-newsroom.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// UPIC.INTEL.COM - FORMATIVECO
		//(upic.intel.com|upic.dev.formativeco.com)
		{
			site: "(upic.intel.com|upic.dev.formativeco.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// VERIZON
		//(localhost|lcb.nuahz.com|beforeyoubuypc.intel.in|beforeyoubuypc-hi.intel.in)
		{
			site: "(lcb.nuahz.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		//(dev.udk.tngdev.com|www.udkpilot.com|uefidk.com|udkpilot.tngprojects.com|www.udkpilot.com)
		{
			site: "(uefidk.com|www.uefidk.com|uefidk.intel.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(dev.udk.tngdev.com|udkpilot.tngprojects.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		//(mission.intel.com) ?EOLD? 10/23/2013
		{
			site: "mission.intel.com.tw",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// eGain : intelWiDi
		{
			site: "supportkb-dev.intel.com|supportkb-localhost.intel.com|supportkb-tst.intel.com|supportkb-psp.intel.com",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", RWD:"true"
		},
		{
			site: "supportkb.intel.com|punv730b.egain.in|punv762c.egain.in|v33w2|vmdev13|punv762z.egain.in",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// eGain : servicerequestmgmt.intel.com
		{
			site: "servicerequestmgmtbench.intel.com/webticketui|DV1EIMVFM2W01.odc.intel.net|fmstwti2wv001.amr.corp.intel.com|dev-webticket.intel.com|servicerequestmgmtcons.intel.com/webticketui|servicerequestmgmt.intel.com/webticketui",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// ERM 
		{
			site: "(erpm|pre-erpm|fmt0crpm.cps|fmdvcrpm.cps|fm1cpmng3w.cps).intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// WHERE TO BUY - APAC
		/*{
			site: "(lcb|tst1mz201.sym.cps|dev1mz201.sym.cps|mz2-dev|mz2-tst).intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(wtbapac|tst1wtb01.sym.cps|dev1wtb01.sym.cps|wtb-dev|wtb-tst|wtb-psp).intel.com",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		{
			site: "(lcb.nuahz.com|lcb-sg.nuahz.com|lcb-my.nuahz.com|lcb-ph.nuahz.com|lcb-au.nuahz.com|lcb-kr.nuahz.com|lcb-th.nuahz.com|lcb-tw.nuahz.com|lcb-vn.nuahz.com|lcb-id.nuahz.com|lcb-hi.nuahz.com|lcb-en-v2.nuahz.com|lcb-in-v2.nuahz.com|lcb-au-v2.nuahz.com|bus-in.nuahz.com|bus-au.nuahz.com|bus-kr.nuahz.com|bus-tw.nuahz.com|bus-in.mrmapac.com|bus-au.mrmapac.com|bus-kr.mrmapac.com|bus-tw.mrmapac.com|lcb-hi.mrmapac.com|lcb-th.mrmapac.com|lcb-vn.mrmapac.com|lcb-tw.mrmapac.com|lcb-kr.mrmapac.com|lcb-id.mrmapac.com|lcb-au.mrmapac.com|lcb-ph.mrmapac.com|lcb-my.mrmapac.com|lcb-sg.mrmapac.com|lcb-in.mrmapac.com|smb.mrmapac.com|smb-au.mrmapac.com|smb-sg.mrmapac.com|smb-tw.mrmapac.com|smb-kr.mrmapac.com|smb-in.mrmapac.com)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(smb.intel.in|smb.intel.com.au|smb.intel.com.tw|smb.intel.co.kr|smb.intel.sg|smb.intel.my)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "locator.intel.(sg|in|com.au|my|ph|com.tw|co.kr|co.id|vn)|locator.thailand.intel.com",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "business.intel.(in|com.au|com.tw|co.kr)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "beforeyoubuypc(.intel.sg|.intel.com.au|.intel.in|.intel.my|.intel.ph|.intel.co.kr|.intel.com.tw|.thailand.intel.com|.intel.co.id|.intel.vn|-hi.intel.in)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "buypc.intel.com.tw",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// PORTAL (WWW.INTEL.COM/P/)
		//EMBEDDED pages
		/*{
			site: "ts1canary-prv-wip.cps.intel.com/p/\\w+/embedded",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		//SUPPORT pages
		//(ts1canary.cps.intel.com/p/xx_xx/support, fm1canary-prv-wip.cps.intel.com/p/xx_xx/support, fm1canary.cps.intel.com/p/xx_xx/support, www.intel.com/p/xx_xx/support)
		{
			site: "(ts1canary|ts1canary-prv-wip).cps.intel.com/p/\\w+/support",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp: INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps: INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", searchFilter:[{sf_var:"topicarea", sf_val:"Support"}]
		},
		{
			site: "(|www|fm1canary.cps|fm1canary-prv-wip.cps).intel.com/(p/\\w+/support|support|jp/support)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp: INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps: INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", searchFilter:[{sf_var:"topicarea", sf_val:"Support"}]
		},
// CPS (WWW.INTEL.COM)
		{
			site: "(proto)-cps.cps.intel.com/support",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", searchFilter:[{sf_var:"topicarea", sf_val:"Support"}]
		},
		{
			site: "(preview)-cps.cps.intel.com/support",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", searchFilter:[{sf_var:"topicarea", sf_val:"Support"}]
		},
		{
			site: "(proto-cps.cps.intel.com|gadev)/sites/sitewide/HAT/TestHarness/50recode/onlyfooter2.htm",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: "", isVisible:"footeronly"
		},
		{
			site: "proto-cps.cps.intel.com/sites/sitewide/hat/testharness/50recode",
			goodCulture: ActiveCulturesMenu3,
			design: "3",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttps, devPathHttp: "",testPathHttp: "", testPathHttps:"", prevPathHttp:"", prevPathHttps:"", prodPathHttp:"", prodPathHttps:"", RWD:"true"
		},
		{
			site: "(proto-cps.cps|gwo-dev).intel.com|gadev",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""/*, searchFilter:[{sf_var:"topicarea", sf_val:"Support"}], isVisible:"footeronly"*/
		},
		
		//APAC pages
		/*{
			site: "(proto-cps.cps.|preview-cps.cps.)intel.com(/au/smb|/in/irdonline|/my/roadmap)",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(proto-cps.cps.|preview-cps.cps.)intel.com(/au/smb|/in/irdonline|/my/roadmap)",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
//SURVEY (SSL.INTEL.COM/FORM-APP)
		//(ssl.intel.com/form-app|ssl-fmt0grebe.cps.intel.com/form-app)
		{
			site: "ssl-fmt0grebe.cps.intel.com/form-app",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		/*{
			site: "(www.|www3.|)intel.com(/ja_JP/itcenter/products/catalog/|/jp/tomorrow/index.htm|/jp/tomorrow/city/howto/|/jp/tomorrow/tvcm/|/jp/tomorrow/wowfinder/)",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(www.|)intel.com/(about|ar_ae|ar_eg|ar_sa|ar_xr|cs_cz|distributed|embedded|en_ap|en_au|en_in|en_sa|en_sg|en_uk|en_us|en_xa|en_xe|en_za|es_ar|es_cl|es_co|es_la|es_mx|es_xl|europe|he_il|he_hu|id_id|inside|itcenter|ja_jp|pt_br|ro_ro|sv_se|techwonders|th|ua|vi_vn|vn|za|en_sg)",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
// CIP (WWW.INTEL.COM/CD/)
		{
			site: "(www.|www3.|)intel.com/cd/consumerportal",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(fm1heron|fm1heron.cps).intel.com/cd/(consumerportal|channel/reseller|channel/distributor)",
			goodCulture: ActiveCulturesMenu2,
			design: "2",
			defaultPathHttp:INTELNAV.headerPaths.recode50.unav2prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav2prodPathHttps, devPathHttp: "",testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
// FOR TESTING PURPOSES ONLY, ON CPS FOR GA TESTING OF HEADER PERFORMANCE
		/*{
			site: "proto-cps.cps.intel.com/support/chipsets/sb/CS-033018.htm",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(www.|www3.|)intel.com/about/test/rhc-csr5-widget-implementation.htm",
			goodCulture: ActiveCultures,
			design: "1",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "http://www.intel.com/etc/designs/intel/us/en/js/recode50.www.js", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},
		{
			site: "(www.|www3.|)intel.com/sites/sitewide/unavtest/unav.htm",
			goodCulture: "ar_AE|ar_EG|ar_SA|ar_XR|cs_CZ|de_DE|en_AP|en_AU|en_IN|en_IR|en_MY|en_PH|en_SA|en_SG|en_UK|en_US|en_XA|en_XE|en_XR|en_ZA|es_ES|es_LA|es_MX|es_XL|fr_FR|he_IL|hu_HU|id_ID|it_IT|ja_JP|ko_KR|nl_NL|pl_PL|pt_BR|ro_RO|ru_RU|sv_SE|th_TH|tr_TR|uk_UA|vi_VN|zh_CN|zh_TW",
			design: "0",
			defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""
		},*/
		{site:"localhost", goodCulture:ActiveCulturesMenu3, design:"3", defaultPathHttp:INTELNAV.headerPaths.recode50.unav3prodPathHttp, defaultPathHttps:INTELNAV.headerPaths.recode50.unav3prodPathHttp, devPathHttp:"", testPathHttp:"", testPathHttps:"", prevPathHttp:"", prevPathHttps:"", prodPathHttp:"", prodPathHttps:""},
		{site: "localhost", goodCulture: "",	design: "0", defaultPathHttp:"", defaultPathHttps:"", devPathHttp: "", testPathHttp: "", testPathHttps: "", prevPathHttp: "", prevPathHttps: "", prodPathHttp: "", prodPathHttps: ""}
	]
};


/*
custom1 (HeaderVersion) = 30recode, 40recodeA-D, INT.50recode, INT.60recode
custom2 (HeaderChosen) = LegacyDev, LegacyProd, uNavDev, uNavDevOverride, uNavTest, unNavTestOverride, uNavPreview, uNavPreviewOverride, uNavProd, uNavProdOverride
custom3 (HeaderSubNav) = gh_home, gh_business, etc.
custom4 (HeaderGEO) = GEOS
*/

// Google Analytics to detect header - PART 1
var _gaq = _gaq || [];
_gaq.push(['GAtracker._setAccount', 'UA-23487238-1']);
_gaq.push(['GAtracker._setDomainName', 'none']);
_gaq.push(['GAtracker._setAllowLinker', true]);
_gaq.push(['GAtracker._setCustomVar', 1, 'HeaderVersion', 'INT.50recode', 3]);



/* public functions*/
function indent(counter) {
	var indents="";
	for (var i = 0; i < counter; i++) {
		indents = indents+"\t"
	}
	return indents;
};
// hf_debug function
function hf_debug(method, message, obj, isdebug) {
	var color=["#FF0000", "#FFBF00", "#80FF00", "#00B359", "#00BFFF", "#265CFF", "#5C26FF", "#D24CFF" ];
	if (isdebug && typeof console != "undefined") {
		switch (method)
		{
		case "warn":
			isdebug_indent = indent(isdebug_bundle_counter);
			console.warn("%c50recode: "+isdebug_indent+message+obj, "background-color:#cc0000;color:#fff;");
			break;
		
		case "info":
			isdebug_indent = indent(isdebug_bundle_counter);
			console.info("50recode: "+isdebug_indent+message+obj);
			break;
			
		case "error":
			isdebug_indent = indent(isdebug_bundle_counter);
			console.error("50recode: "+isdebug_indent+message+obj);
			break;
		
		case "properties":
			isdebug_indent = indent(isdebug_bundle_counter);
			console.dir("50recode: "+isdebug_indent+obj);
			break;
			
		case "timestamp":
			isdebug_indent = indent(isdebug_bundle_counter);
			if (INTELNAV.jQuery_1_5_1.browser.chrome) console.timeStamp("50recode: "+isdebug_indent+message);
			break;
			
		case "timestart":
			isdebug_indent = indent(isdebug_bundle_counter);
			if (INTELNAV.jQuery_1_5_1.browser.chrome) console.time("50recode: "+isdebug_indent+message);
			break;
			
		case "timeend":
			isdebug_indent = indent(isdebug_bundle_counter);
			if (INTELNAV.jQuery_1_5_1.browser.chrome) console.timeEnd("50recode: "+isdebug_indent+message);
			break;
		
		case "bundletimestart":
			if (INTELNAV.jQuery_1_5_1.browser.webkit) {
				isdebug_indent = indent(isdebug_bundle_counter);
				console.log("%c50recode: "+isdebug_indent+"START "+message+obj, "color:"+color[isdebug_bundle_counter]+";");
				console.timeStamp("50recode: "+isdebug_indent+"START "+message);
				console.time("50recode: "+isdebug_indent+message);
				isdebug_bundle_counter+=1;
				isdebug_indent="";
			}
			break;
			
		case "bundletimeend":
			if (INTELNAV.jQuery_1_5_1.browser.webkit) {
				isdebug_bundle_counter-=1;
				isdebug_indent = indent(isdebug_bundle_counter);
				console.log("%c50recode: "+isdebug_indent+"END "+message+obj, "color:"+color[isdebug_bundle_counter]+";");
				console.timeStamp("50recode: "+isdebug_indent+"END "+message);
				console.timeEnd("50recode: "+isdebug_indent+message);
				isdebug_indent="";
			}
			break;
		
		default:
			// log method
			isdebug_indent = indent(isdebug_bundle_counter);
			console.log("50recode: "+isdebug_indent+message+obj);
			break;
		}
	}
};


var load_jquery1_5_1 = function() {
	// ! jQuery v1.5.1 jquery.com | jquery.org/license with Sizzle
	(function(a,b){function cg(a){return d.isWindow(a)?a:a.nodeType===9?a.defaultView||a.parentWindow:!1}function cd(a){if(!bZ[a]){var b=d("<"+a+">").appendTo("body"),c=b.css("display");b.remove();if(c==="none"||c==="")c="block";bZ[a]=c}return bZ[a]}function cc(a,b){var c={};d.each(cb.concat.apply([],cb.slice(0,b)),function(){c[this]=a});return c}function bY(){try{return new a.ActiveXObject("Microsoft.XMLHTTP")}catch(b){}}function bX(){try{return new a.XMLHttpRequest}catch(b){}}function bW(){d(a).unload(function(){for(var a in bU)bU[a](0,1)})}function bQ(a,c){a.dataFilter&&(c=a.dataFilter(c,a.dataType));var e=a.dataTypes,f={},g,h,i=e.length,j,k=e[0],l,m,n,o,p;for(g=1;g<i;g++){if(g===1)for(h in a.converters)typeof h==="string"&&(f[h.toLowerCase()]=a.converters[h]);l=k,k=e[g];if(k==="*")k=l;else if(l!=="*"&&l!==k){m=l+" "+k,n=f[m]||f["* "+k];if(!n){p=b;for(o in f){j=o.split(" ");if(j[0]===l||j[0]==="*"){p=f[j[1]+" "+k];if(p){o=f[o],o===!0?n=p:p===!0&&(n=o);break}}}}!n&&!p&&d.error("No conversion from "+m.replace(" "," to ")),n!==!0&&(c=n?n(c):p(o(c)))}}return c}function bP(a,c,d){var e=a.contents,f=a.dataTypes,g=a.responseFields,h,i,j,k;for(i in g)i in d&&(c[g[i]]=d[i]);while(f[0]==="*")f.shift(),h===b&&(h=a.mimeType||c.getResponseHeader("content-type"));if(h)for(i in e)if(e[i]&&e[i].test(h)){f.unshift(i);break}if(f[0]in d)j=f[0];else{for(i in d){if(!f[0]||a.converters[i+" "+f[0]]){j=i;break}k||(k=i)}j=j||k}if(j){j!==f[0]&&f.unshift(j);return d[j]}}function bO(a,b,c,e){if(d.isArray(b)&&b.length)d.each(b,function(b,f){c||bq.test(a)?e(a,f):bO(a+"["+(typeof f==="object"||d.isArray(f)?b:"")+"]",f,c,e)});else if(c||b==null||typeof b!=="object")e(a,b);else if(d.isArray(b)||d.isEmptyObject(b))e(a,"");else for(var f in b)bO(a+"["+f+"]",b[f],c,e)}function bN(a,c,d,e,f,g){f=f||c.dataTypes[0],g=g||{},g[f]=!0;var h=a[f],i=0,j=h?h.length:0,k=a===bH,l;for(;i<j&&(k||!l);i++)l=h[i](c,d,e),typeof l==="string"&&(!k||g[l]?l=b:(c.dataTypes.unshift(l),l=bN(a,c,d,e,l,g)));(k||!l)&&!g["*"]&&(l=bN(a,c,d,e,"*",g));return l}function bM(a){return function(b,c){typeof b!=="string"&&(c=b,b="*");if(d.isFunction(c)){var e=b.toLowerCase().split(bB),f=0,g=e.length,h,i,j;for(;f<g;f++)h=e[f],j=/^\+/.test(h),j&&(h=h.substr(1)||"*"),i=a[h]=a[h]||[],i[j?"unshift":"push"](c)}}}function bo(a,b,c){var e=b==="width"?bi:bj,f=b==="width"?a.offsetWidth:a.offsetHeight;if(c==="border")return f;d.each(e,function(){c||(f-=parseFloat(d.css(a,"padding"+this))||0),c==="margin"?f+=parseFloat(d.css(a,"margin"+this))||0:f-=parseFloat(d.css(a,"border"+this+"Width"))||0});return f}function ba(a,b){b.src?d.ajax({url:b.src,async:!1,dataType:"script"}):d.globalEval(b.text||b.textContent||b.innerHTML||""),b.parentNode&&b.parentNode.removeChild(b)}function _(a){return"getElementsByTagName"in a?a.getElementsByTagName("*"):"querySelectorAll"in a?a.querySelectorAll("*"):[]}function $(a,b){if(b.nodeType===1){var c=b.nodeName.toLowerCase();b.clearAttributes(),b.mergeAttributes(a);if(c==="object")b.outerHTML=a.outerHTML;else if(c!=="input"||a.type!=="checkbox"&&a.type!=="radio"){if(c==="option")b.selected=a.defaultSelected;else if(c==="input"||c==="textarea")b.defaultValue=a.defaultValue}else a.checked&&(b.defaultChecked=b.checked=a.checked),b.value!==a.value&&(b.value=a.value);b.removeAttribute(d.expando)}}function Z(a,b){if(b.nodeType===1&&d.hasData(a)){var c=d.expando,e=d.data(a),f=d.data(b,e);if(e=e[c]){var g=e.events;f=f[c]=d.extend({},e);if(g){delete f.handle,f.events={};for(var h in g)for(var i=0,j=g[h].length;i<j;i++)d.event.add(b,h+(g[h][i].namespace?".":"")+g[h][i].namespace,g[h][i],g[h][i].data)}}}}function Y(a,b){return d.nodeName(a,"table")?a.getElementsByTagName("tbody")[0]||a.appendChild(a.ownerDocument.createElement("tbody")):a}function O(a,b,c){if(d.isFunction(b))return d.grep(a,function(a,d){var e=!!b.call(a,d,a);return e===c});if(b.nodeType)return d.grep(a,function(a,d){return a===b===c});if(typeof b==="string"){var e=d.grep(a,function(a){return a.nodeType===1});if(J.test(b))return d.filter(b,e,!c);b=d.filter(b,e)}return d.grep(a,function(a,e){return d.inArray(a,b)>=0===c})}function N(a){return!a||!a.parentNode||a.parentNode.nodeType===11}function F(a,b){return(a&&a!=="*"?a+".":"")+b.replace(r,"`").replace(s,"&")}function E(a){var b,c,e,f,g,h,i,j,k,l,m,n,o,q=[],r=[],s=d._data(this,"events");if(a.liveFired!==this&&s&&s.live&&!a.target.disabled&&(!a.button||a.type!=="click")){a.namespace&&(n=new RegExp("(^|\\.)"+a.namespace.split(".").join("\\.(?:.*\\.)?")+"(\\.|$)")),a.liveFired=this;var t=s.live.slice(0);for(i=0;i<t.length;i++)g=t[i],g.origType.replace(p,"")===a.type?r.push(g.selector):t.splice(i--,1);f=d(a.target).closest(r,a.currentTarget);for(j=0,k=f.length;j<k;j++){m=f[j];for(i=0;i<t.length;i++){g=t[i];if(m.selector===g.selector&&(!n||n.test(g.namespace))&&!m.elem.disabled){h=m.elem,e=null;if(g.preType==="mouseenter"||g.preType==="mouseleave")a.type=g.preType,e=d(a.relatedTarget).closest(g.selector)[0];(!e||e!==h)&&q.push({elem:h,handleObj:g,level:m.level})}}}for(j=0,k=q.length;j<k;j++){f=q[j];if(c&&f.level>c)break;a.currentTarget=f.elem,a.data=f.handleObj.data,a.handleObj=f.handleObj,o=f.handleObj.origHandler.apply(f.elem,arguments);if(o===!1||a.isPropagationStopped()){c=f.level,o===!1&&(b=!1);if(a.isImmediatePropagationStopped())break}}return b}}function C(a,c,e){var f=d.extend({},e[0]);f.type=a,f.originalEvent={},f.liveFired=b,d.event.handle.call(c,f),f.isDefaultPrevented()&&e[0].preventDefault()}function w(){return!0}function v(){return!1}function g(a){for(var b in a)if(b!=="toJSON")return!1;return!0}function f(a,c,f){if(f===b&&a.nodeType===1){f=a.getAttribute("data-"+c);if(typeof f==="string"){try{f=f==="true"?!0:f==="false"?!1:f==="null"?null:d.isNaN(f)?e.test(f)?d.parseJSON(f):f:parseFloat(f)}catch(g){}d.data(a,c,f)}else f=b}return f}var c=a.document,d=function(){function I(){if(!d.isReady){try{c.documentElement.doScroll("left")}catch(a){setTimeout(I,1);return}d.ready()}}var d=function(a,b){return new d.fn.init(a,b,g)},e=a.jQuery,f=a.$,g,h=/^(?:[^<]*(<[\w\W]+>)[^>]*$|#([\w\-]+)$)/,i=/\S/,j=/^\s+/,k=/\s+$/,l=/\d/,m=/^<(\w+)\s*\/?>(?:<\/\1>)?$/,n=/^[\],:{}\s]*$/,o=/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g,p=/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,q=/(?:^|:|,)(?:\s*\[)+/g,r=/(webkit)[ \/]([\w.]+)/,s=/(opera)(?:.*version)?[ \/]([\w.]+)/,t=/(msie) ([\w.]+)/,u=/(mozilla)(?:.*? rv:([\w.]+))?/,v=navigator.userAgent,w,x=!1,y,z="then done fail isResolved isRejected promise".split(" "),A,B=Object.prototype.toString,C=Object.prototype.hasOwnProperty,D=Array.prototype.push,E=Array.prototype.slice,F=String.prototype.trim,G=Array.prototype.indexOf,H={};d.fn=d.prototype={constructor:d,init:function(a,e,f){var g,i,j,k;if(!a)return this;if(a.nodeType){this.context=this[0]=a,this.length=1;return this}if(a==="body"&&!e&&c.body){this.context=c,this[0]=c.body,this.selector="body",this.length=1;return this}if(typeof a==="string"){g=h.exec(a);if(!g||!g[1]&&e)return!e||e.jquery?(e||f).find(a):this.constructor(e).find(a);if(g[1]){e=e instanceof d?e[0]:e,k=e?e.ownerDocument||e:c,j=m.exec(a),j?d.isPlainObject(e)?(a=[c.createElement(j[1])],d.fn.attr.call(a,e,!0)):a=[k.createElement(j[1])]:(j=d.buildFragment([g[1]],[k]),a=(j.cacheable?d.clone(j.fragment):j.fragment).childNodes);return d.merge(this,a)}i=c.getElementById(g[2]);if(i&&i.parentNode){if(i.id!==g[2])return f.find(a);this.length=1,this[0]=i}this.context=c,this.selector=a;return this}if(d.isFunction(a))return f.ready(a);a.selector!==b&&(this.selector=a.selector,this.context=a.context);return d.makeArray(a,this)},selector:"",jquery:"1.5.1",length:0,size:function(){return this.length},toArray:function(){return E.call(this,0)},get:function(a){return a==null?this.toArray():a<0?this[this.length+a]:this[a]},pushStack:function(a,b,c){var e=this.constructor();d.isArray(a)?D.apply(e,a):d.merge(e,a),e.prevObject=this,e.context=this.context,b==="find"?e.selector=this.selector+(this.selector?" ":"")+c:b&&(e.selector=this.selector+"."+b+"("+c+")");return e},each:function(a,b){return d.each(this,a,b)},ready:function(a){d.bindReady(),y.done(a);return this},eq:function(a){return a===-1?this.slice(a):this.slice(a,+a+1)},first:function(){return this.eq(0)},last:function(){return this.eq(-1)},slice:function(){return this.pushStack(E.apply(this,arguments),"slice",E.call(arguments).join(","))},map:function(a){return this.pushStack(d.map(this,function(b,c){return a.call(b,c,b)}))},end:function(){return this.prevObject||this.constructor(null)},push:D,sort:[].sort,splice:[].splice},d.fn.init.prototype=d.fn,d.extend=d.fn.extend=function(){var a,c,e,f,g,h,i=arguments[0]||{},j=1,k=arguments.length,l=!1;typeof i==="boolean"&&(l=i,i=arguments[1]||{},j=2),typeof i!=="object"&&!d.isFunction(i)&&(i={}),k===j&&(i=this,--j);for(;j<k;j++)if((a=arguments[j])!=null)for(c in a){e=i[c],f=a[c];if(i===f)continue;l&&f&&(d.isPlainObject(f)||(g=d.isArray(f)))?(g?(g=!1,h=e&&d.isArray(e)?e:[]):h=e&&d.isPlainObject(e)?e:{},i[c]=d.extend(l,h,f)):f!==b&&(i[c]=f)}return i},d.extend({noConflict:function(b){a.$=f,b&&(a.jQuery=e);return d},isReady:!1,readyWait:1,ready:function(a){a===!0&&d.readyWait--;if(!d.readyWait||a!==!0&&!d.isReady){if(!c.body)return setTimeout(d.ready,1);d.isReady=!0;if(a!==!0&&--d.readyWait>0)return;y.resolveWith(c,[d]),d.fn.trigger&&d(c).trigger("ready").unbind("ready")}},bindReady:function(){if(!x){x=!0;if(c.readyState==="complete")return setTimeout(d.ready,1);if(c.addEventListener)c.addEventListener("DOMContentLoaded",A,!1),a.addEventListener("load",d.ready,!1);else if(c.attachEvent){c.attachEvent("onreadystatechange",A),a.attachEvent("onload",d.ready);var b=!1;try{b=a.frameElement==null}catch(e){}c.documentElement.doScroll&&b&&I()}}},isFunction:function(a){return d.type(a)==="function"},isArray:Array.isArray||function(a){return d.type(a)==="array"},isWindow:function(a){return a&&typeof a==="object"&&"setInterval"in a},isNaN:function(a){return a==null||!l.test(a)||isNaN(a)},type:function(a){return a==null?String(a):H[B.call(a)]||"object"},isPlainObject:function(a){if(!a||d.type(a)!=="object"||a.nodeType||d.isWindow(a))return!1;if(a.constructor&&!C.call(a,"constructor")&&!C.call(a.constructor.prototype,"isPrototypeOf"))return!1;var c;for(c in a){}return c===b||C.call(a,c)},isEmptyObject:function(a){for(var b in a)return!1;return!0},error:function(a){throw a},parseJSON:function(b){if(typeof b!=="string"||!b)return null;b=d.trim(b);if(n.test(b.replace(o,"@").replace(p,"]").replace(q,"")))return a.JSON&&a.JSON.parse?a.JSON.parse(b):(new Function("return "+b))();d.error("Invalid JSON: "+b)},parseXML:function(b,c,e){a.DOMParser?(e=new DOMParser,c=e.parseFromString(b,"text/xml")):(c=new ActiveXObject("Microsoft.XMLDOM"),c.async="false",c.loadXML(b)),e=c.documentElement,(!e||!e.nodeName||e.nodeName==="parsererror")&&d.error("Invalid XML: "+b);return c},noop:function(){},globalEval:function(a){if(a&&i.test(a)){var b=c.head||c.getElementsByTagName("head")[0]||c.documentElement,e=c.createElement("script");d.support.scriptEval()?e.appendChild(c.createTextNode(a)):e.text=a,b.insertBefore(e,b.firstChild),b.removeChild(e)}},nodeName:function(a,b){return a.nodeName&&a.nodeName.toUpperCase()===b.toUpperCase()},each:function(a,c,e){var f,g=0,h=a.length,i=h===b||d.isFunction(a);if(e){if(i){for(f in a)if(c.apply(a[f],e)===!1)break}else for(;g<h;)if(c.apply(a[g++],e)===!1)break}else if(i){for(f in a)if(c.call(a[f],f,a[f])===!1)break}else for(var j=a[0];g<h&&c.call(j,g,j)!==!1;j=a[++g]){}return a},trim:F?function(a){return a==null?"":F.call(a)}:function(a){return a==null?"":(a+"").replace(j,"").replace(k,"")},makeArray:function(a,b){var c=b||[];if(a!=null){var e=d.type(a);a.length==null||e==="string"||e==="function"||e==="regexp"||d.isWindow(a)?D.call(c,a):d.merge(c,a)}return c},inArray:function(a,b){if(b.indexOf)return b.indexOf(a);for(var c=0,d=b.length;c<d;c++)if(b[c]===a)return c;return-1},merge:function(a,c){var d=a.length,e=0;if(typeof c.length==="number")for(var f=c.length;e<f;e++)a[d++]=c[e];else while(c[e]!==b)a[d++]=c[e++];a.length=d;return a},grep:function(a,b,c){var d=[],e;c=!!c;for(var f=0,g=a.length;f<g;f++)e=!!b(a[f],f),c!==e&&d.push(a[f]);return d},map:function(a,b,c){var d=[],e;for(var f=0,g=a.length;f<g;f++)e=b(a[f],f,c),e!=null&&(d[d.length]=e);return d.concat.apply([],d)},guid:1,proxy:function(a,c,e){arguments.length===2&&(typeof c==="string"?(e=a,a=e[c],c=b):c&&!d.isFunction(c)&&(e=c,c=b)),!c&&a&&(c=function(){return a.apply(e||this,arguments)}),a&&(c.guid=a.guid=a.guid||c.guid||d.guid++);return c},access:function(a,c,e,f,g,h){var i=a.length;if(typeof c==="object"){for(var j in c)d.access(a,j,c[j],f,g,e);return a}if(e!==b){f=!h&&f&&d.isFunction(e);for(var k=0;k<i;k++)g(a[k],c,f?e.call(a[k],k,g(a[k],c)):e,h);return a}return i?g(a[0],c):b},now:function(){return(new Date).getTime()},_Deferred:function(){var a=[],b,c,e,f={done:function(){if(!e){var c=arguments,g,h,i,j,k;b&&(k=b,b=0);for(g=0,h=c.length;g<h;g++)i=c[g],j=d.type(i),j==="array"?f.done.apply(f,i):j==="function"&&a.push(i);k&&f.resolveWith(k[0],k[1])}return this},resolveWith:function(d,f){if(!e&&!b&&!c){c=1;try{while(a[0])a.shift().apply(d,f)}catch(g){throw g}finally{b=[d,f],c=0}}return this},resolve:function(){f.resolveWith(d.isFunction(this.promise)?this.promise():this,arguments);return this},isResolved:function(){return c||b},cancel:function(){e=1,a=[];return this}};return f},Deferred:function(a){var b=d._Deferred(),c=d._Deferred(),e;d.extend(b,{then:function(a,c){b.done(a).fail(c);return this},fail:c.done,rejectWith:c.resolveWith,reject:c.resolve,isRejected:c.isResolved,promise:function(a){if(a==null){if(e)return e;e=a={}}var c=z.length;while(c--)a[z[c]]=b[z[c]];return a}}),b.done(c.cancel).fail(b.cancel),delete b.cancel,a&&a.call(b,b);return b},when:function(a){var b=arguments.length,c=b<=1&&a&&d.isFunction(a.promise)?a:d.Deferred(),e=c.promise();if(b>1){var f=E.call(arguments,0),g=b,h=function(a){return function(b){f[a]=arguments.length>1?E.call(arguments,0):b,--g||c.resolveWith(e,f)}};while(b--)a=f[b],a&&d.isFunction(a.promise)?a.promise().then(h(b),c.reject):--g;g||c.resolveWith(e,f)}else c!==a&&c.resolve(a);return e},uaMatch:function(a){a=a.toLowerCase();var b=r.exec(a)||s.exec(a)||t.exec(a)||a.indexOf("compatible")<0&&u.exec(a)||[];return{browser:b[1]||"",version:b[2]||"0"}},sub:function(){function a(b,c){return new a.fn.init(b,c)}d.extend(!0,a,this),a.superclass=this,a.fn=a.prototype=this(),a.fn.constructor=a,a.subclass=this.subclass,a.fn.init=function b(b,c){c&&c instanceof d&&!(c instanceof a)&&(c=a(c));return d.fn.init.call(this,b,c,e)},a.fn.init.prototype=a.fn;var e=a(c);return a},browser:{}}),y=d._Deferred(),d.each("Boolean Number String Function Array Date RegExp Object".split(" "),function(a,b){H["[object "+b+"]"]=b.toLowerCase()}),w=d.uaMatch(v),w.browser&&(d.browser[w.browser]=!0,d.browser.version=w.version),d.browser.webkit&&(d.browser.safari=!0),G&&(d.inArray=function(a,b){return G.call(b,a)}),i.test(" ")&&(j=/^[\s\xA0]+/,k=/[\s\xA0]+$/),g=d(c),c.addEventListener?A=function(){c.removeEventListener("DOMContentLoaded",A,!1),d.ready()}:c.attachEvent&&(A=function(){c.readyState==="complete"&&(c.detachEvent("onreadystatechange",A),d.ready())});return d}();(function(){d.support={};var b=c.createElement("div");b.style.display="none",b.innerHTML="   <link/><table></table><a href='/a' style='color:red;float:left;opacity:.55;'>a</a><input type='checkbox'/>";var e=b.getElementsByTagName("*"),f=b.getElementsByTagName("a")[0],g=c.createElement("select"),h=g.appendChild(c.createElement("option")),i=b.getElementsByTagName("input")[0];if(e&&e.length&&f){d.support={leadingWhitespace:b.firstChild.nodeType===3,tbody:!b.getElementsByTagName("tbody").length,htmlSerialize:!!b.getElementsByTagName("link").length,style:/red/.test(f.getAttribute("style")),hrefNormalized:f.getAttribute("href")==="/a",opacity:/^0.55$/.test(f.style.opacity),cssFloat:!!f.style.cssFloat,checkOn:i.value==="on",optSelected:h.selected,deleteExpando:!0,optDisabled:!1,checkClone:!1,noCloneEvent:!0,noCloneChecked:!0,boxModel:null,inlineBlockNeedsLayout:!1,shrinkWrapBlocks:!1,reliableHiddenOffsets:!0},i.checked=!0,d.support.noCloneChecked=i.cloneNode(!0).checked,g.disabled=!0,d.support.optDisabled=!h.disabled;var j=null;d.support.scriptEval=function(){if(j===null){var b=c.documentElement,e=c.createElement("script"),f="script"+d.now();try{e.appendChild(c.createTextNode("window."+f+"=1;"))}catch(g){}b.insertBefore(e,b.firstChild),a[f]?(j=!0,delete a[f]):j=!1,b.removeChild(e),b=e=f=null}return j};try{delete b.test}catch(k){d.support.deleteExpando=!1}!b.addEventListener&&b.attachEvent&&b.fireEvent&&(b.attachEvent("onclick",function l(){d.support.noCloneEvent=!1,b.detachEvent("onclick",l)}),b.cloneNode(!0).fireEvent("onclick")),b=c.createElement("div"),b.innerHTML="<input type='radio' name='radiotest' checked='checked'/>";var m=c.createDocumentFragment();m.appendChild(b.firstChild),d.support.checkClone=m.cloneNode(!0).cloneNode(!0).lastChild.checked,d(function(){var a=c.createElement("div"),b=c.getElementsByTagName("body")[0];if(b){a.style.width=a.style.paddingLeft="1px",b.appendChild(a),d.boxModel=d.support.boxModel=a.offsetWidth===2,"zoom"in a.style&&(a.style.display="inline",a.style.zoom=1,d.support.inlineBlockNeedsLayout=a.offsetWidth===2,a.style.display="",a.innerHTML="<div style='width:4px;'></div>",d.support.shrinkWrapBlocks=a.offsetWidth!==2),a.innerHTML="<table><tr><td style='padding:0;border:0;display:none'></td><td>t</td></tr></table>";var e=a.getElementsByTagName("td");d.support.reliableHiddenOffsets=e[0].offsetHeight===0,e[0].style.display="",e[1].style.display="none",d.support.reliableHiddenOffsets=d.support.reliableHiddenOffsets&&e[0].offsetHeight===0,a.innerHTML="",b.removeChild(a).style.display="none",a=e=null}});var n=function(a){var b=c.createElement("div");a="on"+a;if(!b.attachEvent)return!0;var d=a in b;d||(b.setAttribute(a,"return;"),d=typeof b[a]==="function"),b=null;return d};d.support.submitBubbles=n("submit"),d.support.changeBubbles=n("change"),b=e=f=null}})();var e=/^(?:\{.*\}|\[.*\])$/;d.extend({cache:{},uuid:0,expando:"jQuery"+(d.fn.jquery+Math.random()).replace(/\D/g,""),noData:{embed:!0,object:"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000",applet:!0},hasData:function(a){a=a.nodeType?d.cache[a[d.expando]]:a[d.expando];return!!a&&!g(a)},data:function(a,c,e,f){if(d.acceptData(a)){var g=d.expando,h=typeof c==="string",i,j=a.nodeType,k=j?d.cache:a,l=j?a[d.expando]:a[d.expando]&&d.expando;if((!l||f&&l&&!k[l][g])&&h&&e===b)return;l||(j?a[d.expando]=l=++d.uuid:l=d.expando),k[l]||(k[l]={},j||(k[l].toJSON=d.noop));if(typeof c==="object"||typeof c==="function")f?k[l][g]=d.extend(k[l][g],c):k[l]=d.extend(k[l],c);i=k[l],f&&(i[g]||(i[g]={}),i=i[g]),e!==b&&(i[c]=e);if(c==="events"&&!i[c])return i[g]&&i[g].events;return h?i[c]:i}},removeData:function(b,c,e){if(d.acceptData(b)){var f=d.expando,h=b.nodeType,i=h?d.cache:b,j=h?b[d.expando]:d.expando;if(!i[j])return;if(c){var k=e?i[j][f]:i[j];if(k){delete k[c];if(!g(k))return}}if(e){delete i[j][f];if(!g(i[j]))return}var l=i[j][f];d.support.deleteExpando||i!=a?delete i[j]:i[j]=null,l?(i[j]={},h||(i[j].toJSON=d.noop),i[j][f]=l):h&&(d.support.deleteExpando?delete b[d.expando]:b.removeAttribute?b.removeAttribute(d.expando):b[d.expando]=null)}},_data:function(a,b,c){return d.data(a,b,c,!0)},acceptData:function(a){if(a.nodeName){var b=d.noData[a.nodeName.toLowerCase()];if(b)return b!==!0&&a.getAttribute("classid")===b}return!0}}),d.fn.extend({data:function(a,c){var e=null;if(typeof a==="undefined"){if(this.length){e=d.data(this[0]);if(this[0].nodeType===1){var g=this[0].attributes,h;for(var i=0,j=g.length;i<j;i++)h=g[i].name,h.indexOf("data-")===0&&(h=h.substr(5),f(this[0],h,e[h]))}}return e}if(typeof a==="object")return this.each(function(){d.data(this,a)});var k=a.split(".");k[1]=k[1]?"."+k[1]:"";if(c===b){e=this.triggerHandler("getData"+k[1]+"!",[k[0]]),e===b&&this.length&&(e=d.data(this[0],a),e=f(this[0],a,e));return e===b&&k[1]?this.data(k[0]):e}return this.each(function(){var b=d(this),e=[k[0],c];b.triggerHandler("setData"+k[1]+"!",e),d.data(this,a,c),b.triggerHandler("changeData"+k[1]+"!",e)})},removeData:function(a){return this.each(function(){d.removeData(this,a)})}}),d.extend({queue:function(a,b,c){if(a){b=(b||"fx")+"queue";var e=d._data(a,b);if(!c)return e||[];!e||d.isArray(c)?e=d._data(a,b,d.makeArray(c)):e.push(c);return e}},dequeue:function(a,b){b=b||"fx";var c=d.queue(a,b),e=c.shift();e==="inprogress"&&(e=c.shift()),e&&(b==="fx"&&c.unshift("inprogress"),e.call(a,function(){d.dequeue(a,b)})),c.length||d.removeData(a,b+"queue",!0)}}),d.fn.extend({queue:function(a,c){typeof a!=="string"&&(c=a,a="fx");if(c===b)return d.queue(this[0],a);return this.each(function(b){var e=d.queue(this,a,c);a==="fx"&&e[0]!=="inprogress"&&d.dequeue(this,a)})},dequeue:function(a){return this.each(function(){d.dequeue(this,a)})},delay:function(a,b){a=d.fx?d.fx.speeds[a]||a:a,b=b||"fx";return this.queue(b,function(){var c=this;setTimeout(function(){d.dequeue(c,b)},a)})},clearQueue:function(a){return this.queue(a||"fx",[])}});var h=/[\n\t\r]/g,i=/\s+/,j=/\r/g,k=/^(?:href|src|style)$/,l=/^(?:button|input)$/i,m=/^(?:button|input|object|select|textarea)$/i,n=/^a(?:rea)?$/i,o=/^(?:radio|checkbox)$/i;d.props={"for":"htmlFor","class":"className",readonly:"readOnly",maxlength:"maxLength",cellspacing:"cellSpacing",rowspan:"rowSpan",colspan:"colSpan",tabindex:"tabIndex",usemap:"useMap",frameborder:"frameBorder"},d.fn.extend({attr:function(a,b){return d.access(this,a,b,!0,d.attr)},removeAttr:function(a,b){return this.each(function(){d.attr(this,a,""),this.nodeType===1&&this.removeAttribute(a)})},addClass:function(a){if(d.isFunction(a))return this.each(function(b){var c=d(this);c.addClass(a.call(this,b,c.attr("class")))});if(a&&typeof a==="string"){var b=(a||"").split(i);for(var c=0,e=this.length;c<e;c++){var f=this[c];if(f.nodeType===1)if(f.className){var g=" "+f.className+" ",h=f.className;for(var j=0,k=b.length;j<k;j++)g.indexOf(" "+b[j]+" ")<0&&(h+=" "+b[j]);f.className=d.trim(h)}else f.className=a}}return this},removeClass:function(a){if(d.isFunction(a))return this.each(function(b){var c=d(this);c.removeClass(a.call(this,b,c.attr("class")))});if(a&&typeof a==="string"||a===b){var c=(a||"").split(i);for(var e=0,f=this.length;e<f;e++){var g=this[e];if(g.nodeType===1&&g.className)if(a){var j=(" "+g.className+" ").replace(h," ");for(var k=0,l=c.length;k<l;k++)j=j.replace(" "+c[k]+" "," ");g.className=d.trim(j)}else g.className=""}}return this},toggleClass:function(a,b){var c=typeof a,e=typeof b==="boolean";if(d.isFunction(a))return this.each(function(c){var e=d(this);e.toggleClass(a.call(this,c,e.attr("class"),b),b)});return this.each(function(){if(c==="string"){var f,g=0,h=d(this),j=b,k=a.split(i);while(f=k[g++])j=e?j:!h.hasClass(f),h[j?"addClass":"removeClass"](f)}else if(c==="undefined"||c==="boolean")this.className&&d._data(this,"__className__",this.className),this.className=this.className||a===!1?"":d._data(this,"__className__")||""})},hasClass:function(a){var b=" "+a+" ";for(var c=0,d=this.length;c<d;c++)if((" "+this[c].className+" ").replace(h," ").indexOf(b)>-1)return!0;return!1},val:function(a){if(!arguments.length){var c=this[0];if(c){if(d.nodeName(c,"option")){var e=c.attributes.value;return!e||e.specified?c.value:c.text}if(d.nodeName(c,"select")){var f=c.selectedIndex,g=[],h=c.options,i=c.type==="select-one";if(f<0)return null;for(var k=i?f:0,l=i?f+1:h.length;k<l;k++){var m=h[k];if(m.selected&&(d.support.optDisabled?!m.disabled:m.getAttribute("disabled")===null)&&(!m.parentNode.disabled||!d.nodeName(m.parentNode,"optgroup"))){a=d(m).val();if(i)return a;g.push(a)}}if(i&&!g.length&&h.length)return d(h[f]).val();return g}if(o.test(c.type)&&!d.support.checkOn)return c.getAttribute("value")===null?"on":c.value;return(c.value||"").replace(j,"")}return b}var n=d.isFunction(a);return this.each(function(b){var c=d(this),e=a;if(this.nodeType===1){n&&(e=a.call(this,b,c.val())),e==null?e="":typeof e==="number"?e+="":d.isArray(e)&&(e=d.map(e,function(a){return a==null?"":a+""}));if(d.isArray(e)&&o.test(this.type))this.checked=d.inArray(c.val(),e)>=0;else if(d.nodeName(this,"select")){var f=d.makeArray(e);d("option",this).each(function(){this.selected=d.inArray(d(this).val(),f)>=0}),f.length||(this.selectedIndex=-1)}else this.value=e}})}}),d.extend({attrFn:{val:!0,css:!0,html:!0,text:!0,data:!0,width:!0,height:!0,offset:!0},attr:function(a,c,e,f){if(!a||a.nodeType===3||a.nodeType===8||a.nodeType===2)return b;if(f&&c in d.attrFn)return d(a)[c](e);var g=a.nodeType!==1||!d.isXMLDoc(a),h=e!==b;c=g&&d.props[c]||c;if(a.nodeType===1){var i=k.test(c);if(c==="selected"&&!d.support.optSelected){var j=a.parentNode;j&&(j.selectedIndex,j.parentNode&&j.parentNode.selectedIndex)}if((c in a||a[c]!==b)&&g&&!i){h&&(c==="type"&&l.test(a.nodeName)&&a.parentNode&&d.error("type property can't be changed"),e===null?a.nodeType===1&&a.removeAttribute(c):a[c]=e);if(d.nodeName(a,"form")&&a.getAttributeNode(c))return a.getAttributeNode(c).nodeValue;if(c==="tabIndex"){var o=a.getAttributeNode("tabIndex");return o&&o.specified?o.value:m.test(a.nodeName)||n.test(a.nodeName)&&a.href?0:b}return a[c]}if(!d.support.style&&g&&c==="style"){h&&(a.style.cssText=""+e);return a.style.cssText}h&&a.setAttribute(c,""+e);if(!a.attributes[c]&&(a.hasAttribute&&!a.hasAttribute(c)))return b;var p=!d.support.hrefNormalized&&g&&i?a.getAttribute(c,2):a.getAttribute(c);return p===null?b:p}h&&(a[c]=e);return a[c]}});var p=/\.(.*)$/,q=/^(?:textarea|input|select)$/i,r=/\./g,s=/ /g,t=/[^\w\s.|`]/g,u=function(a){return a.replace(t,"\\$&")};d.event={add:function(c,e,f,g){if(c.nodeType!==3&&c.nodeType!==8){try{d.isWindow(c)&&(c!==a&&!c.frameElement)&&(c=a)}catch(h){}if(f===!1)f=v;else if(!f)return;var i,j;f.handler&&(i=f,f=i.handler),f.guid||(f.guid=d.guid++);var k=d._data(c);if(!k)return;var l=k.events,m=k.handle;l||(k.events=l={}),m||(k.handle=m=function(){return typeof d!=="undefined"&&!d.event.triggered?d.event.handle.apply(m.elem,arguments):b}),m.elem=c,e=e.split(" ");var n,o=0,p;while(n=e[o++]){j=i?d.extend({},i):{handler:f,data:g},n.indexOf(".")>-1?(p=n.split("."),n=p.shift(),j.namespace=p.slice(0).sort().join(".")):(p=[],j.namespace=""),j.type=n,j.guid||(j.guid=f.guid);var q=l[n],r=d.event.special[n]||{};if(!q){q=l[n]=[];if(!r.setup||r.setup.call(c,g,p,m)===!1)c.addEventListener?c.addEventListener(n,m,!1):c.attachEvent&&c.attachEvent("on"+n,m)}r.add&&(r.add.call(c,j),j.handler.guid||(j.handler.guid=f.guid)),q.push(j),d.event.global[n]=!0}c=null}},global:{},remove:function(a,c,e,f){if(a.nodeType!==3&&a.nodeType!==8){e===!1&&(e=v);var g,h,i,j,k=0,l,m,n,o,p,q,r,s=d.hasData(a)&&d._data(a),t=s&&s.events;if(!s||!t)return;c&&c.type&&(e=c.handler,c=c.type);if(!c||typeof c==="string"&&c.charAt(0)==="."){c=c||"";for(h in t)d.event.remove(a,h+c);return}c=c.split(" ");while(h=c[k++]){r=h,q=null,l=h.indexOf(".")<0,m=[],l||(m=h.split("."),h=m.shift(),n=new RegExp("(^|\\.)"+d.map(m.slice(0).sort(),u).join("\\.(?:.*\\.)?")+"(\\.|$)")),p=t[h];if(!p)continue;if(!e){for(j=0;j<p.length;j++){q=p[j];if(l||n.test(q.namespace))d.event.remove(a,r,q.handler,j),p.splice(j--,1)}continue}o=d.event.special[h]||{};for(j=f||0;j<p.length;j++){q=p[j];if(e.guid===q.guid){if(l||n.test(q.namespace))f==null&&p.splice(j--,1),o.remove&&o.remove.call(a,q);if(f!=null)break}}if(p.length===0||f!=null&&p.length===1)(!o.teardown||o.teardown.call(a,m)===!1)&&d.removeEvent(a,h,s.handle),g=null,delete t[h]}if(d.isEmptyObject(t)){var w=s.handle;w&&(w.elem=null),delete s.events,delete s.handle,d.isEmptyObject(s)&&d.removeData(a,b,!0)}}},trigger:function(a,c,e){var f=a.type||a,g=arguments[3];if(!g){a=typeof a==="object"?a[d.expando]?a:d.extend(d.Event(f),a):d.Event(f),f.indexOf("!")>=0&&(a.type=f=f.slice(0,-1),a.exclusive=!0),e||(a.stopPropagation(),d.event.global[f]&&d.each(d.cache,function(){var b=d.expando,e=this[b];e&&e.events&&e.events[f]&&d.event.trigger(a,c,e.handle.elem)}));if(!e||e.nodeType===3||e.nodeType===8)return b;a.result=b,a.target=e,c=d.makeArray(c),c.unshift(a)}a.currentTarget=e;var h=d._data(e,"handle");h&&h.apply(e,c);var i=e.parentNode||e.ownerDocument;try{e&&e.nodeName&&d.noData[e.nodeName.toLowerCase()]||e["on"+f]&&e["on"+f].apply(e,c)===!1&&(a.result=!1,a.preventDefault())}catch(j){}if(!a.isPropagationStopped()&&i)d.event.trigger(a,c,i,!0);else if(!a.isDefaultPrevented()){var k,l=a.target,m=f.replace(p,""),n=d.nodeName(l,"a")&&m==="click",o=d.event.special[m]||{};if((!o._default||o._default.call(e,a)===!1)&&!n&&!(l&&l.nodeName&&d.noData[l.nodeName.toLowerCase()])){try{l[m]&&(k=l["on"+m],k&&(l["on"+m]=null),d.event.triggered=!0,l[m]())}catch(q){}k&&(l["on"+m]=k),d.event.triggered=!1}}},handle:function(c){var e,f,g,h,i,j=[],k=d.makeArray(arguments);c=k[0]=d.event.fix(c||a.event),c.currentTarget=this,e=c.type.indexOf(".")<0&&!c.exclusive,e||(g=c.type.split("."),c.type=g.shift(),j=g.slice(0).sort(),h=new RegExp("(^|\\.)"+j.join("\\.(?:.*\\.)?")+"(\\.|$)")),c.namespace=c.namespace||j.join("."),i=d._data(this,"events"),f=(i||{})[c.type];if(i&&f){f=f.slice(0);for(var l=0,m=f.length;l<m;l++){var n=f[l];if(e||h.test(n.namespace)){c.handler=n.handler,c.data=n.data,c.handleObj=n;var o=n.handler.apply(this,k);o!==b&&(c.result=o,o===!1&&(c.preventDefault(),c.stopPropagation()));if(c.isImmediatePropagationStopped())break}}}return c.result},props:"altKey attrChange attrName bubbles button cancelable charCode clientX clientY ctrlKey currentTarget data detail eventPhase fromElement handler keyCode layerX layerY metaKey newValue offsetX offsetY pageX pageY prevValue relatedNode relatedTarget screenX screenY shiftKey srcElement target toElement view wheelDelta which".split(" "),fix:function(a){if(a[d.expando])return a;var e=a;a=d.Event(e);for(var f=this.props.length,g;f;)g=this.props[--f],a[g]=e[g];a.target||(a.target=a.srcElement||c),a.target.nodeType===3&&(a.target=a.target.parentNode),!a.relatedTarget&&a.fromElement&&(a.relatedTarget=a.fromElement===a.target?a.toElement:a.fromElement);if(a.pageX==null&&a.clientX!=null){var h=c.documentElement,i=c.body;a.pageX=a.clientX+(h&&h.scrollLeft||i&&i.scrollLeft||0)-(h&&h.clientLeft||i&&i.clientLeft||0),a.pageY=a.clientY+(h&&h.scrollTop||i&&i.scrollTop||0)-(h&&h.clientTop||i&&i.clientTop||0)}a.which==null&&(a.charCode!=null||a.keyCode!=null)&&(a.which=a.charCode!=null?a.charCode:a.keyCode),!a.metaKey&&a.ctrlKey&&(a.metaKey=a.ctrlKey),!a.which&&a.button!==b&&(a.which=a.button&1?1:a.button&2?3:a.button&4?2:0);return a},guid:1e8,proxy:d.proxy,special:{ready:{setup:d.bindReady,teardown:d.noop},live:{add:function(a){d.event.add(this,F(a.origType,a.selector),d.extend({},a,{handler:E,guid:a.handler.guid}))},remove:function(a){d.event.remove(this,F(a.origType,a.selector),a)}},beforeunload:{setup:function(a,b,c){d.isWindow(this)&&(this.onbeforeunload=c)},teardown:function(a,b){this.onbeforeunload===b&&(this.onbeforeunload=null)}}}},d.removeEvent=c.removeEventListener?function(a,b,c){a.removeEventListener&&a.removeEventListener(b,c,!1)}:function(a,b,c){a.detachEvent&&a.detachEvent("on"+b,c)},d.Event=function(a){if(!this.preventDefault)return new d.Event(a);a&&a.type?(this.originalEvent=a,this.type=a.type,this.isDefaultPrevented=a.defaultPrevented||a.returnValue===!1||a.getPreventDefault&&a.getPreventDefault()?w:v):this.type=a,this.timeStamp=d.now(),this[d.expando]=!0},d.Event.prototype={preventDefault:function(){this.isDefaultPrevented=w;var a=this.originalEvent;a&&(a.preventDefault?a.preventDefault():a.returnValue=!1)},stopPropagation:function(){this.isPropagationStopped=w;var a=this.originalEvent;a&&(a.stopPropagation&&a.stopPropagation(),a.cancelBubble=!0)},stopImmediatePropagation:function(){this.isImmediatePropagationStopped=w,this.stopPropagation()},isDefaultPrevented:v,isPropagationStopped:v,isImmediatePropagationStopped:v};var x=function(a){var b=a.relatedTarget;try{if(b!==c&&!b.parentNode)return;while(b&&b!==this)b=b.parentNode;b!==this&&(a.type=a.data,d.event.handle.apply(this,arguments))}catch(e){}},y=function(a){a.type=a.data,d.event.handle.apply(this,arguments)};d.each({mouseenter:"mouseover",mouseleave:"mouseout"},function(a,b){d.event.special[a]={setup:function(c){d.event.add(this,b,c&&c.selector?y:x,a)},teardown:function(a){d.event.remove(this,b,a&&a.selector?y:x)}}}),d.support.submitBubbles||(d.event.special.submit={setup:function(a,b){if(this.nodeName&&this.nodeName.toLowerCase()!=="form")d.event.add(this,"click.specialSubmit",function(a){var b=a.target,c=b.type;(c==="submit"||c==="image")&&d(b).closest("form").length&&C("submit",this,arguments)}),d.event.add(this,"keypress.specialSubmit",function(a){var b=a.target,c=b.type;(c==="text"||c==="password")&&d(b).closest("form").length&&a.keyCode===13&&C("submit",this,arguments)});else return!1},teardown:function(a){d.event.remove(this,".specialSubmit")}});if(!d.support.changeBubbles){var z,A=function(a){var b=a.type,c=a.value;b==="radio"||b==="checkbox"?c=a.checked:b==="select-multiple"?c=a.selectedIndex>-1?d.map(a.options,function(a){return a.selected}).join("-"):"":a.nodeName.toLowerCase()==="select"&&(c=a.selectedIndex);return c},B=function B(a){var c=a.target,e,f;if(q.test(c.nodeName)&&!c.readOnly){e=d._data(c,"_change_data"),f=A(c),(a.type!=="focusout"||c.type!=="radio")&&d._data(c,"_change_data",f);if(e===b||f===e)return;if(e!=null||f)a.type="change",a.liveFired=b,d.event.trigger(a,arguments[1],c)}};d.event.special.change={filters:{focusout:B,beforedeactivate:B,click:function(a){var b=a.target,c=b.type;(c==="radio"||c==="checkbox"||b.nodeName.toLowerCase()==="select")&&B.call(this,a)},keydown:function(a){var b=a.target,c=b.type;(a.keyCode===13&&b.nodeName.toLowerCase()!=="textarea"||a.keyCode===32&&(c==="checkbox"||c==="radio")||c==="select-multiple")&&B.call(this,a)},beforeactivate:function(a){var b=a.target;d._data(b,"_change_data",A(b))}},setup:function(a,b){if(this.type==="file")return!1;for(var c in z)d.event.add(this,c+".specialChange",z[c]);return q.test(this.nodeName)},teardown:function(a){d.event.remove(this,".specialChange");return q.test(this.nodeName)}},z=d.event.special.change.filters,z.focus=z.beforeactivate}c.addEventListener&&d.each({focus:"focusin",blur:"focusout"},function(a,b){function c(a){a=d.event.fix(a),a.type=b;return d.event.handle.call(this,a)}d.event.special[b]={setup:function(){this.addEventListener(a,c,!0)},teardown:function(){this.removeEventListener(a,c,!0)}}}),d.each(["bind","one"],function(a,c){d.fn[c]=function(a,e,f){if(typeof a==="object"){for(var g in a)this[c](g,e,a[g],f);return this}if(d.isFunction(e)||e===!1)f=e,e=b;var h=c==="one"?d.proxy(f,function(a){d(this).unbind(a,h);return f.apply(this,arguments)}):f;if(a==="unload"&&c!=="one")this.one(a,e,f);else for(var i=0,j=this.length;i<j;i++)d.event.add(this[i],a,h,e);return this}}),d.fn.extend({unbind:function(a,b){if(typeof a!=="object"||a.preventDefault)for(var e=0,f=this.length;e<f;e++)d.event.remove(this[e],a,b);else for(var c in a)this.unbind(c,a[c]);return this},delegate:function(a,b,c,d){return this.live(b,c,d,a)},undelegate:function(a,b,c){return arguments.length===0?this.unbind("live"):this.die(b,null,c,a)},trigger:function(a,b){return this.each(function(){d.event.trigger(a,b,this)})},triggerHandler:function(a,b){if(this[0]){var c=d.Event(a);c.preventDefault(),c.stopPropagation(),d.event.trigger(c,b,this[0]);return c.result}},toggle:function(a){var b=arguments,c=1;while(c<b.length)d.proxy(a,b[c++]);return this.click(d.proxy(a,function(e){var f=(d._data(this,"lastToggle"+a.guid)||0)%c;d._data(this,"lastToggle"+a.guid,f+1),e.preventDefault();return b[f].apply(this,arguments)||!1}))},hover:function(a,b){return this.mouseenter(a).mouseleave(b||a)}});var D={focus:"focusin",blur:"focusout",mouseenter:"mouseover",mouseleave:"mouseout"};d.each(["live","die"],function(a,c){d.fn[c]=function(a,e,f,g){var h,i=0,j,k,l,m=g||this.selector,n=g?this:d(this.context);if(typeof a==="object"&&!a.preventDefault){for(var o in a)n[c](o,e,a[o],m);return this}d.isFunction(e)&&(f=e,e=b),a=(a||"").split(" ");while((h=a[i++])!=null){j=p.exec(h),k="",j&&(k=j[0],h=h.replace(p,""));if(h==="hover"){a.push("mouseenter"+k,"mouseleave"+k);continue}l=h,h==="focus"||h==="blur"?(a.push(D[h]+k),h=h+k):h=(D[h]||h)+k;if(c==="live")for(var q=0,r=n.length;q<r;q++)d.event.add(n[q],"live."+F(h,m),{data:e,selector:m,handler:f,origType:h,origHandler:f,preType:l});else n.unbind("live."+F(h,m),f)}return this}}),d.each("blur focus focusin focusout load resize scroll unload click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave change select submit keydown keypress keyup error".split(" "),function(a,b){d.fn[b]=function(a,c){c==null&&(c=a,a=null);return arguments.length>0?this.bind(b,a,c):this.trigger(b)},d.attrFn&&(d.attrFn[b]=!0)}),function(){function u(a,b,c,d,e,f){for(var g=0,h=d.length;g<h;g++){var i=d[g];if(i){var j=!1;i=i[a];while(i){if(i.sizcache===c){j=d[i.sizset];break}if(i.nodeType===1){f||(i.sizcache=c,i.sizset=g);if(typeof b!=="string"){if(i===b){j=!0;break}}else if(k.filter(b,[i]).length>0){j=i;break}}i=i[a]}d[g]=j}}}function t(a,b,c,d,e,f){for(var g=0,h=d.length;g<h;g++){var i=d[g];if(i){var j=!1;i=i[a];while(i){if(i.sizcache===c){j=d[i.sizset];break}i.nodeType===1&&!f&&(i.sizcache=c,i.sizset=g);if(i.nodeName.toLowerCase()===b){j=i;break}i=i[a]}d[g]=j}}}var a=/((?:\((?:\([^()]+\)|[^()]+)+\)|\[(?:\[[^\[\]]*\]|['"][^'"]*['"]|[^\[\]'"]+)+\]|\\.|[^ >+~,(\[\\]+)+|[>+~])(\s*,\s*)?((?:.|\r|\n)*)/g,e=0,f=Object.prototype.toString,g=!1,h=!0,i=/\\/g,j=/\W/;[0,0].sort(function(){h=!1;return 0});var k=function(b,d,e,g){e=e||[],d=d||c;var h=d;if(d.nodeType!==1&&d.nodeType!==9)return[];if(!b||typeof b!=="string")return e;var i,j,n,o,q,r,s,t,u=!0,w=k.isXML(d),x=[],y=b;do{a.exec(""),i=a.exec(y);if(i){y=i[3],x.push(i[1]);if(i[2]){o=i[3];break}}}while(i);if(x.length>1&&m.exec(b))if(x.length===2&&l.relative[x[0]])j=v(x[0]+x[1],d);else{j=l.relative[x[0]]?[d]:k(x.shift(),d);while(x.length)b=x.shift(),l.relative[b]&&(b+=x.shift()),j=v(b,j)}else{!g&&x.length>1&&d.nodeType===9&&!w&&l.match.ID.test(x[0])&&!l.match.ID.test(x[x.length-1])&&(q=k.find(x.shift(),d,w),d=q.expr?k.filter(q.expr,q.set)[0]:q.set[0]);if(d){q=g?{expr:x.pop(),set:p(g)}:k.find(x.pop(),x.length===1&&(x[0]==="~"||x[0]==="+")&&d.parentNode?d.parentNode:d,w),j=q.expr?k.filter(q.expr,q.set):q.set,x.length>0?n=p(j):u=!1;while(x.length)r=x.pop(),s=r,l.relative[r]?s=x.pop():r="",s==null&&(s=d),l.relative[r](n,s,w)}else n=x=[]}n||(n=j),n||k.error(r||b);if(f.call(n)==="[object Array]")if(u)if(d&&d.nodeType===1)for(t=0;n[t]!=null;t++)n[t]&&(n[t]===!0||n[t].nodeType===1&&k.contains(d,n[t]))&&e.push(j[t]);else for(t=0;n[t]!=null;t++)n[t]&&n[t].nodeType===1&&e.push(j[t]);else e.push.apply(e,n);else p(n,e);o&&(k(o,h,e,g),k.uniqueSort(e));return e};k.uniqueSort=function(a){if(r){g=h,a.sort(r);if(g)for(var b=1;b<a.length;b++)a[b]===a[b-1]&&a.splice(b--,1)}return a},k.matches=function(a,b){return k(a,null,null,b)},k.matchesSelector=function(a,b){return k(b,null,null,[a]).length>0},k.find=function(a,b,c){var d;if(!a)return[];for(var e=0,f=l.order.length;e<f;e++){var g,h=l.order[e];if(g=l.leftMatch[h].exec(a)){var j=g[1];g.splice(1,1);if(j.substr(j.length-1)!=="\\"){g[1]=(g[1]||"").replace(i,""),d=l.find[h](g,b,c);if(d!=null){a=a.replace(l.match[h],"");break}}}}d||(d=typeof b.getElementsByTagName!=="undefined"?b.getElementsByTagName("*"):[]);return{set:d,expr:a}},k.filter=function(a,c,d,e){var f,g,h=a,i=[],j=c,m=c&&c[0]&&k.isXML(c[0]);while(a&&c.length){for(var n in l.filter)if((f=l.leftMatch[n].exec(a))!=null&&f[2]){var o,p,q=l.filter[n],r=f[1];g=!1,f.splice(1,1);if(r.substr(r.length-1)==="\\")continue;j===i&&(i=[]);if(l.preFilter[n]){f=l.preFilter[n](f,j,d,i,e,m);if(f){if(f===!0)continue}else g=o=!0}if(f)for(var s=0;(p=j[s])!=null;s++)if(p){o=q(p,f,s,j);var t=e^!!o;d&&o!=null?t?g=!0:j[s]=!1:t&&(i.push(p),g=!0)}if(o!==b){d||(j=i),a=a.replace(l.match[n],"");if(!g)return[];break}}if(a===h)if(g==null)k.error(a);else break;h=a}return j},k.error=function(a){throw"Syntax error, unrecognized expression: "+a};var l=k.selectors={order:["ID","NAME","TAG"],match:{ID:/#((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,CLASS:/\.((?:[\w\u00c0-\uFFFF\-]|\\.)+)/,NAME:/\[name=['"]*((?:[\w\u00c0-\uFFFF\-]|\\.)+)['"]*\]/,ATTR:/\[\s*((?:[\w\u00c0-\uFFFF\-]|\\.)+)\s*(?:(\S?=)\s*(?:(['"])(.*?)\3|(#?(?:[\w\u00c0-\uFFFF\-]|\\.)*)|)|)\s*\]/,TAG:/^((?:[\w\u00c0-\uFFFF\*\-]|\\.)+)/,CHILD:/:(only|nth|last|first)-child(?:\(\s*(even|odd|(?:[+\-]?\d+|(?:[+\-]?\d*)?n\s*(?:[+\-]\s*\d+)?))\s*\))?/,POS:/:(nth|eq|gt|lt|first|last|even|odd)(?:\((\d*)\))?(?=[^\-]|$)/,PSEUDO:/:((?:[\w\u00c0-\uFFFF\-]|\\.)+)(?:\((['"]?)((?:\([^\)]+\)|[^\(\)]*)+)\2\))?/},leftMatch:{},attrMap:{"class":"className","for":"htmlFor"},attrHandle:{href:function(a){return a.getAttribute("href")},type:function(a){return a.getAttribute("type")}},relative:{"+":function(a,b){var c=typeof b==="string",d=c&&!j.test(b),e=c&&!d;d&&(b=b.toLowerCase());for(var f=0,g=a.length,h;f<g;f++)if(h=a[f]){while((h=h.previousSibling)&&h.nodeType!==1){}a[f]=e||h&&h.nodeName.toLowerCase()===b?h||!1:h===b}e&&k.filter(b,a,!0)},">":function(a,b){var c,d=typeof b==="string",e=0,f=a.length;if(d&&!j.test(b)){b=b.toLowerCase();for(;e<f;e++){c=a[e];if(c){var g=c.parentNode;a[e]=g.nodeName.toLowerCase()===b?g:!1}}}else{for(;e<f;e++)c=a[e],c&&(a[e]=d?c.parentNode:c.parentNode===b);d&&k.filter(b,a,!0)}},"":function(a,b,c){var d,f=e++,g=u;typeof b==="string"&&!j.test(b)&&(b=b.toLowerCase(),d=b,g=t),g("parentNode",b,f,a,d,c)},"~":function(a,b,c){var d,f=e++,g=u;typeof b==="string"&&!j.test(b)&&(b=b.toLowerCase(),d=b,g=t),g("previousSibling",b,f,a,d,c)}},find:{ID:function(a,b,c){if(typeof b.getElementById!=="undefined"&&!c){var d=b.getElementById(a[1]);return d&&d.parentNode?[d]:[]}},NAME:function(a,b){if(typeof b.getElementsByName!=="undefined"){var c=[],d=b.getElementsByName(a[1]);for(var e=0,f=d.length;e<f;e++)d[e].getAttribute("name")===a[1]&&c.push(d[e]);return c.length===0?null:c}},TAG:function(a,b){if(typeof b.getElementsByTagName!=="undefined")return b.getElementsByTagName(a[1])}},preFilter:{CLASS:function(a,b,c,d,e,f){a=" "+a[1].replace(i,"")+" ";if(f)return a;for(var g=0,h;(h=b[g])!=null;g++)h&&(e^(h.className&&(" "+h.className+" ").replace(/[\t\n\r]/g," ").indexOf(a)>=0)?c||d.push(h):c&&(b[g]=!1));return!1},ID:function(a){return a[1].replace(i,"")},TAG:function(a,b){return a[1].replace(i,"").toLowerCase()},CHILD:function(a){if(a[1]==="nth"){a[2]||k.error(a[0]),a[2]=a[2].replace(/^\+|\s*/g,"");var b=/(-?)(\d*)(?:n([+\-]?\d*))?/.exec(a[2]==="even"&&"2n"||a[2]==="odd"&&"2n+1"||!/\D/.test(a[2])&&"0n+"+a[2]||a[2]);a[2]=b[1]+(b[2]||1)-0,a[3]=b[3]-0}else a[2]&&k.error(a[0]);a[0]=e++;return a},ATTR:function(a,b,c,d,e,f){var g=a[1]=a[1].replace(i,"");!f&&l.attrMap[g]&&(a[1]=l.attrMap[g]),a[4]=(a[4]||a[5]||"").replace(i,""),a[2]==="~="&&(a[4]=" "+a[4]+" ");return a},PSEUDO:function(b,c,d,e,f){if(b[1]==="not")if((a.exec(b[3])||"").length>1||/^\w/.test(b[3]))b[3]=k(b[3],null,null,c);else{var g=k.filter(b[3],c,d,!0^f);d||e.push.apply(e,g);return!1}else if(l.match.POS.test(b[0])||l.match.CHILD.test(b[0]))return!0;return b},POS:function(a){a.unshift(!0);return a}},filters:{enabled:function(a){return a.disabled===!1&&a.type!=="hidden"},disabled:function(a){return a.disabled===!0},checked:function(a){return a.checked===!0},selected:function(a){a.parentNode&&a.parentNode.selectedIndex;return a.selected===!0},parent:function(a){return!!a.firstChild},empty:function(a){return!a.firstChild},has:function(a,b,c){return!!k(c[3],a).length},header:function(a){return/h\d/i.test(a.nodeName)},text:function(a){return"text"===a.getAttribute("type")},radio:function(a){return"radio"===a.type},checkbox:function(a){return"checkbox"===a.type},file:function(a){return"file"===a.type},password:function(a){return"password"===a.type},submit:function(a){return"submit"===a.type},image:function(a){return"image"===a.type},reset:function(a){return"reset"===a.type},button:function(a){return"button"===a.type||a.nodeName.toLowerCase()==="button"},input:function(a){return/input|select|textarea|button/i.test(a.nodeName)}},setFilters:{first:function(a,b){return b===0},last:function(a,b,c,d){return b===d.length-1},even:function(a,b){return b%2===0},odd:function(a,b){return b%2===1},lt:function(a,b,c){return b<c[3]-0},gt:function(a,b,c){return b>c[3]-0},nth:function(a,b,c){return c[3]-0===b},eq:function(a,b,c){return c[3]-0===b}},filter:{PSEUDO:function(a,b,c,d){var e=b[1],f=l.filters[e];if(f)return f(a,c,b,d);if(e==="contains")return(a.textContent||a.innerText||k.getText([a])||"").indexOf(b[3])>=0;if(e==="not"){var g=b[3];for(var h=0,i=g.length;h<i;h++)if(g[h]===a)return!1;return!0}k.error(e)},CHILD:function(a,b){var c=b[1],d=a;switch(c){case"only":case"first":while(d=d.previousSibling)if(d.nodeType===1)return!1;if(c==="first")return!0;d=a;case"last":while(d=d.nextSibling)if(d.nodeType===1)return!1;return!0;case"nth":var e=b[2],f=b[3];if(e===1&&f===0)return!0;var g=b[0],h=a.parentNode;if(h&&(h.sizcache!==g||!a.nodeIndex)){var i=0;for(d=h.firstChild;d;d=d.nextSibling)d.nodeType===1&&(d.nodeIndex=++i);h.sizcache=g}var j=a.nodeIndex-f;return e===0?j===0:j%e===0&&j/e>=0}},ID:function(a,b){return a.nodeType===1&&a.getAttribute("id")===b},TAG:function(a,b){return b==="*"&&a.nodeType===1||a.nodeName.toLowerCase()===b},CLASS:function(a,b){return(" "+(a.className||a.getAttribute("class"))+" ").indexOf(b)>-1},ATTR:function(a,b){var c=b[1],d=l.attrHandle[c]?l.attrHandle[c](a):a[c]!=null?a[c]:a.getAttribute(c),e=d+"",f=b[2],g=b[4];return d==null?f==="!=":f==="="?e===g:f==="*="?e.indexOf(g)>=0:f==="~="?(" "+e+" ").indexOf(g)>=0:g?f==="!="?e!==g:f==="^="?e.indexOf(g)===0:f==="$="?e.substr(e.length-g.length)===g:f==="|="?e===g||e.substr(0,g.length+1)===g+"-":!1:e&&d!==!1},POS:function(a,b,c,d){var e=b[2],f=l.setFilters[e];if(f)return f(a,c,b,d)}}},m=l.match.POS,n=function(a,b){return"\\"+(b-0+1)};for(var o in l.match)l.match[o]=new RegExp(l.match[o].source+/(?![^\[]*\])(?![^\(]*\))/.source),l.leftMatch[o]=new RegExp(/(^(?:.|\r|\n)*?)/.source+l.match[o].source.replace(/\\(\d+)/g,n));var p=function(a,b){a=Array.prototype.slice.call(a,0);if(b){b.push.apply(b,a);return b}return a};try{Array.prototype.slice.call(c.documentElement.childNodes,0)[0].nodeType}catch(q){p=function(a,b){var c=0,d=b||[];if(f.call(a)==="[object Array]")Array.prototype.push.apply(d,a);else if(typeof a.length==="number")for(var e=a.length;c<e;c++)d.push(a[c]);else for(;a[c];c++)d.push(a[c]);return d}}var r,s;c.documentElement.compareDocumentPosition?r=function(a,b){if(a===b){g=!0;return 0}if(!a.compareDocumentPosition||!b.compareDocumentPosition)return a.compareDocumentPosition?-1:1;return a.compareDocumentPosition(b)&4?-1:1}:(r=function(a,b){var c,d,e=[],f=[],h=a.parentNode,i=b.parentNode,j=h;if(a===b){g=!0;return 0}if(h===i)return s(a,b);if(!h)return-1;if(!i)return 1;while(j)e.unshift(j),j=j.parentNode;j=i;while(j)f.unshift(j),j=j.parentNode;c=e.length,d=f.length;for(var k=0;k<c&&k<d;k++)if(e[k]!==f[k])return s(e[k],f[k]);return k===c?s(a,f[k],-1):s(e[k],b,1)},s=function(a,b,c){if(a===b)return c;var d=a.nextSibling;while(d){if(d===b)return-1;d=d.nextSibling}return 1}),k.getText=function(a){var b="",c;for(var d=0;a[d];d++)c=a[d],c.nodeType===3||c.nodeType===4?b+=c.nodeValue:c.nodeType!==8&&(b+=k.getText(c.childNodes));return b},function(){var a=c.createElement("div"),d="script"+(new Date).getTime(),e=c.documentElement;a.innerHTML="<a name='"+d+"'/>",e.insertBefore(a,e.firstChild),c.getElementById(d)&&(l.find.ID=function(a,c,d){if(typeof c.getElementById!=="undefined"&&!d){var e=c.getElementById(a[1]);return e?e.id===a[1]||typeof e.getAttributeNode!=="undefined"&&e.getAttributeNode("id").nodeValue===a[1]?[e]:b:[]}},l.filter.ID=function(a,b){var c=typeof a.getAttributeNode!=="undefined"&&a.getAttributeNode("id");return a.nodeType===1&&c&&c.nodeValue===b}),e.removeChild(a),e=a=null}(),function(){var a=c.createElement("div");a.appendChild(c.createComment("")),a.getElementsByTagName("*").length>0&&(l.find.TAG=function(a,b){var c=b.getElementsByTagName(a[1]);if(a[1]==="*"){var d=[];for(var e=0;c[e];e++)c[e].nodeType===1&&d.push(c[e]);c=d}return c}),a.innerHTML="<a href='#'></a>",a.firstChild&&typeof a.firstChild.getAttribute!=="undefined"&&a.firstChild.getAttribute("href")!=="#"&&(l.attrHandle.href=function(a){return a.getAttribute("href",2)}),a=null}(),c.querySelectorAll&&function(){var a=k,b=c.createElement("div"),d="__sizzle__";b.innerHTML="<p class='TEST'></p>";if(!b.querySelectorAll||b.querySelectorAll(".TEST").length!==0){k=function(b,e,f,g){e=e||c;if(!g&&!k.isXML(e)){var h=/^(\w+$)|^\.([\w\-]+$)|^#([\w\-]+$)/.exec(b);if(h&&(e.nodeType===1||e.nodeType===9)){if(h[1])return p(e.getElementsByTagName(b),f);if(h[2]&&l.find.CLASS&&e.getElementsByClassName)return p(e.getElementsByClassName(h[2]),f)}if(e.nodeType===9){if(b==="body"&&e.body)return p([e.body],f);if(h&&h[3]){var i=e.getElementById(h[3]);if(!i||!i.parentNode)return p([],f);if(i.id===h[3])return p([i],f)}try{return p(e.querySelectorAll(b),f)}catch(j){}}else if(e.nodeType===1&&e.nodeName.toLowerCase()!=="object"){var m=e,n=e.getAttribute("id"),o=n||d,q=e.parentNode,r=/^\s*[+~]/.test(b);n?o=o.replace(/'/g,"\\$&"):e.setAttribute("id",o),r&&q&&(e=e.parentNode);try{if(!r||q)return p(e.querySelectorAll("[id='"+o+"'] "+b),f)}catch(s){}finally{n||m.removeAttribute("id")}}}return a(b,e,f,g)};for(var e in a)k[e]=a[e];b=null}}(),function(){var a=c.documentElement,b=a.matchesSelector||a.mozMatchesSelector||a.webkitMatchesSelector||a.msMatchesSelector,d=!1;try{b.call(c.documentElement,"[test!='']:sizzle")}catch(e){d=!0}b&&(k.matchesSelector=function(a,c){c=c.replace(/\=\s*([^'"\]]*)\s*\]/g,"='$1']");if(!k.isXML(a))try{if(d||!l.match.PSEUDO.test(c)&&!/!=/.test(c))return b.call(a,c)}catch(e){}return k(c,null,null,[a]).length>0})}(),function(){var a=c.createElement("div");a.innerHTML="<div class='test e'></div><div class='test'></div>";if(a.getElementsByClassName&&a.getElementsByClassName("e").length!==0){a.lastChild.className="e";if(a.getElementsByClassName("e").length===1)return;l.order.splice(1,0,"CLASS"),l.find.CLASS=function(a,b,c){if(typeof b.getElementsByClassName!=="undefined"&&!c)return b.getElementsByClassName(a[1])},a=null}}(),c.documentElement.contains?k.contains=function(a,b){return a!==b&&(a.contains?a.contains(b):!0)}:c.documentElement.compareDocumentPosition?k.contains=function(a,b){return!!(a.compareDocumentPosition(b)&16)}:k.contains=function(){return!1},k.isXML=function(a){var b=(a?a.ownerDocument||a:0).documentElement;return b?b.nodeName!=="HTML":!1};var v=function(a,b){var c,d=[],e="",f=b.nodeType?[b]:b;while(c=l.match.PSEUDO.exec(a))e+=c[0],a=a.replace(l.match.PSEUDO,"");a=l.relative[a]?a+"*":a;for(var g=0,h=f.length;g<h;g++)k(a,f[g],d);return k.filter(e,d)};d.find=k,d.expr=k.selectors,d.expr[":"]=d.expr.filters,d.unique=k.uniqueSort,d.text=k.getText,d.isXMLDoc=k.isXML,d.contains=k.contains}();var G=/Until$/,H=/^(?:parents|prevUntil|prevAll)/,I=/,/,J=/^.[^:#\[\.,]*$/,K=Array.prototype.slice,L=d.expr.match.POS,M={children:!0,contents:!0,next:!0,prev:!0};d.fn.extend({find:function(a){var b=this.pushStack("","find",a),c=0;for(var e=0,f=this.length;e<f;e++){c=b.length,d.find(a,this[e],b);if(e>0)for(var g=c;g<b.length;g++)for(var h=0;h<c;h++)if(b[h]===b[g]){b.splice(g--,1);break}}return b},has:function(a){var b=d(a);return this.filter(function(){for(var a=0,c=b.length;a<c;a++)if(d.contains(this,b[a]))return!0})},not:function(a){return this.pushStack(O(this,a,!1),"not",a)},filter:function(a){return this.pushStack(O(this,a,!0),"filter",a)},is:function(a){return!!a&&d.filter(a,this).length>0},closest:function(a,b){var c=[],e,f,g=this[0];if(d.isArray(a)){var h,i,j={},k=1;if(g&&a.length){for(e=0,f=a.length;e<f;e++)i=a[e],j[i]||(j[i]=d.expr.match.POS.test(i)?d(i,b||this.context):i);while(g&&g.ownerDocument&&g!==b){for(i in j)h=j[i],(h.jquery?h.index(g)>-1:d(g).is(h))&&c.push({selector:i,elem:g,level:k});g=g.parentNode,k++}}return c}var l=L.test(a)?d(a,b||this.context):null;for(e=0,f=this.length;e<f;e++){g=this[e];while(g){if(l?l.index(g)>-1:d.find.matchesSelector(g,a)){c.push(g);break}g=g.parentNode;if(!g||!g.ownerDocument||g===b)break}}c=c.length>1?d.unique(c):c;return this.pushStack(c,"closest",a)},index:function(a){if(!a||typeof a==="string")return d.inArray(this[0],a?d(a):this.parent().children());return d.inArray(a.jquery?a[0]:a,this)},add:function(a,b){var c=typeof a==="string"?d(a,b):d.makeArray(a),e=d.merge(this.get(),c);return this.pushStack(N(c[0])||N(e[0])?e:d.unique(e))},andSelf:function(){return this.add(this.prevObject)}}),d.each({parent:function(a){var b=a.parentNode;return b&&b.nodeType!==11?b:null},parents:function(a){return d.dir(a,"parentNode")},parentsUntil:function(a,b,c){return d.dir(a,"parentNode",c)},next:function(a){return d.nth(a,2,"nextSibling")},prev:function(a){return d.nth(a,2,"previousSibling")},nextAll:function(a){return d.dir(a,"nextSibling")},prevAll:function(a){return d.dir(a,"previousSibling")},nextUntil:function(a,b,c){return d.dir(a,"nextSibling",c)},prevUntil:function(a,b,c){return d.dir(a,"previousSibling",c)},siblings:function(a){return d.sibling(a.parentNode.firstChild,a)},children:function(a){return d.sibling(a.firstChild)},contents:function(a){return d.nodeName(a,"iframe")?a.contentDocument||a.contentWindow.document:d.makeArray(a.childNodes)}},function(a,b){d.fn[a]=function(c,e){var f=d.map(this,b,c),g=K.call(arguments);G.test(a)||(e=c),e&&typeof e==="string"&&(f=d.filter(e,f)),f=this.length>1&&!M[a]?d.unique(f):f,(this.length>1||I.test(e))&&H.test(a)&&(f=f.reverse());return this.pushStack(f,a,g.join(","))}}),d.extend({filter:function(a,b,c){c&&(a=":not("+a+")");return b.length===1?d.find.matchesSelector(b[0],a)?[b[0]]:[]:d.find.matches(a,b)},dir:function(a,c,e){var f=[],g=a[c];while(g&&g.nodeType!==9&&(e===b||g.nodeType!==1||!d(g).is(e)))g.nodeType===1&&f.push(g),g=g[c];return f},nth:function(a,b,c,d){b=b||1;var e=0;for(;a;a=a[c])if(a.nodeType===1&&++e===b)break;return a},sibling:function(a,b){var c=[];for(;a;a=a.nextSibling)a.nodeType===1&&a!==b&&c.push(a);return c}});var P=/ jQuery\d+="(?:\d+|null)"/g,Q=/^\s+/,R=/<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/ig,S=/<([\w:]+)/,T=/<tbody/i,U=/<|&#?\w+;/,V=/<(?:script|object|embed|option|style)/i,W=/checked\s*(?:[^=]|=\s*.checked.)/i,X={option:[1,"<select multiple='multiple'>","</select>"],legend:[1,"<fieldset>","</fieldset>"],thead:[1,"<table>","</table>"],tr:[2,"<table><tbody>","</tbody></table>"],td:[3,"<table><tbody><tr>","</tr></tbody></table>"],col:[2,"<table><tbody></tbody><colgroup>","</colgroup></table>"],area:[1,"<map>","</map>"],_default:[0,"",""]};X.optgroup=X.option,X.tbody=X.tfoot=X.colgroup=X.caption=X.thead,X.th=X.td,d.support.htmlSerialize||(X._default=[1,"div<div>","</div>"]),d.fn.extend({text:function(a){if(d.isFunction(a))return this.each(function(b){var c=d(this);c.text(a.call(this,b,c.text()))});if(typeof a!=="object"&&a!==b)return this.empty().append((this[0]&&this[0].ownerDocument||c).createTextNode(a));return d.text(this)},wrapAll:function(a){if(d.isFunction(a))return this.each(function(b){d(this).wrapAll(a.call(this,b))});if(this[0]){var b=d(a,this[0].ownerDocument).eq(0).clone(!0);this[0].parentNode&&b.insertBefore(this[0]),b.map(function(){var a=this;while(a.firstChild&&a.firstChild.nodeType===1)a=a.firstChild;return a}).append(this)}return this},wrapInner:function(a){if(d.isFunction(a))return this.each(function(b){d(this).wrapInner(a.call(this,b))});return this.each(function(){var b=d(this),c=b.contents();c.length?c.wrapAll(a):b.append(a)})},wrap:function(a){return this.each(function(){d(this).wrapAll(a)})},unwrap:function(){return this.parent().each(function(){d.nodeName(this,"body")||d(this).replaceWith(this.childNodes)}).end()},append:function(){return this.domManip(arguments,!0,function(a){this.nodeType===1&&this.appendChild(a)})},prepend:function(){return this.domManip(arguments,!0,function(a){this.nodeType===1&&this.insertBefore(a,this.firstChild)})},before:function(){if(this[0]&&this[0].parentNode)return this.domManip(arguments,!1,function(a){this.parentNode.insertBefore(a,this)});if(arguments.length){var a=d(arguments[0]);a.push.apply(a,this.toArray());return this.pushStack(a,"before",arguments)}},after:function(){if(this[0]&&this[0].parentNode)return this.domManip(arguments,!1,function(a){this.parentNode.insertBefore(a,this.nextSibling)});if(arguments.length){var a=this.pushStack(this,"after",arguments);a.push.apply(a,d(arguments[0]).toArray());return a}},remove:function(a,b){for(var c=0,e;(e=this[c])!=null;c++)if(!a||d.filter(a,[e]).length)!b&&e.nodeType===1&&(d.cleanData(e.getElementsByTagName("*")),d.cleanData([e])),e.parentNode&&e.parentNode.removeChild(e);return this},empty:function(){for(var a=0,b;(b=this[a])!=null;a++){b.nodeType===1&&d.cleanData(b.getElementsByTagName("*"));while(b.firstChild)b.removeChild(b.firstChild)}return this},clone:function(a,b){a=a==null?!1:a,b=b==null?a:b;return this.map(function(){return d.clone(this,a,b)})},html:function(a){if(a===b)return this[0]&&this[0].nodeType===1?this[0].innerHTML.replace(P,""):null;if(typeof a!=="string"||V.test(a)||!d.support.leadingWhitespace&&Q.test(a)||X[(S.exec(a)||["",""])[1].toLowerCase()])d.isFunction(a)?this.each(function(b){var c=d(this);c.html(a.call(this,b,c.html()))}):this.empty().append(a);else{a=a.replace(R,"<$1></$2>");try{for(var c=0,e=this.length;c<e;c++)this[c].nodeType===1&&(d.cleanData(this[c].getElementsByTagName("*")),this[c].innerHTML=a)}catch(f){this.empty().append(a)}}return this},replaceWith:function(a){if(this[0]&&this[0].parentNode){if(d.isFunction(a))return this.each(function(b){var c=d(this),e=c.html();c.replaceWith(a.call(this,b,e))});typeof a!=="string"&&(a=d(a).detach());return this.each(function(){var b=this.nextSibling,c=this.parentNode;d(this).remove(),b?d(b).before(a):d(c).append(a)})}return this.pushStack(d(d.isFunction(a)?a():a),"replaceWith",a)},detach:function(a){return this.remove(a,!0)},domManip:function(a,c,e){var f,g,h,i,j=a[0],k=[];if(!d.support.checkClone&&arguments.length===3&&typeof j==="string"&&W.test(j))return this.each(function(){d(this).domManip(a,c,e,!0)});if(d.isFunction(j))return this.each(function(f){var g=d(this);a[0]=j.call(this,f,c?g.html():b),g.domManip(a,c,e)});if(this[0]){i=j&&j.parentNode,d.support.parentNode&&i&&i.nodeType===11&&i.childNodes.length===this.length?f={fragment:i}:f=d.buildFragment(a,this,k),h=f.fragment,h.childNodes.length===1?g=h=h.firstChild:g=h.firstChild;if(g){c=c&&d.nodeName(g,"tr");for(var l=0,m=this.length,n=m-1;l<m;l++)e.call(c?Y(this[l],g):this[l],f.cacheable||m>1&&l<n?d.clone(h,!0,!0):h)}k.length&&d.each(k,ba)}return this}}),d.buildFragment=function(a,b,e){var f,g,h,i=b&&b[0]?b[0].ownerDocument||b[0]:c;a.length===1&&typeof a[0]==="string"&&a[0].length<512&&i===c&&a[0].charAt(0)==="<"&&!V.test(a[0])&&(d.support.checkClone||!W.test(a[0]))&&(g=!0,h=d.fragments[a[0]],h&&(h!==1&&(f=h))),f||(f=i.createDocumentFragment(),d.clean(a,i,f,e)),g&&(d.fragments[a[0]]=h?f:1);return{fragment:f,cacheable:g}},d.fragments={},d.each({appendTo:"append",prependTo:"prepend",insertBefore:"before",insertAfter:"after",replaceAll:"replaceWith"},function(a,b){d.fn[a]=function(c){var e=[],f=d(c),g=this.length===1&&this[0].parentNode;if(g&&g.nodeType===11&&g.childNodes.length===1&&f.length===1){f[b](this[0]);return this}for(var h=0,i=f.length;h<i;h++){var j=(h>0?this.clone(!0):this).get();d(f[h])[b](j),e=e.concat(j)}return this.pushStack(e,a,f.selector)}}),d.extend({clone:function(a,b,c){var e=a.cloneNode(!0),f,g,h;if((!d.support.noCloneEvent||!d.support.noCloneChecked)&&(a.nodeType===1||a.nodeType===11)&&!d.isXMLDoc(a)){$(a,e),f=_(a),g=_(e);for(h=0;f[h];++h)$(f[h],g[h])}if(b){Z(a,e);if(c){f=_(a),g=_(e);for(h=0;f[h];++h)Z(f[h],g[h])}}return e},clean:function(a,b,e,f){b=b||c,typeof b.createElement==="undefined"&&(b=b.ownerDocument||b[0]&&b[0].ownerDocument||c);var g=[];for(var h=0,i;(i=a[h])!=null;h++){typeof i==="number"&&(i+="");if(!i)continue;if(typeof i!=="string"||U.test(i)){if(typeof i==="string"){i=i.replace(R,"<$1></$2>");var j=(S.exec(i)||["",""])[1].toLowerCase(),k=X[j]||X._default,l=k[0],m=b.createElement("div");m.innerHTML=k[1]+i+k[2];while(l--)m=m.lastChild;if(!d.support.tbody){var n=T.test(i),o=j==="table"&&!n?m.firstChild&&m.firstChild.childNodes:k[1]==="<table>"&&!n?m.childNodes:[];for(var p=o.length-1;p>=0;--p)d.nodeName(o[p],"tbody")&&!o[p].childNodes.length&&o[p].parentNode.removeChild(o[p])}!d.support.leadingWhitespace&&Q.test(i)&&m.insertBefore(b.createTextNode(Q.exec(i)[0]),m.firstChild),i=m.childNodes}}else i=b.createTextNode(i);i.nodeType?g.push(i):g=d.merge(g,i)}if(e)for(h=0;g[h];h++)!f||!d.nodeName(g[h],"script")||g[h].type&&g[h].type.toLowerCase()!=="text/javascript"?(g[h].nodeType===1&&g.splice.apply(g,[h+1,0].concat(d.makeArray(g[h].getElementsByTagName("script")))),e.appendChild(g[h])):f.push(g[h].parentNode?g[h].parentNode.removeChild(g[h]):g[h]);return g},cleanData:function(a){var b,c,e=d.cache,f=d.expando,g=d.event.special,h=d.support.deleteExpando;for(var i=0,j;(j=a[i])!=null;i++){if(j.nodeName&&d.noData[j.nodeName.toLowerCase()])continue;c=j[d.expando];if(c){b=e[c]&&e[c][f];if(b&&b.events){for(var k in b.events)g[k]?d.event.remove(j,k):d.removeEvent(j,k,b.handle);b.handle&&(b.handle.elem=null)}h?delete j[d.expando]:j.removeAttribute&&j.removeAttribute(d.expando),delete e[c]}}}});var bb=/alpha\([^)]*\)/i,bc=/opacity=([^)]*)/,bd=/-([a-z])/ig,be=/([A-Z])/g,bf=/^-?\d+(?:px)?$/i,bg=/^-?\d/,bh={position:"absolute",visibility:"hidden",display:"block"},bi=["Left","Right"],bj=["Top","Bottom"],bk,bl,bm,bn=function(a,b){return b.toUpperCase()};d.fn.css=function(a,c){if(arguments.length===2&&c===b)return this;return d.access(this,a,c,!0,function(a,c,e){return e!==b?d.style(a,c,e):d.css(a,c)})},d.extend({cssHooks:{opacity:{get:function(a,b){if(b){var c=bk(a,"opacity","opacity");return c===""?"1":c}return a.style.opacity}}},cssNumber:{zIndex:!0,fontWeight:!0,opacity:!0,zoom:!0,lineHeight:!0},cssProps:{"float":d.support.cssFloat?"cssFloat":"styleFloat"},style:function(a,c,e,f){if(a&&a.nodeType!==3&&a.nodeType!==8&&a.style){var g,h=d.camelCase(c),i=a.style,j=d.cssHooks[h];c=d.cssProps[h]||h;if(e===b){if(j&&"get"in j&&(g=j.get(a,!1,f))!==b)return g;return i[c]}if(typeof e==="number"&&isNaN(e)||e==null)return;typeof e==="number"&&!d.cssNumber[h]&&(e+="px");if(!j||!("set"in j)||(e=j.set(a,e))!==b)try{i[c]=e}catch(k){}}},css:function(a,c,e){var f,g=d.camelCase(c),h=d.cssHooks[g];c=d.cssProps[g]||g;if(h&&"get"in h&&(f=h.get(a,!0,e))!==b)return f;if(bk)return bk(a,c,g)},swap:function(a,b,c){var d={};for(var e in b)d[e]=a.style[e],a.style[e]=b[e];c.call(a);for(e in b)a.style[e]=d[e]},camelCase:function(a){return a.replace(bd,bn)}}),d.curCSS=d.css,d.each(["height","width"],function(a,b){d.cssHooks[b]={get:function(a,c,e){var f;if(c){a.offsetWidth!==0?f=bo(a,b,e):d.swap(a,bh,function(){f=bo(a,b,e)});if(f<=0){f=bk(a,b,b),f==="0px"&&bm&&(f=bm(a,b,b));if(f!=null)return f===""||f==="auto"?"0px":f}if(f<0||f==null){f=a.style[b];return f===""||f==="auto"?"0px":f}return typeof f==="string"?f:f+"px"}},set:function(a,b){if(!bf.test(b))return b;b=parseFloat(b);if(b>=0)return b+"px"}}}),d.support.opacity||(d.cssHooks.opacity={get:function(a,b){return bc.test((b&&a.currentStyle?a.currentStyle.filter:a.style.filter)||"")?parseFloat(RegExp.$1)/100+"":b?"1":""},set:function(a,b){var c=a.style;c.zoom=1;var e=d.isNaN(b)?"":"alpha(opacity="+b*100+")",f=c.filter||"";c.filter=bb.test(f)?f.replace(bb,e):c.filter+" "+e}}),c.defaultView&&c.defaultView.getComputedStyle&&(bl=function(a,c,e){var f,g,h;e=e.replace(be,"-$1").toLowerCase();if(!(g=a.ownerDocument.defaultView))return b;if(h=g.getComputedStyle(a,null))f=h.getPropertyValue(e),f===""&&!d.contains(a.ownerDocument.documentElement,a)&&(f=d.style(a,e));return f}),c.documentElement.currentStyle&&(bm=function(a,b){var c,d=a.currentStyle&&a.currentStyle[b],e=a.runtimeStyle&&a.runtimeStyle[b],f=a.style;!bf.test(d)&&bg.test(d)&&(c=f.left,e&&(a.runtimeStyle.left=a.currentStyle.left),f.left=b==="fontSize"?"1em":d||0,d=f.pixelLeft+"px",f.left=c,e&&(a.runtimeStyle.left=e));return d===""?"auto":d}),bk=bl||bm,d.expr&&d.expr.filters&&(d.expr.filters.hidden=function(a){var b=a.offsetWidth,c=a.offsetHeight;return b===0&&c===0||!d.support.reliableHiddenOffsets&&(a.style.display||d.css(a,"display"))==="none"},d.expr.filters.visible=function(a){return!d.expr.filters.hidden(a)});var bp=/%20/g,bq=/\[\]$/,br=/\r?\n/g,bs=/#.*$/,bt=/^(.*?):[ \t]*([^\r\n]*)\r?$/mg,bu=/^(?:color|date|datetime|email|hidden|month|number|password|range|search|tel|text|time|url|week)$/i,bv=/(?:^file|^widget|\-extension):$/,bw=/^(?:GET|HEAD)$/,bx=/^\/\//,by=/\?/,bz=/<script\b[^<]*(?:(?!<\/script>)<[^<]*)*<\/script>/gi,bA=/^(?:select|textarea)/i,bB=/\s+/,bC=/([?&])_=[^&]*/,bD=/(^|\-)([a-z])/g,bE=function(a,b,c){return b+c.toUpperCase()},bF=/^([\w\+\.\-]+:)\/\/([^\/?#:]*)(?::(\d+))?/,bG=d.fn.load,bH={},bI={},bJ,bK;try{bJ=c.location.href}catch(bL){bJ=c.createElement("a"),bJ.href="",bJ=bJ.href}bK=bF.exec(bJ.toLowerCase()),d.fn.extend({load:function(a,c,e){if(typeof a!=="string"&&bG)return bG.apply(this,arguments);if(!this.length)return this;var f=a.indexOf(" ");if(f>=0){var g=a.slice(f,a.length);a=a.slice(0,f)}var h="GET";c&&(d.isFunction(c)?(e=c,c=b):typeof c==="object"&&(c=d.param(c,d.ajaxSettings.traditional),h="POST"));var i=this;d.ajax({url:a,type:h,dataType:"html",data:c,complete:function(a,b,c){c=a.responseText,a.isResolved()&&(a.done(function(a){c=a}),i.html(g?d("<div>").append(c.replace(bz,"")).find(g):c)),e&&i.each(e,[c,b,a])}});return this},serialize:function(){return d.param(this.serializeArray())},serializeArray:function(){return this.map(function(){return this.elements?d.makeArray(this.elements):this}).filter(function(){return this.name&&!this.disabled&&(this.checked||bA.test(this.nodeName)||bu.test(this.type))}).map(function(a,b){var c=d(this).val();return c==null?null:d.isArray(c)?d.map(c,function(a,c){return{name:b.name,value:a.replace(br,"\r\n")}}):{name:b.name,value:c.replace(br,"\r\n")}}).get()}}),d.each("ajaxStart ajaxStop ajaxComplete ajaxError ajaxSuccess ajaxSend".split(" "),function(a,b){d.fn[b]=function(a){return this.bind(b,a)}}),d.each(["get","post"],function(a,c){d[c]=function(a,e,f,g){d.isFunction(e)&&(g=g||f,f=e,e=b);return d.ajax({type:c,url:a,data:e,success:f,dataType:g})}}),d.extend({getScript:function(a,c){return d.get(a,b,c,"script")},getJSON:function(a,b,c){return d.get(a,b,c,"json")},ajaxSetup:function(a,b){b?d.extend(!0,a,d.ajaxSettings,b):(b=a,a=d.extend(!0,d.ajaxSettings,b));for(var c in {context:1,url:1})c in b?a[c]=b[c]:c in d.ajaxSettings&&(a[c]=d.ajaxSettings[c]);return a},ajaxSettings:{url:bJ,isLocal:bv.test(bK[1]),global:!0,type:"GET",contentType:"application/x-www-form-urlencoded",processData:!0,async:!0,accepts:{xml:"application/xml, text/xml",html:"text/html",text:"text/plain",json:"application/json, text/javascript","*":"*/*"},contents:{xml:/xml/,html:/html/,json:/json/},responseFields:{xml:"responseXML",text:"responseText"},converters:{"* text":a.String,"text html":!0,"text json":d.parseJSON,"text xml":d.parseXML}},ajaxPrefilter:bM(bH),ajaxTransport:bM(bI),ajax:function(a,c){function v(a,c,l,n){if(r!==2){r=2,p&&clearTimeout(p),o=b,m=n||"",u.readyState=a?4:0;var q,t,v,w=l?bP(e,u,l):b,x,y;if(a>=200&&a<300||a===304){if(e.ifModified){if(x=u.getResponseHeader("Last-Modified"))d.lastModified[k]=x;if(y=u.getResponseHeader("Etag"))d.etag[k]=y}if(a===304)c="notmodified",q=!0;else try{t=bQ(e,w),c="success",q=!0}catch(z){c="parsererror",v=z}}else{v=c;if(!c||a)c="error",a<0&&(a=0)}u.status=a,u.statusText=c,q?h.resolveWith(f,[t,c,u]):h.rejectWith(f,[u,c,v]),u.statusCode(j),j=b,s&&g.trigger("ajax"+(q?"Success":"Error"),[u,e,q?t:v]),i.resolveWith(f,[u,c]),s&&(g.trigger("ajaxComplete",[u,e]),--d.active||d.event.trigger("ajaxStop"))}}typeof a==="object"&&(c=a,a=b),c=c||{};var e=d.ajaxSetup({},c),f=e.context||e,g=f!==e&&(f.nodeType||f instanceof d)?d(f):d.event,h=d.Deferred(),i=d._Deferred(),j=e.statusCode||{},k,l={},m,n,o,p,q,r=0,s,t,u={readyState:0,setRequestHeader:function(a,b){r||(l[a.toLowerCase().replace(bD,bE)]=b);return this},getAllResponseHeaders:function(){return r===2?m:null},getResponseHeader:function(a){var c;if(r===2){if(!n){n={};while(c=bt.exec(m))n[c[1].toLowerCase()]=c[2]}c=n[a.toLowerCase()]}return c===b?null:c},overrideMimeType:function(a){r||(e.mimeType=a);return this},abort:function(a){a=a||"abort",o&&o.abort(a),v(0,a);return this}};h.promise(u),u.success=u.done,u.error=u.fail,u.complete=i.done,u.statusCode=function(a){if(a){var b;if(r<2)for(b in a)j[b]=[j[b],a[b]];else b=a[u.status],u.then(b,b)}return this},e.url=((a||e.url)+"").replace(bs,"").replace(bx,bK[1]+"//"),e.dataTypes=d.trim(e.dataType||"*").toLowerCase().split(bB),e.crossDomain||(q=bF.exec(e.url.toLowerCase()),e.crossDomain=q&&(q[1]!=bK[1]||q[2]!=bK[2]||(q[3]||(q[1]==="http:"?80:443))!=(bK[3]||(bK[1]==="http:"?80:443)))),e.data&&e.processData&&typeof e.data!=="string"&&(e.data=d.param(e.data,e.traditional)),bN(bH,e,c,u);if(r===2)return!1;s=e.global,e.type=e.type.toUpperCase(),e.hasContent=!bw.test(e.type),s&&d.active++===0&&d.event.trigger("ajaxStart");if(!e.hasContent){e.data&&(e.url+=(by.test(e.url)?"&":"?")+e.data),k=e.url;if(e.cache===!1){var w=d.now(),x=e.url.replace(bC,"$1_="+w);e.url=x+(x===e.url?(by.test(e.url)?"&":"?")+"_="+w:"")}}if(e.data&&e.hasContent&&e.contentType!==!1||c.contentType)l["Content-Type"]=e.contentType;e.ifModified&&(k=k||e.url,d.lastModified[k]&&(l["If-Modified-Since"]=d.lastModified[k]),d.etag[k]&&(l["If-None-Match"]=d.etag[k])),l.Accept=e.dataTypes[0]&&e.accepts[e.dataTypes[0]]?e.accepts[e.dataTypes[0]]+(e.dataTypes[0]!=="*"?", */*; q=0.01":""):e.accepts["*"];for(t in e.headers)u.setRequestHeader(t,e.headers[t]);if(e.beforeSend&&(e.beforeSend.call(f,u,e)===!1||r===2)){u.abort();return!1}for(t in {success:1,error:1,complete:1})u[t](e[t]);o=bN(bI,e,c,u);if(o){u.readyState=1,s&&g.trigger("ajaxSend",[u,e]),e.async&&e.timeout>0&&(p=setTimeout(function(){u.abort("timeout")},e.timeout));try{r=1,o.send(l,v)}catch(y){status<2?v(-1,y):d.error(y)}}else v(-1,"No Transport");return u},param:function(a,c){var e=[],f=function(a,b){b=d.isFunction(b)?b():b,e[e.length]=encodeURIComponent(a)+"="+encodeURIComponent(b)};c===b&&(c=d.ajaxSettings.traditional);if(d.isArray(a)||a.jquery&&!d.isPlainObject(a))d.each(a,function(){f(this.name,this.value)});else for(var g in a)bO(g,a[g],c,f);return e.join("&").replace(bp,"+")}}),d.extend({active:0,lastModified:{},etag:{}});var bR=d.now(),bS=/(\=)\?(&|$)|()\?\?()/i;d.ajaxSetup({jsonp:"callback",jsonpCallback:function(){return d.expando+"_"+bR++}}),d.ajaxPrefilter("json jsonp",function(b,c,e){var f=typeof b.data==="string";if(b.dataTypes[0]==="jsonp"||c.jsonpCallback||c.jsonp!=null||b.jsonp!==!1&&(bS.test(b.url)||f&&bS.test(b.data))){var g,h=b.jsonpCallback=d.isFunction(b.jsonpCallback)?b.jsonpCallback():b.jsonpCallback,i=a[h],j=b.url,k=b.data,l="$1"+h+"$2",m=function(){a[h]=i,g&&d.isFunction(i)&&a[h](g[0])};b.jsonp!==!1&&(j=j.replace(bS,l),b.url===j&&(f&&(k=k.replace(bS,l)),b.data===k&&(j+=(/\?/.test(j)?"&":"?")+b.jsonp+"="+h))),b.url=j,b.data=k,a[h]=function(a){g=[a]},e.then(m,m),b.converters["script json"]=function(){g||d.error(h+" was not called");return g[0]},b.dataTypes[0]="json";return"script"}}),d.ajaxSetup({accepts:{script:"text/javascript, application/javascript, application/ecmascript, application/x-ecmascript"},contents:{script:/javascript|ecmascript/},converters:{"text script":function(a){d.globalEval(a);return a}}}),d.ajaxPrefilter("script",function(a){a.cache===b&&(a.cache=!1),a.crossDomain&&(a.type="GET",a.global=!1)}),d.ajaxTransport("script",function(a){if(a.crossDomain){var d,e=c.head||c.getElementsByTagName("head")[0]||c.documentElement;return{send:function(f,g){d=c.createElement("script"),d.async="async",a.scriptCharset&&(d.charset=a.scriptCharset),d.src=a.url,d.onload=d.onreadystatechange=function(a,c){if(!d.readyState||/loaded|complete/.test(d.readyState))d.onload=d.onreadystatechange=null,e&&d.parentNode&&e.removeChild(d),d=b,c||g(200,"success")},e.insertBefore(d,e.firstChild)},abort:function(){d&&d.onload(0,1)}}}});var bT=d.now(),bU,bV;d.ajaxSettings.xhr=a.ActiveXObject?function(){return!this.isLocal&&bX()||bY()}:bX,bV=d.ajaxSettings.xhr(),d.support.ajax=!!bV,d.support.cors=bV&&"withCredentials"in bV,bV=b,d.support.ajax&&d.ajaxTransport(function(a){if(!a.crossDomain||d.support.cors){var c;return{send:function(e,f){var g=a.xhr(),h,i;a.username?g.open(a.type,a.url,a.async,a.username,a.password):g.open(a.type,a.url,a.async);if(a.xhrFields)for(i in a.xhrFields)g[i]=a.xhrFields[i];a.mimeType&&g.overrideMimeType&&g.overrideMimeType(a.mimeType),(!a.crossDomain||a.hasContent)&&!e["X-Requested-With"]&&(e["X-Requested-With"]="XMLHttpRequest");try{for(i in e)g.setRequestHeader(i,e[i])}catch(j){}g.send(a.hasContent&&a.data||null),c=function(e,i){var j,k,l,m,n;try{if(c&&(i||g.readyState===4)){c=b,h&&(g.onreadystatechange=d.noop,delete bU[h]);if(i)g.readyState!==4&&g.abort();else{j=g.status,l=g.getAllResponseHeaders(),m={},n=g.responseXML,n&&n.documentElement&&(m.xml=n),m.text=g.responseText;try{k=g.statusText}catch(o){k=""}j||!a.isLocal||a.crossDomain?j===1223&&(j=204):j=m.text?200:404}}}catch(p){i||f(-1,p)}m&&f(j,k,m,l)},a.async&&g.readyState!==4?(bU||(bU={},bW()),h=bT++,g.onreadystatechange=bU[h]=c):c()},abort:function(){c&&c(0,1)}}}});var bZ={},b$=/^(?:toggle|show|hide)$/,b_=/^([+\-]=)?([\d+.\-]+)([a-z%]*)$/i,ca,cb=[["height","marginTop","marginBottom","paddingTop","paddingBottom"],["width","marginLeft","marginRight","paddingLeft","paddingRight"],["opacity"]];d.fn.extend({show:function(a,b,c){var e,f;if(a||a===0)return this.animate(cc("show",3),a,b,c);for(var g=0,h=this.length;g<h;g++)e=this[g],f=e.style.display,!d._data(e,"olddisplay")&&f==="none"&&(f=e.style.display=""),f===""&&d.css(e,"display")==="none"&&d._data(e,"olddisplay",cd(e.nodeName));for(g=0;g<h;g++){e=this[g],f=e.style.display;if(f===""||f==="none")e.style.display=d._data(e,"olddisplay")||""}return this},hide:function(a,b,c){if(a||a===0)return this.animate(cc("hide",3),a,b,c);for(var e=0,f=this.length;e<f;e++){var g=d.css(this[e],"display");g!=="none"&&!d._data(this[e],"olddisplay")&&d._data(this[e],"olddisplay",g)}for(e=0;e<f;e++)this[e].style.display="none";return this},_toggle:d.fn.toggle,toggle:function(a,b,c){var e=typeof a==="boolean";d.isFunction(a)&&d.isFunction(b)?this._toggle.apply(this,arguments):a==null||e?this.each(function(){var b=e?a:d(this).is(":hidden");d(this)[b?"show":"hide"]()}):this.animate(cc("toggle",3),a,b,c);return this},fadeTo:function(a,b,c,d){return this.filter(":hidden").css("opacity",0).show().end().animate({opacity:b},a,c,d)},animate:function(a,b,c,e){var f=d.speed(b,c,e);if(d.isEmptyObject(a))return this.each(f.complete);return this[f.queue===!1?"each":"queue"](function(){var b=d.extend({},f),c,e=this.nodeType===1,g=e&&d(this).is(":hidden"),h=this;for(c in a){var i=d.camelCase(c);c!==i&&(a[i]=a[c],delete a[c],c=i);if(a[c]==="hide"&&g||a[c]==="show"&&!g)return b.complete.call(this);if(e&&(c==="height"||c==="width")){b.overflow=[this.style.overflow,this.style.overflowX,this.style.overflowY];if(d.css(this,"display")==="inline"&&d.css(this,"float")==="none")if(d.support.inlineBlockNeedsLayout){var j=cd(this.nodeName);j==="inline"?this.style.display="inline-block":(this.style.display="inline",this.style.zoom=1)}else this.style.display="inline-block"}d.isArray(a[c])&&((b.specialEasing=b.specialEasing||{})[c]=a[c][1],a[c]=a[c][0])}b.overflow!=null&&(this.style.overflow="hidden"),b.curAnim=d.extend({},a),d.each(a,function(c,e){var f=new d.fx(h,b,c);if(b$.test(e))f[e==="toggle"?g?"show":"hide":e](a);else{var i=b_.exec(e),j=f.cur();if(i){var k=parseFloat(i[2]),l=i[3]||(d.cssNumber[c]?"":"px");l!=="px"&&(d.style(h,c,(k||1)+l),j=(k||1)/f.cur()*j,d.style(h,c,j+l)),i[1]&&(k=(i[1]==="-="?-1:1)*k+j),f.custom(j,k,l)}else f.custom(j,e,"")}});return!0})},stop:function(a,b){var c=d.timers;a&&this.queue([]),this.each(function(){for(var a=c.length-1;a>=0;a--)c[a].elem===this&&(b&&c[a](!0),c.splice(a,1))}),b||this.dequeue();return this}}),d.each({slideDown:cc("show",1),slideUp:cc("hide",1),slideToggle:cc("toggle",1),fadeIn:{opacity:"show"},fadeOut:{opacity:"hide"},fadeToggle:{opacity:"toggle"}},function(a,b){d.fn[a]=function(a,c,d){return this.animate(b,a,c,d)}}),d.extend({speed:function(a,b,c){var e=a&&typeof a==="object"?d.extend({},a):{complete:c||!c&&b||d.isFunction(a)&&a,duration:a,easing:c&&b||b&&!d.isFunction(b)&&b};e.duration=d.fx.off?0:typeof e.duration==="number"?e.duration:e.duration in d.fx.speeds?d.fx.speeds[e.duration]:d.fx.speeds._default,e.old=e.complete,e.complete=function(){e.queue!==!1&&d(this).dequeue(),d.isFunction(e.old)&&e.old.call(this)};return e},easing:{linear:function(a,b,c,d){return c+d*a},swing:function(a,b,c,d){return(-Math.cos(a*Math.PI)/2+.5)*d+c}},timers:[],fx:function(a,b,c){this.options=b,this.elem=a,this.prop=c,b.orig||(b.orig={})}}),d.fx.prototype={update:function(){this.options.step&&this.options.step.call(this.elem,this.now,this),(d.fx.step[this.prop]||d.fx.step._default)(this)},cur:function(){if(this.elem[this.prop]!=null&&(!this.elem.style||this.elem.style[this.prop]==null))return this.elem[this.prop];var a,b=d.css(this.elem,this.prop);return isNaN(a=parseFloat(b))?!b||b==="auto"?0:b:a},custom:function(a,b,c){function g(a){return e.step(a)}var e=this,f=d.fx;this.startTime=d.now(),this.start=a,this.end=b,this.unit=c||this.unit||(d.cssNumber[this.prop]?"":"px"),this.now=this.start,this.pos=this.state=0,g.elem=this.elem,g()&&d.timers.push(g)&&!ca&&(ca=setInterval(f.tick,f.interval))},show:function(){this.options.orig[this.prop]=d.style(this.elem,this.prop),this.options.show=!0,this.custom(this.prop==="width"||this.prop==="height"?1:0,this.cur()),d(this.elem).show()},hide:function(){this.options.orig[this.prop]=d.style(this.elem,this.prop),this.options.hide=!0,this.custom(this.cur(),0)},step:function(a){var b=d.now(),c=!0;if(a||b>=this.options.duration+this.startTime){this.now=this.end,this.pos=this.state=1,this.update(),this.options.curAnim[this.prop]=!0;for(var e in this.options.curAnim)this.options.curAnim[e]!==!0&&(c=!1);if(c){if(this.options.overflow!=null&&!d.support.shrinkWrapBlocks){var f=this.elem,g=this.options;d.each(["","X","Y"],function(a,b){f.style["overflow"+b]=g.overflow[a]})}this.options.hide&&d(this.elem).hide();if(this.options.hide||this.options.show)for(var h in this.options.curAnim)d.style(this.elem,h,this.options.orig[h]);this.options.complete.call(this.elem)}return!1}var i=b-this.startTime;this.state=i/this.options.duration;var j=this.options.specialEasing&&this.options.specialEasing[this.prop],k=this.options.easing||(d.easing.swing?"swing":"linear");this.pos=d.easing[j||k](this.state,i,0,1,this.options.duration),this.now=this.start+(this.end-this.start)*this.pos,this.update();return!0}},d.extend(d.fx,{tick:function(){var a=d.timers;for(var b=0;b<a.length;b++)a[b]()||a.splice(b--,1);a.length||d.fx.stop()},interval:13,stop:function(){clearInterval(ca),ca=null},speeds:{slow:600,fast:200,_default:400},step:{opacity:function(a){d.style(a.elem,"opacity",a.now)},_default:function(a){a.elem.style&&a.elem.style[a.prop]!=null?a.elem.style[a.prop]=(a.prop==="width"||a.prop==="height"?Math.max(0,a.now):a.now)+a.unit:a.elem[a.prop]=a.now}}}),d.expr&&d.expr.filters&&(d.expr.filters.animated=function(a){return d.grep(d.timers,function(b){return a===b.elem}).length});var ce=/^t(?:able|d|h)$/i,cf=/^(?:body|html)$/i;"getBoundingClientRect"in c.documentElement?d.fn.offset=function(a){var b=this[0],c;if(a)return this.each(function(b){d.offset.setOffset(this,a,b)});if(!b||!b.ownerDocument)return null;if(b===b.ownerDocument.body)return d.offset.bodyOffset(b);try{c=b.getBoundingClientRect()}catch(e){}var f=b.ownerDocument,g=f.documentElement;if(!c||!d.contains(g,b))return c?{top:c.top,left:c.left}:{top:0,left:0};var h=f.body,i=cg(f),j=g.clientTop||h.clientTop||0,k=g.clientLeft||h.clientLeft||0,l=i.pageYOffset||d.support.boxModel&&g.scrollTop||h.scrollTop,m=i.pageXOffset||d.support.boxModel&&g.scrollLeft||h.scrollLeft,n=c.top+l-j,o=c.left+m-k;return{top:n,left:o}}:d.fn.offset=function(a){var b=this[0];if(a)return this.each(function(b){d.offset.setOffset(this,a,b)});if(!b||!b.ownerDocument)return null;if(b===b.ownerDocument.body)return d.offset.bodyOffset(b);d.offset.initialize();var c,e=b.offsetParent,f=b,g=b.ownerDocument,h=g.documentElement,i=g.body,j=g.defaultView,k=j?j.getComputedStyle(b,null):b.currentStyle,l=b.offsetTop,m=b.offsetLeft;while((b=b.parentNode)&&b!==i&&b!==h){if(d.offset.supportsFixedPosition&&k.position==="fixed")break;c=j?j.getComputedStyle(b,null):b.currentStyle,l-=b.scrollTop,m-=b.scrollLeft,b===e&&(l+=b.offsetTop,m+=b.offsetLeft,d.offset.doesNotAddBorder&&(!d.offset.doesAddBorderForTableAndCells||!ce.test(b.nodeName))&&(l+=parseFloat(c.borderTopWidth)||0,m+=parseFloat(c.borderLeftWidth)||0),f=e,e=b.offsetParent),d.offset.subtractsBorderForOverflowNotVisible&&c.overflow!=="visible"&&(l+=parseFloat(c.borderTopWidth)||0,m+=parseFloat(c.borderLeftWidth)||0),k=c}if(k.position==="relative"||k.position==="static")l+=i.offsetTop,m+=i.offsetLeft;d.offset.supportsFixedPosition&&k.position==="fixed"&&(l+=Math.max(h.scrollTop,i.scrollTop),m+=Math.max(h.scrollLeft,i.scrollLeft));return{top:l,left:m}},d.offset={initialize:function(){var a=c.body,b=c.createElement("div"),e,f,g,h,i=parseFloat(d.css(a,"marginTop"))||0,j="<div style='position:absolute;top:0;left:0;margin:0;border:5px solid #000;padding:0;width:1px;height:1px;'><div></div></div><table style='position:absolute;top:0;left:0;margin:0;border:5px solid #000;padding:0;width:1px;height:1px;' cellpadding='0' cellspacing='0'><tr><td></td></tr></table>";d.extend(b.style,{position:"absolute",top:0,left:0,margin:0,border:0,width:"1px",height:"1px",visibility:"hidden"}),b.innerHTML=j,a.insertBefore(b,a.firstChild),e=b.firstChild,f=e.firstChild,h=e.nextSibling.firstChild.firstChild,this.doesNotAddBorder=f.offsetTop!==5,this.doesAddBorderForTableAndCells=h.offsetTop===5,f.style.position="fixed",f.style.top="20px",this.supportsFixedPosition=f.offsetTop===20||f.offsetTop===15,f.style.position=f.style.top="",e.style.overflow="hidden",e.style.position="relative",this.subtractsBorderForOverflowNotVisible=f.offsetTop===-5,this.doesNotIncludeMarginInBodyOffset=a.offsetTop!==i,a.removeChild(b),a=b=e=f=g=h=null,d.offset.initialize=d.noop},bodyOffset:function(a){var b=a.offsetTop,c=a.offsetLeft;d.offset.initialize(),d.offset.doesNotIncludeMarginInBodyOffset&&(b+=parseFloat(d.css(a,"marginTop"))||0,c+=parseFloat(d.css(a,"marginLeft"))||0);return{top:b,left:c}},setOffset:function(a,b,c){var e=d.css(a,"position");e==="static"&&(a.style.position="relative");var f=d(a),g=f.offset(),h=d.css(a,"top"),i=d.css(a,"left"),j=e==="absolute"&&d.inArray("auto",[h,i])>-1,k={},l={},m,n;j&&(l=f.position()),m=j?l.top:parseInt(h,10)||0,n=j?l.left:parseInt(i,10)||0,d.isFunction(b)&&(b=b.call(a,c,g)),b.top!=null&&(k.top=b.top-g.top+m),b.left!=null&&(k.left=b.left-g.left+n),"using"in b?b.using.call(a,k):f.css(k)}},d.fn.extend({position:function(){if(!this[0])return null;var a=this[0],b=this.offsetParent(),c=this.offset(),e=cf.test(b[0].nodeName)?{top:0,left:0}:b.offset();c.top-=parseFloat(d.css(a,"marginTop"))||0,c.left-=parseFloat(d.css(a,"marginLeft"))||0,e.top+=parseFloat(d.css(b[0],"borderTopWidth"))||0,e.left+=parseFloat(d.css(b[0],"borderLeftWidth"))||0;return{top:c.top-e.top,left:c.left-e.left}},offsetParent:function(){return this.map(function(){var a=this.offsetParent||c.body;while(a&&(!cf.test(a.nodeName)&&d.css(a,"position")==="static"))a=a.offsetParent;return a})}}),d.each(["Left","Top"],function(a,c){var e="scroll"+c;d.fn[e]=function(c){var f=this[0],g;if(!f)return null;if(c!==b)return this.each(function(){g=cg(this),g?g.scrollTo(a?d(g).scrollLeft():c,a?c:d(g).scrollTop()):this[e]=c});g=cg(f);return g?"pageXOffset"in g?g[a?"pageYOffset":"pageXOffset"]:d.support.boxModel&&g.document.documentElement[e]||g.document.body[e]:f[e]}}),d.each(["Height","Width"],function(a,c){var e=c.toLowerCase();d.fn["inner"+c]=function(){return this[0]?parseFloat(d.css(this[0],e,"padding")):null},d.fn["outer"+c]=function(a){return this[0]?parseFloat(d.css(this[0],e,a?"margin":"border")):null},d.fn[e]=function(a){var f=this[0];if(!f)return a==null?null:this;if(d.isFunction(a))return this.each(function(b){var c=d(this);c[e](a.call(this,b,c[e]()))});if(d.isWindow(f)){var g=f.document.documentElement["client"+c];return f.document.compatMode==="CSS1Compat"&&g||f.document.body["client"+c]||g}if(f.nodeType===9)return Math.max(f.documentElement["client"+c],f.body["scroll"+c],f.documentElement["scroll"+c],f.body["offset"+c],f.documentElement["offset"+c]);if(a===b){var h=d.css(f,e),i=parseFloat(h);return d.isNaN(i)?h:i}return this.css(e,typeof a==="string"?a:a+"px")}}),INTELNAV.jQuery_1_5_1=d})(window);
};

var load_jquery1_10_1 = function() {
	// ! jQuery v1.10.1 | (c) 2005, 2013 jQuery Foundation, Inc. | jquery.org/license
	// @ source Mapping URL=jquery-1.10.1.min.map
	(function(e,t){var n,r,i=typeof t,o=e.location,a=e.document,s=a.documentElement,l=e.jQuery,u=e.$,c={},p=[],f="1.10.1",d=p.concat,h=p.push,g=p.slice,m=p.indexOf,y=c.toString,v=c.hasOwnProperty,b=f.trim,x=function(e,t){return new x.fn.init(e,t,r)},w=/[+-]?(?:\d*\.|)\d+(?:[eE][+-]?\d+|)/.source,T=/\S+/g,C=/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g,N=/^(?:\s*(<[\w\W]+>)[^>]*|#([\w-]*))$/,k=/^<(\w+)\s*\/?>(?:<\/\1>|)$/,E=/^[\],:{}\s]*$/,S=/(?:^|:|,)(?:\s*\[)+/g,A=/\\(?:["\\\/bfnrt]|u[\da-fA-F]{4})/g,j=/"[^"\\\r\n]*"|true|false|null|-?(?:\d+\.|)\d+(?:[eE][+-]?\d+|)/g,D=/^-ms-/,L=/-([\da-z])/gi,H=function(e,t){return t.toUpperCase()},q=function(e){(a.addEventListener||"load"===e.type||"complete"===a.readyState)&&(_(),x.ready())},_=function(){a.addEventListener?(a.removeEventListener("DOMContentLoaded",q,!1),e.removeEventListener("load",q,!1)):(a.detachEvent("onreadystatechange",q),e.detachEvent("onload",q))};x.fn=x.prototype={jquery:f,constructor:x,init:function(e,n,r){var i,o;if(!e)return this;if("string"==typeof e){if(i="<"===e.charAt(0)&&">"===e.charAt(e.length-1)&&e.length>=3?[null,e,null]:N.exec(e),!i||!i[1]&&n)return!n||n.jquery?(n||r).find(e):this.constructor(n).find(e);if(i[1]){if(n=n instanceof x?n[0]:n,x.merge(this,x.parseHTML(i[1],n&&n.nodeType?n.ownerDocument||n:a,!0)),k.test(i[1])&&x.isPlainObject(n))for(i in n)x.isFunction(this[i])?this[i](n[i]):this.attr(i,n[i]);return this}if(o=a.getElementById(i[2]),o&&o.parentNode){if(o.id!==i[2])return r.find(e);this.length=1,this[0]=o}return this.context=a,this.selector=e,this}return e.nodeType?(this.context=this[0]=e,this.length=1,this):x.isFunction(e)?r.ready(e):(e.selector!==t&&(this.selector=e.selector,this.context=e.context),x.makeArray(e,this))},selector:"",length:0,toArray:function(){return g.call(this)},get:function(e){return null==e?this.toArray():0>e?this[this.length+e]:this[e]},pushStack:function(e){var t=x.merge(this.constructor(),e);return t.prevObject=this,t.context=this.context,t},each:function(e,t){return x.each(this,e,t)},ready:function(e){return x.ready.promise().done(e),this},slice:function(){return this.pushStack(g.apply(this,arguments))},first:function(){return this.eq(0)},last:function(){return this.eq(-1)},eq:function(e){var t=this.length,n=+e+(0>e?t:0);return this.pushStack(n>=0&&t>n?[this[n]]:[])},map:function(e){return this.pushStack(x.map(this,function(t,n){return e.call(t,n,t)}))},end:function(){return this.prevObject||this.constructor(null)},push:h,sort:[].sort,splice:[].splice},x.fn.init.prototype=x.fn,x.extend=x.fn.extend=function(){var e,n,r,i,o,a,s=arguments[0]||{},l=1,u=arguments.length,c=!1;for("boolean"==typeof s&&(c=s,s=arguments[1]||{},l=2),"object"==typeof s||x.isFunction(s)||(s={}),u===l&&(s=this,--l);u>l;l++)if(null!=(o=arguments[l]))for(i in o)e=s[i],r=o[i],s!==r&&(c&&r&&(x.isPlainObject(r)||(n=x.isArray(r)))?(n?(n=!1,a=e&&x.isArray(e)?e:[]):a=e&&x.isPlainObject(e)?e:{},s[i]=x.extend(c,a,r)):r!==t&&(s[i]=r));return s},x.extend({expando:"jQuery"+(f+Math.random()).replace(/\D/g,""),noConflict:function(t){return e.$===x&&(e.$=u),t&&e.jQuery===x&&(e.jQuery=l),x},isReady:!1,readyWait:1,holdReady:function(e){e?x.readyWait++:x.ready(!0)},ready:function(e){if(e===!0?!--x.readyWait:!x.isReady){if(!a.body)return setTimeout(x.ready);x.isReady=!0,e!==!0&&--x.readyWait>0||(n.resolveWith(a,[x]),x.fn.trigger&&x(a).trigger("ready").off("ready"))}},isFunction:function(e){return"function"===x.type(e)},isArray:Array.isArray||function(e){return"array"===x.type(e)},isWindow:function(e){return null!=e&&e==e.window},isNumeric:function(e){return!isNaN(parseFloat(e))&&isFinite(e)},type:function(e){return null==e?e+"":"object"==typeof e||"function"==typeof e?c[y.call(e)]||"object":typeof e},isPlainObject:function(e){var n;if(!e||"object"!==x.type(e)||e.nodeType||x.isWindow(e))return!1;try{if(e.constructor&&!v.call(e,"constructor")&&!v.call(e.constructor.prototype,"isPrototypeOf"))return!1}catch(r){return!1}if(x.support.ownLast)for(n in e)return v.call(e,n);for(n in e);return n===t||v.call(e,n)},isEmptyObject:function(e){var t;for(t in e)return!1;return!0},error:function(e){throw Error(e)},parseHTML:function(e,t,n){if(!e||"string"!=typeof e)return null;"boolean"==typeof t&&(n=t,t=!1),t=t||a;var r=k.exec(e),i=!n&&[];return r?[t.createElement(r[1])]:(r=x.buildFragment([e],t,i),i&&x(i).remove(),x.merge([],r.childNodes))},parseJSON:function(n){return e.JSON&&e.JSON.parse?e.JSON.parse(n):null===n?n:"string"==typeof n&&(n=x.trim(n),n&&E.test(n.replace(A,"@").replace(j,"]").replace(S,"")))?Function("return "+n)():(x.error("Invalid JSON: "+n),t)},parseXML:function(n){var r,i;if(!n||"string"!=typeof n)return null;try{e.DOMParser?(i=new DOMParser,r=i.parseFromString(n,"text/xml")):(r=new ActiveXObject("Microsoft.XMLDOM"),r.async="false",r.loadXML(n))}catch(o){r=t}return r&&r.documentElement&&!r.getElementsByTagName("parsererror").length||x.error("Invalid XML: "+n),r},noop:function(){},globalEval:function(t){t&&x.trim(t)&&(e.execScript||function(t){e.eval.call(e,t)})(t)},camelCase:function(e){return e.replace(D,"ms-").replace(L,H)},nodeName:function(e,t){return e.nodeName&&e.nodeName.toLowerCase()===t.toLowerCase()},each:function(e,t,n){var r,i=0,o=e.length,a=M(e);if(n){if(a){for(;o>i;i++)if(r=t.apply(e[i],n),r===!1)break}else for(i in e)if(r=t.apply(e[i],n),r===!1)break}else if(a){for(;o>i;i++)if(r=t.call(e[i],i,e[i]),r===!1)break}else for(i in e)if(r=t.call(e[i],i,e[i]),r===!1)break;return e},trim:b&&!b.call("\ufeff\u00a0")?function(e){return null==e?"":b.call(e)}:function(e){return null==e?"":(e+"").replace(C,"")},makeArray:function(e,t){var n=t||[];return null!=e&&(M(Object(e))?x.merge(n,"string"==typeof e?[e]:e):h.call(n,e)),n},inArray:function(e,t,n){var r;if(t){if(m)return m.call(t,e,n);for(r=t.length,n=n?0>n?Math.max(0,r+n):n:0;r>n;n++)if(n in t&&t[n]===e)return n}return-1},merge:function(e,n){var r=n.length,i=e.length,o=0;if("number"==typeof r)for(;r>o;o++)e[i++]=n[o];else while(n[o]!==t)e[i++]=n[o++];return e.length=i,e},grep:function(e,t,n){var r,i=[],o=0,a=e.length;for(n=!!n;a>o;o++)r=!!t(e[o],o),n!==r&&i.push(e[o]);return i},map:function(e,t,n){var r,i=0,o=e.length,a=M(e),s=[];if(a)for(;o>i;i++)r=t(e[i],i,n),null!=r&&(s[s.length]=r);else for(i in e)r=t(e[i],i,n),null!=r&&(s[s.length]=r);return d.apply([],s)},guid:1,proxy:function(e,n){var r,i,o;return"string"==typeof n&&(o=e[n],n=e,e=o),x.isFunction(e)?(r=g.call(arguments,2),i=function(){return e.apply(n||this,r.concat(g.call(arguments)))},i.guid=e.guid=e.guid||x.guid++,i):t},access:function(e,n,r,i,o,a,s){var l=0,u=e.length,c=null==r;if("object"===x.type(r)){o=!0;for(l in r)x.access(e,n,l,r[l],!0,a,s)}else if(i!==t&&(o=!0,x.isFunction(i)||(s=!0),c&&(s?(n.call(e,i),n=null):(c=n,n=function(e,t,n){return c.call(x(e),n)})),n))for(;u>l;l++)n(e[l],r,s?i:i.call(e[l],l,n(e[l],r)));return o?e:c?n.call(e):u?n(e[0],r):a},now:function(){return(new Date).getTime()},swap:function(e,t,n,r){var i,o,a={};for(o in t)a[o]=e.style[o],e.style[o]=t[o];i=n.apply(e,r||[]);for(o in t)e.style[o]=a[o];return i}}),x.ready.promise=function(t){if(!n)if(n=x.Deferred(),"complete"===a.readyState)setTimeout(x.ready);else if(a.addEventListener)a.addEventListener("DOMContentLoaded",q,!1),e.addEventListener("load",q,!1);else{a.attachEvent("onreadystatechange",q),e.attachEvent("onload",q);var r=!1;try{r=null==e.frameElement&&a.documentElement}catch(i){}r&&r.doScroll&&function o(){if(!x.isReady){try{r.doScroll("left")}catch(e){return setTimeout(o,50)}_(),x.ready()}}()}return n.promise(t)},x.each("Boolean Number String Function Array Date RegExp Object Error".split(" "),function(e,t){c["[object "+t+"]"]=t.toLowerCase()});function M(e){var t=e.length,n=x.type(e);return x.isWindow(e)?!1:1===e.nodeType&&t?!0:"array"===n||"function"!==n&&(0===t||"number"==typeof t&&t>0&&t-1 in e)}r=x(a),function(e,t){var n,r,i,o,a,s,l,u,c,p,f,d,h,g,m,y,v,b="sizzle"+-new Date,w=e.document,T=0,C=0,N=lt(),k=lt(),E=lt(),S=!1,A=function(){return 0},j=typeof t,D=1<<31,L={}.hasOwnProperty,H=[],q=H.pop,_=H.push,M=H.push,O=H.slice,F=H.indexOf||function(e){var t=0,n=this.length;for(;n>t;t++)if(this[t]===e)return t;return-1},B="checked|selected|async|autofocus|autoplay|controls|defer|disabled|hidden|ismap|loop|multiple|open|readonly|required|scoped",P="[\\x20\\t\\r\\n\\f]",R="(?:\\\\.|[\\w-]|[^\\x00-\\xa0])+",W=R.replace("w","w#"),$="\\["+P+"*("+R+")"+P+"*(?:([*^$|!~]?=)"+P+"*(?:(['\"])((?:\\\\.|[^\\\\])*?)\\3|("+W+")|)|)"+P+"*\\]",I=":("+R+")(?:\\(((['\"])((?:\\\\.|[^\\\\])*?)\\3|((?:\\\\.|[^\\\\()[\\]]|"+$.replace(3,8)+")*)|.*)\\)|)",z=RegExp("^"+P+"+|((?:^|[^\\\\])(?:\\\\.)*)"+P+"+$","g"),X=RegExp("^"+P+"*,"+P+"*"),U=RegExp("^"+P+"*([>+~]|"+P+")"+P+"*"),V=RegExp(P+"*[+~]"),Y=RegExp("="+P+"*([^\\]'\"]*)"+P+"*\\]","g"),J=RegExp(I),G=RegExp("^"+W+"$"),Q={ID:RegExp("^#("+R+")"),CLASS:RegExp("^\\.("+R+")"),TAG:RegExp("^("+R.replace("w","w*")+")"),ATTR:RegExp("^"+$),PSEUDO:RegExp("^"+I),CHILD:RegExp("^:(only|first|last|nth|nth-last)-(child|of-type)(?:\\("+P+"*(even|odd|(([+-]|)(\\d*)n|)"+P+"*(?:([+-]|)"+P+"*(\\d+)|))"+P+"*\\)|)","i"),bool:RegExp("^(?:"+B+")$","i"),needsContext:RegExp("^"+P+"*[>+~]|:(even|odd|eq|gt|lt|nth|first|last)(?:\\("+P+"*((?:-\\d)?\\d*)"+P+"*\\)|)(?=[^-]|$)","i")},K=/^[^{]+\{\s*\[native \w/,Z=/^(?:#([\w-]+)|(\w+)|\.([\w-]+))$/,et=/^(?:input|select|textarea|button)$/i,tt=/^h\d$/i,nt=/'|\\/g,rt=RegExp("\\\\([\\da-f]{1,6}"+P+"?|("+P+")|.)","ig"),it=function(e,t,n){var r="0x"+t-65536;return r!==r||n?t:0>r?String.fromCharCode(r+65536):String.fromCharCode(55296|r>>10,56320|1023&r)};try{M.apply(H=O.call(w.childNodes),w.childNodes),H[w.childNodes.length].nodeType}catch(ot){M={apply:H.length?function(e,t){_.apply(e,O.call(t))}:function(e,t){var n=e.length,r=0;while(e[n++]=t[r++]);e.length=n-1}}}function at(e,t,n,i){var o,a,s,l,u,c,d,m,y,x;if((t?t.ownerDocument||t:w)!==f&&p(t),t=t||f,n=n||[],!e||"string"!=typeof e)return n;if(1!==(l=t.nodeType)&&9!==l)return[];if(h&&!i){if(o=Z.exec(e))if(s=o[1]){if(9===l){if(a=t.getElementById(s),!a||!a.parentNode)return n;if(a.id===s)return n.push(a),n}else if(t.ownerDocument&&(a=t.ownerDocument.getElementById(s))&&v(t,a)&&a.id===s)return n.push(a),n}else{if(o[2])return M.apply(n,t.getElementsByTagName(e)),n;if((s=o[3])&&r.getElementsByClassName&&t.getElementsByClassName)return M.apply(n,t.getElementsByClassName(s)),n}if(r.qsa&&(!g||!g.test(e))){if(m=d=b,y=t,x=9===l&&e,1===l&&"object"!==t.nodeName.toLowerCase()){c=bt(e),(d=t.getAttribute("id"))?m=d.replace(nt,"\\$&"):t.setAttribute("id",m),m="[id='"+m+"'] ",u=c.length;while(u--)c[u]=m+xt(c[u]);y=V.test(e)&&t.parentNode||t,x=c.join(",")}if(x)try{return M.apply(n,y.querySelectorAll(x)),n}catch(T){}finally{d||t.removeAttribute("id")}}}return At(e.replace(z,"$1"),t,n,i)}function st(e){return K.test(e+"")}function lt(){var e=[];function t(n,r){return e.push(n+=" ")>o.cacheLength&&delete t[e.shift()],t[n]=r}return t}function ut(e){return e[b]=!0,e}function ct(e){var t=f.createElement("div");try{return!!e(t)}catch(n){return!1}finally{t.parentNode&&t.parentNode.removeChild(t),t=null}}function pt(e,t,n){e=e.split("|");var r,i=e.length,a=n?null:t;while(i--)(r=o.attrHandle[e[i]])&&r!==t||(o.attrHandle[e[i]]=a)}function ft(e,t){var n=e.getAttributeNode(t);return n&&n.specified?n.value:e[t]===!0?t.toLowerCase():null}function dt(e,t){return e.getAttribute(t,"type"===t.toLowerCase()?1:2)}function ht(e){return"input"===e.nodeName.toLowerCase()?e.defaultValue:t}function gt(e,t){var n=t&&e,r=n&&1===e.nodeType&&1===t.nodeType&&(~t.sourceIndex||D)-(~e.sourceIndex||D);if(r)return r;if(n)while(n=n.nextSibling)if(n===t)return-1;return e?1:-1}function mt(e){return function(t){var n=t.nodeName.toLowerCase();return"input"===n&&t.type===e}}function yt(e){return function(t){var n=t.nodeName.toLowerCase();return("input"===n||"button"===n)&&t.type===e}}function vt(e){return ut(function(t){return t=+t,ut(function(n,r){var i,o=e([],n.length,t),a=o.length;while(a--)n[i=o[a]]&&(n[i]=!(r[i]=n[i]))})})}s=at.isXML=function(e){var t=e&&(e.ownerDocument||e).documentElement;return t?"HTML"!==t.nodeName:!1},r=at.support={},p=at.setDocument=function(e){var n=e?e.ownerDocument||e:w,i=n.parentWindow;return n!==f&&9===n.nodeType&&n.documentElement?(f=n,d=n.documentElement,h=!s(n),i&&i.frameElement&&i.attachEvent("onbeforeunload",function(){p()}),r.attributes=ct(function(e){return e.innerHTML="<a href='#'></a>",pt("type|href|height|width",dt,"#"===e.firstChild.getAttribute("href")),pt(B,ft,null==e.getAttribute("disabled")),e.className="i",!e.getAttribute("className")}),r.input=ct(function(e){return e.innerHTML="<input>",e.firstChild.setAttribute("value",""),""===e.firstChild.getAttribute("value")}),pt("value",ht,r.attributes&&r.input),r.getElementsByTagName=ct(function(e){return e.appendChild(n.createComment("")),!e.getElementsByTagName("*").length}),r.getElementsByClassName=ct(function(e){return e.innerHTML="<div class='a'></div><div class='a i'></div>",e.firstChild.className="i",2===e.getElementsByClassName("i").length}),r.getById=ct(function(e){return d.appendChild(e).id=b,!n.getElementsByName||!n.getElementsByName(b).length}),r.getById?(o.find.ID=function(e,t){if(typeof t.getElementById!==j&&h){var n=t.getElementById(e);return n&&n.parentNode?[n]:[]}},o.filter.ID=function(e){var t=e.replace(rt,it);return function(e){return e.getAttribute("id")===t}}):(delete o.find.ID,o.filter.ID=function(e){var t=e.replace(rt,it);return function(e){var n=typeof e.getAttributeNode!==j&&e.getAttributeNode("id");return n&&n.value===t}}),o.find.TAG=r.getElementsByTagName?function(e,n){return typeof n.getElementsByTagName!==j?n.getElementsByTagName(e):t}:function(e,t){var n,r=[],i=0,o=t.getElementsByTagName(e);if("*"===e){while(n=o[i++])1===n.nodeType&&r.push(n);return r}return o},o.find.CLASS=r.getElementsByClassName&&function(e,n){return typeof n.getElementsByClassName!==j&&h?n.getElementsByClassName(e):t},m=[],g=[],(r.qsa=st(n.querySelectorAll))&&(ct(function(e){e.innerHTML="<select><option selected=''></option></select>",e.querySelectorAll("[selected]").length||g.push("\\["+P+"*(?:value|"+B+")"),e.querySelectorAll(":checked").length||g.push(":checked")}),ct(function(e){var t=n.createElement("input");t.setAttribute("type","hidden"),e.appendChild(t).setAttribute("t",""),e.querySelectorAll("[t^='']").length&&g.push("[*^$]="+P+"*(?:''|\"\")"),e.querySelectorAll(":enabled").length||g.push(":enabled",":disabled"),e.querySelectorAll("*,:x"),g.push(",.*:")})),(r.matchesSelector=st(y=d.webkitMatchesSelector||d.mozMatchesSelector||d.oMatchesSelector||d.msMatchesSelector))&&ct(function(e){r.disconnectedMatch=y.call(e,"div"),y.call(e,"[s!='']:x"),m.push("!=",I)}),g=g.length&&RegExp(g.join("|")),m=m.length&&RegExp(m.join("|")),v=st(d.contains)||d.compareDocumentPosition?function(e,t){var n=9===e.nodeType?e.documentElement:e,r=t&&t.parentNode;return e===r||!(!r||1!==r.nodeType||!(n.contains?n.contains(r):e.compareDocumentPosition&&16&e.compareDocumentPosition(r)))}:function(e,t){if(t)while(t=t.parentNode)if(t===e)return!0;return!1},r.sortDetached=ct(function(e){return 1&e.compareDocumentPosition(n.createElement("div"))}),A=d.compareDocumentPosition?function(e,t){if(e===t)return S=!0,0;var i=t.compareDocumentPosition&&e.compareDocumentPosition&&e.compareDocumentPosition(t);return i?1&i||!r.sortDetached&&t.compareDocumentPosition(e)===i?e===n||v(w,e)?-1:t===n||v(w,t)?1:c?F.call(c,e)-F.call(c,t):0:4&i?-1:1:e.compareDocumentPosition?-1:1}:function(e,t){var r,i=0,o=e.parentNode,a=t.parentNode,s=[e],l=[t];if(e===t)return S=!0,0;if(!o||!a)return e===n?-1:t===n?1:o?-1:a?1:c?F.call(c,e)-F.call(c,t):0;if(o===a)return gt(e,t);r=e;while(r=r.parentNode)s.unshift(r);r=t;while(r=r.parentNode)l.unshift(r);while(s[i]===l[i])i++;return i?gt(s[i],l[i]):s[i]===w?-1:l[i]===w?1:0},n):f},at.matches=function(e,t){return at(e,null,null,t)},at.matchesSelector=function(e,t){if((e.ownerDocument||e)!==f&&p(e),t=t.replace(Y,"='$1']"),!(!r.matchesSelector||!h||m&&m.test(t)||g&&g.test(t)))try{var n=y.call(e,t);if(n||r.disconnectedMatch||e.document&&11!==e.document.nodeType)return n}catch(i){}return at(t,f,null,[e]).length>0},at.contains=function(e,t){return(e.ownerDocument||e)!==f&&p(e),v(e,t)},at.attr=function(e,n){(e.ownerDocument||e)!==f&&p(e);var i=o.attrHandle[n.toLowerCase()],a=i&&L.call(o.attrHandle,n.toLowerCase())?i(e,n,!h):t;return a===t?r.attributes||!h?e.getAttribute(n):(a=e.getAttributeNode(n))&&a.specified?a.value:null:a},at.error=function(e){throw Error("Syntax error, unrecognized expression: "+e)},at.uniqueSort=function(e){var t,n=[],i=0,o=0;if(S=!r.detectDuplicates,c=!r.sortStable&&e.slice(0),e.sort(A),S){while(t=e[o++])t===e[o]&&(i=n.push(o));while(i--)e.splice(n[i],1)}return e},a=at.getText=function(e){var t,n="",r=0,i=e.nodeType;if(i){if(1===i||9===i||11===i){if("string"==typeof e.textContent)return e.textContent;for(e=e.firstChild;e;e=e.nextSibling)n+=a(e)}else if(3===i||4===i)return e.nodeValue}else for(;t=e[r];r++)n+=a(t);return n},o=at.selectors={cacheLength:50,createPseudo:ut,match:Q,attrHandle:{},find:{},relative:{">":{dir:"parentNode",first:!0}," ":{dir:"parentNode"},"+":{dir:"previousSibling",first:!0},"~":{dir:"previousSibling"}},preFilter:{ATTR:function(e){return e[1]=e[1].replace(rt,it),e[3]=(e[4]||e[5]||"").replace(rt,it),"~="===e[2]&&(e[3]=" "+e[3]+" "),e.slice(0,4)},CHILD:function(e){return e[1]=e[1].toLowerCase(),"nth"===e[1].slice(0,3)?(e[3]||at.error(e[0]),e[4]=+(e[4]?e[5]+(e[6]||1):2*("even"===e[3]||"odd"===e[3])),e[5]=+(e[7]+e[8]||"odd"===e[3])):e[3]&&at.error(e[0]),e},PSEUDO:function(e){var n,r=!e[5]&&e[2];return Q.CHILD.test(e[0])?null:(e[3]&&e[4]!==t?e[2]=e[4]:r&&J.test(r)&&(n=bt(r,!0))&&(n=r.indexOf(")",r.length-n)-r.length)&&(e[0]=e[0].slice(0,n),e[2]=r.slice(0,n)),e.slice(0,3))}},filter:{TAG:function(e){var t=e.replace(rt,it).toLowerCase();return"*"===e?function(){return!0}:function(e){return e.nodeName&&e.nodeName.toLowerCase()===t}},CLASS:function(e){var t=N[e+" "];return t||(t=RegExp("(^|"+P+")"+e+"("+P+"|$)"))&&N(e,function(e){return t.test("string"==typeof e.className&&e.className||typeof e.getAttribute!==j&&e.getAttribute("class")||"")})},ATTR:function(e,t,n){return function(r){var i=at.attr(r,e);return null==i?"!="===t:t?(i+="","="===t?i===n:"!="===t?i!==n:"^="===t?n&&0===i.indexOf(n):"*="===t?n&&i.indexOf(n)>-1:"$="===t?n&&i.slice(-n.length)===n:"~="===t?(" "+i+" ").indexOf(n)>-1:"|="===t?i===n||i.slice(0,n.length+1)===n+"-":!1):!0}},CHILD:function(e,t,n,r,i){var o="nth"!==e.slice(0,3),a="last"!==e.slice(-4),s="of-type"===t;return 1===r&&0===i?function(e){return!!e.parentNode}:function(t,n,l){var u,c,p,f,d,h,g=o!==a?"nextSibling":"previousSibling",m=t.parentNode,y=s&&t.nodeName.toLowerCase(),v=!l&&!s;if(m){if(o){while(g){p=t;while(p=p[g])if(s?p.nodeName.toLowerCase()===y:1===p.nodeType)return!1;h=g="only"===e&&!h&&"nextSibling"}return!0}if(h=[a?m.firstChild:m.lastChild],a&&v){c=m[b]||(m[b]={}),u=c[e]||[],d=u[0]===T&&u[1],f=u[0]===T&&u[2],p=d&&m.childNodes[d];while(p=++d&&p&&p[g]||(f=d=0)||h.pop())if(1===p.nodeType&&++f&&p===t){c[e]=[T,d,f];break}}else if(v&&(u=(t[b]||(t[b]={}))[e])&&u[0]===T)f=u[1];else while(p=++d&&p&&p[g]||(f=d=0)||h.pop())if((s?p.nodeName.toLowerCase()===y:1===p.nodeType)&&++f&&(v&&((p[b]||(p[b]={}))[e]=[T,f]),p===t))break;return f-=i,f===r||0===f%r&&f/r>=0}}},PSEUDO:function(e,t){var n,r=o.pseudos[e]||o.setFilters[e.toLowerCase()]||at.error("unsupported pseudo: "+e);return r[b]?r(t):r.length>1?(n=[e,e,"",t],o.setFilters.hasOwnProperty(e.toLowerCase())?ut(function(e,n){var i,o=r(e,t),a=o.length;while(a--)i=F.call(e,o[a]),e[i]=!(n[i]=o[a])}):function(e){return r(e,0,n)}):r}},pseudos:{not:ut(function(e){var t=[],n=[],r=l(e.replace(z,"$1"));return r[b]?ut(function(e,t,n,i){var o,a=r(e,null,i,[]),s=e.length;while(s--)(o=a[s])&&(e[s]=!(t[s]=o))}):function(e,i,o){return t[0]=e,r(t,null,o,n),!n.pop()}}),has:ut(function(e){return function(t){return at(e,t).length>0}}),contains:ut(function(e){return function(t){return(t.textContent||t.innerText||a(t)).indexOf(e)>-1}}),lang:ut(function(e){return G.test(e||"")||at.error("unsupported lang: "+e),e=e.replace(rt,it).toLowerCase(),function(t){var n;do if(n=h?t.lang:t.getAttribute("xml:lang")||t.getAttribute("lang"))return n=n.toLowerCase(),n===e||0===n.indexOf(e+"-");while((t=t.parentNode)&&1===t.nodeType);return!1}}),target:function(t){var n=e.location&&e.location.hash;return n&&n.slice(1)===t.id},root:function(e){return e===d},focus:function(e){return e===f.activeElement&&(!f.hasFocus||f.hasFocus())&&!!(e.type||e.href||~e.tabIndex)},enabled:function(e){return e.disabled===!1},disabled:function(e){return e.disabled===!0},checked:function(e){var t=e.nodeName.toLowerCase();return"input"===t&&!!e.checked||"option"===t&&!!e.selected},selected:function(e){return e.parentNode&&e.parentNode.selectedIndex,e.selected===!0},empty:function(e){for(e=e.firstChild;e;e=e.nextSibling)if(e.nodeName>"@"||3===e.nodeType||4===e.nodeType)return!1;return!0},parent:function(e){return!o.pseudos.empty(e)},header:function(e){return tt.test(e.nodeName)},input:function(e){return et.test(e.nodeName)},button:function(e){var t=e.nodeName.toLowerCase();return"input"===t&&"button"===e.type||"button"===t},text:function(e){var t;return"input"===e.nodeName.toLowerCase()&&"text"===e.type&&(null==(t=e.getAttribute("type"))||t.toLowerCase()===e.type)},first:vt(function(){return[0]}),last:vt(function(e,t){return[t-1]}),eq:vt(function(e,t,n){return[0>n?n+t:n]}),even:vt(function(e,t){var n=0;for(;t>n;n+=2)e.push(n);return e}),odd:vt(function(e,t){var n=1;for(;t>n;n+=2)e.push(n);return e}),lt:vt(function(e,t,n){var r=0>n?n+t:n;for(;--r>=0;)e.push(r);return e}),gt:vt(function(e,t,n){var r=0>n?n+t:n;for(;t>++r;)e.push(r);return e})}};for(n in{radio:!0,checkbox:!0,file:!0,password:!0,image:!0})o.pseudos[n]=mt(n);for(n in{submit:!0,reset:!0})o.pseudos[n]=yt(n);function bt(e,t){var n,r,i,a,s,l,u,c=k[e+" "];if(c)return t?0:c.slice(0);s=e,l=[],u=o.preFilter;while(s){(!n||(r=X.exec(s)))&&(r&&(s=s.slice(r[0].length)||s),l.push(i=[])),n=!1,(r=U.exec(s))&&(n=r.shift(),i.push({value:n,type:r[0].replace(z," ")}),s=s.slice(n.length));for(a in o.filter)!(r=Q[a].exec(s))||u[a]&&!(r=u[a](r))||(n=r.shift(),i.push({value:n,type:a,matches:r}),s=s.slice(n.length));if(!n)break}return t?s.length:s?at.error(e):k(e,l).slice(0)}function xt(e){var t=0,n=e.length,r="";for(;n>t;t++)r+=e[t].value;return r}function wt(e,t,n){var r=t.dir,o=n&&"parentNode"===r,a=C++;return t.first?function(t,n,i){while(t=t[r])if(1===t.nodeType||o)return e(t,n,i)}:function(t,n,s){var l,u,c,p=T+" "+a;if(s){while(t=t[r])if((1===t.nodeType||o)&&e(t,n,s))return!0}else while(t=t[r])if(1===t.nodeType||o)if(c=t[b]||(t[b]={}),(u=c[r])&&u[0]===p){if((l=u[1])===!0||l===i)return l===!0}else if(u=c[r]=[p],u[1]=e(t,n,s)||i,u[1]===!0)return!0}}function Tt(e){return e.length>1?function(t,n,r){var i=e.length;while(i--)if(!e[i](t,n,r))return!1;return!0}:e[0]}function Ct(e,t,n,r,i){var o,a=[],s=0,l=e.length,u=null!=t;for(;l>s;s++)(o=e[s])&&(!n||n(o,r,i))&&(a.push(o),u&&t.push(s));return a}function Nt(e,t,n,r,i,o){return r&&!r[b]&&(r=Nt(r)),i&&!i[b]&&(i=Nt(i,o)),ut(function(o,a,s,l){var u,c,p,f=[],d=[],h=a.length,g=o||St(t||"*",s.nodeType?[s]:s,[]),m=!e||!o&&t?g:Ct(g,f,e,s,l),y=n?i||(o?e:h||r)?[]:a:m;if(n&&n(m,y,s,l),r){u=Ct(y,d),r(u,[],s,l),c=u.length;while(c--)(p=u[c])&&(y[d[c]]=!(m[d[c]]=p))}if(o){if(i||e){if(i){u=[],c=y.length;while(c--)(p=y[c])&&u.push(m[c]=p);i(null,y=[],u,l)}c=y.length;while(c--)(p=y[c])&&(u=i?F.call(o,p):f[c])>-1&&(o[u]=!(a[u]=p))}}else y=Ct(y===a?y.splice(h,y.length):y),i?i(null,a,y,l):M.apply(a,y)})}function kt(e){var t,n,r,i=e.length,a=o.relative[e[0].type],s=a||o.relative[" "],l=a?1:0,c=wt(function(e){return e===t},s,!0),p=wt(function(e){return F.call(t,e)>-1},s,!0),f=[function(e,n,r){return!a&&(r||n!==u)||((t=n).nodeType?c(e,n,r):p(e,n,r))}];for(;i>l;l++)if(n=o.relative[e[l].type])f=[wt(Tt(f),n)];else{if(n=o.filter[e[l].type].apply(null,e[l].matches),n[b]){for(r=++l;i>r;r++)if(o.relative[e[r].type])break;return Nt(l>1&&Tt(f),l>1&&xt(e.slice(0,l-1).concat({value:" "===e[l-2].type?"*":""})).replace(z,"$1"),n,r>l&&kt(e.slice(l,r)),i>r&&kt(e=e.slice(r)),i>r&&xt(e))}f.push(n)}return Tt(f)}function Et(e,t){var n=0,r=t.length>0,a=e.length>0,s=function(s,l,c,p,d){var h,g,m,y=[],v=0,b="0",x=s&&[],w=null!=d,C=u,N=s||a&&o.find.TAG("*",d&&l.parentNode||l),k=T+=null==C?1:Math.random()||.1;for(w&&(u=l!==f&&l,i=n);null!=(h=N[b]);b++){if(a&&h){g=0;while(m=e[g++])if(m(h,l,c)){p.push(h);break}w&&(T=k,i=++n)}r&&((h=!m&&h)&&v--,s&&x.push(h))}if(v+=b,r&&b!==v){g=0;while(m=t[g++])m(x,y,l,c);if(s){if(v>0)while(b--)x[b]||y[b]||(y[b]=q.call(p));y=Ct(y)}M.apply(p,y),w&&!s&&y.length>0&&v+t.length>1&&at.uniqueSort(p)}return w&&(T=k,u=C),x};return r?ut(s):s}l=at.compile=function(e,t){var n,r=[],i=[],o=E[e+" "];if(!o){t||(t=bt(e)),n=t.length;while(n--)o=kt(t[n]),o[b]?r.push(o):i.push(o);o=E(e,Et(i,r))}return o};function St(e,t,n){var r=0,i=t.length;for(;i>r;r++)at(e,t[r],n);return n}function At(e,t,n,i){var a,s,u,c,p,f=bt(e);if(!i&&1===f.length){if(s=f[0]=f[0].slice(0),s.length>2&&"ID"===(u=s[0]).type&&r.getById&&9===t.nodeType&&h&&o.relative[s[1].type]){if(t=(o.find.ID(u.matches[0].replace(rt,it),t)||[])[0],!t)return n;e=e.slice(s.shift().value.length)}a=Q.needsContext.test(e)?0:s.length;while(a--){if(u=s[a],o.relative[c=u.type])break;if((p=o.find[c])&&(i=p(u.matches[0].replace(rt,it),V.test(s[0].type)&&t.parentNode||t))){if(s.splice(a,1),e=i.length&&xt(s),!e)return M.apply(n,i),n;break}}}return l(e,f)(i,t,!h,n,V.test(e)),n}o.pseudos.nth=o.pseudos.eq;function jt(){}jt.prototype=o.filters=o.pseudos,o.setFilters=new jt,r.sortStable=b.split("").sort(A).join("")===b,p(),[0,0].sort(A),r.detectDuplicates=S,x.find=at,x.expr=at.selectors,x.expr[":"]=x.expr.pseudos,x.unique=at.uniqueSort,x.text=at.getText,x.isXMLDoc=at.isXML,x.contains=at.contains}(e);var O={};function F(e){var t=O[e]={};return x.each(e.match(T)||[],function(e,n){t[n]=!0}),t}x.Callbacks=function(e){e="string"==typeof e?O[e]||F(e):x.extend({},e);var n,r,i,o,a,s,l=[],u=!e.once&&[],c=function(t){for(r=e.memory&&t,i=!0,a=s||0,s=0,o=l.length,n=!0;l&&o>a;a++)if(l[a].apply(t[0],t[1])===!1&&e.stopOnFalse){r=!1;break}n=!1,l&&(u?u.length&&c(u.shift()):r?l=[]:p.disable())},p={add:function(){if(l){var t=l.length;(function i(t){x.each(t,function(t,n){var r=x.type(n);"function"===r?e.unique&&p.has(n)||l.push(n):n&&n.length&&"string"!==r&&i(n)})})(arguments),n?o=l.length:r&&(s=t,c(r))}return this},remove:function(){return l&&x.each(arguments,function(e,t){var r;while((r=x.inArray(t,l,r))>-1)l.splice(r,1),n&&(o>=r&&o--,a>=r&&a--)}),this},has:function(e){return e?x.inArray(e,l)>-1:!(!l||!l.length)},empty:function(){return l=[],o=0,this},disable:function(){return l=u=r=t,this},disabled:function(){return!l},lock:function(){return u=t,r||p.disable(),this},locked:function(){return!u},fireWith:function(e,t){return t=t||[],t=[e,t.slice?t.slice():t],!l||i&&!u||(n?u.push(t):c(t)),this},fire:function(){return p.fireWith(this,arguments),this},fired:function(){return!!i}};return p},x.extend({Deferred:function(e){var t=[["resolve","done",x.Callbacks("once memory"),"resolved"],["reject","fail",x.Callbacks("once memory"),"rejected"],["notify","progress",x.Callbacks("memory")]],n="pending",r={state:function(){return n},always:function(){return i.done(arguments).fail(arguments),this},then:function(){var e=arguments;return x.Deferred(function(n){x.each(t,function(t,o){var a=o[0],s=x.isFunction(e[t])&&e[t];i[o[1]](function(){var e=s&&s.apply(this,arguments);e&&x.isFunction(e.promise)?e.promise().done(n.resolve).fail(n.reject).progress(n.notify):n[a+"With"](this===r?n.promise():this,s?[e]:arguments)})}),e=null}).promise()},promise:function(e){return null!=e?x.extend(e,r):r}},i={};return r.pipe=r.then,x.each(t,function(e,o){var a=o[2],s=o[3];r[o[1]]=a.add,s&&a.add(function(){n=s},t[1^e][2].disable,t[2][2].lock),i[o[0]]=function(){return i[o[0]+"With"](this===i?r:this,arguments),this},i[o[0]+"With"]=a.fireWith}),r.promise(i),e&&e.call(i,i),i},when:function(e){var t=0,n=g.call(arguments),r=n.length,i=1!==r||e&&x.isFunction(e.promise)?r:0,o=1===i?e:x.Deferred(),a=function(e,t,n){return function(r){t[e]=this,n[e]=arguments.length>1?g.call(arguments):r,n===s?o.notifyWith(t,n):--i||o.resolveWith(t,n)}},s,l,u;if(r>1)for(s=Array(r),l=Array(r),u=Array(r);r>t;t++)n[t]&&x.isFunction(n[t].promise)?n[t].promise().done(a(t,u,n)).fail(o.reject).progress(a(t,l,s)):--i;return i||o.resolveWith(u,n),o.promise()}}),x.support=function(t){var n,r,o,s,l,u,c,p,f,d=a.createElement("div");if(d.setAttribute("className","t"),d.innerHTML="  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>",n=d.getElementsByTagName("*")||[],r=d.getElementsByTagName("a")[0],!r||!r.style||!n.length)return t;s=a.createElement("select"),u=s.appendChild(a.createElement("option")),o=d.getElementsByTagName("input")[0],r.style.cssText="top:1px;float:left;opacity:.5",t.getSetAttribute="t"!==d.className,t.leadingWhitespace=3===d.firstChild.nodeType,t.tbody=!d.getElementsByTagName("tbody").length,t.htmlSerialize=!!d.getElementsByTagName("link").length,t.style=/top/.test(r.getAttribute("style")),t.hrefNormalized="/a"===r.getAttribute("href"),t.opacity=/^0.5/.test(r.style.opacity),t.cssFloat=!!r.style.cssFloat,t.checkOn=!!o.value,t.optSelected=u.selected,t.enctype=!!a.createElement("form").enctype,t.html5Clone="<:nav></:nav>"!==a.createElement("nav").cloneNode(!0).outerHTML,t.inlineBlockNeedsLayout=!1,t.shrinkWrapBlocks=!1,t.pixelPosition=!1,t.deleteExpando=!0,t.noCloneEvent=!0,t.reliableMarginRight=!0,t.boxSizingReliable=!0,o.checked=!0,t.noCloneChecked=o.cloneNode(!0).checked,s.disabled=!0,t.optDisabled=!u.disabled;try{delete d.test}catch(h){t.deleteExpando=!1}o=a.createElement("input"),o.setAttribute("value",""),t.input=""===o.getAttribute("value"),o.value="t",o.setAttribute("type","radio"),t.radioValue="t"===o.value,o.setAttribute("checked","t"),o.setAttribute("name","t"),l=a.createDocumentFragment(),l.appendChild(o),t.appendChecked=o.checked,t.checkClone=l.cloneNode(!0).cloneNode(!0).lastChild.checked,d.attachEvent&&(d.attachEvent("onclick",function(){t.noCloneEvent=!1}),d.cloneNode(!0).click());for(f in{submit:!0,change:!0,focusin:!0})d.setAttribute(c="on"+f,"t"),t[f+"Bubbles"]=c in e||d.attributes[c].expando===!1;d.style.backgroundClip="content-box",d.cloneNode(!0).style.backgroundClip="",t.clearCloneStyle="content-box"===d.style.backgroundClip;for(f in x(t))break;return t.ownLast="0"!==f,x(function(){var n,r,o,s="padding:0;margin:0;border:0;display:block;box-sizing:content-box;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;",l=a.getElementsByTagName("body")[0];l&&(n=a.createElement("div"),n.style.cssText="border:0;width:0;height:0;position:absolute;top:0;left:-9999px;margin-top:1px",l.appendChild(n).appendChild(d),d.innerHTML="<table><tr><td></td><td>t</td></tr></table>",o=d.getElementsByTagName("td"),o[0].style.cssText="padding:0;margin:0;border:0;display:none",p=0===o[0].offsetHeight,o[0].style.display="",o[1].style.display="none",t.reliableHiddenOffsets=p&&0===o[0].offsetHeight,d.innerHTML="",d.style.cssText="box-sizing:border-box;-moz-box-sizing:border-box;-webkit-box-sizing:border-box;padding:1px;border:1px;display:block;width:4px;margin-top:1%;position:absolute;top:1%;",x.swap(l,null!=l.style.zoom?{zoom:1}:{},function(){t.boxSizing=4===d.offsetWidth}),e.getComputedStyle&&(t.pixelPosition="1%"!==(e.getComputedStyle(d,null)||{}).top,t.boxSizingReliable="4px"===(e.getComputedStyle(d,null)||{width:"4px"}).width,r=d.appendChild(a.createElement("div")),r.style.cssText=d.style.cssText=s,r.style.marginRight=r.style.width="0",d.style.width="1px",t.reliableMarginRight=!parseFloat((e.getComputedStyle(r,null)||{}).marginRight)),typeof d.style.zoom!==i&&(d.innerHTML="",d.style.cssText=s+"width:1px;padding:1px;display:inline;zoom:1",t.inlineBlockNeedsLayout=3===d.offsetWidth,d.style.display="block",d.innerHTML="<div></div>",d.firstChild.style.width="5px",t.shrinkWrapBlocks=3!==d.offsetWidth,t.inlineBlockNeedsLayout&&(l.style.zoom=1)),l.removeChild(n),n=d=o=r=null)}),n=s=l=u=r=o=null,t}({});var B=/(?:\{[\s\S]*\}|\[[\s\S]*\])$/,P=/([A-Z])/g;function R(e,n,r,i){if(x.acceptData(e)){var o,a,s=x.expando,l=e.nodeType,u=l?x.cache:e,c=l?e[s]:e[s]&&s;if(c&&u[c]&&(i||u[c].data)||r!==t||"string"!=typeof n)return c||(c=l?e[s]=p.pop()||x.guid++:s),u[c]||(u[c]=l?{}:{toJSON:x.noop}),("object"==typeof n||"function"==typeof n)&&(i?u[c]=x.extend(u[c],n):u[c].data=x.extend(u[c].data,n)),a=u[c],i||(a.data||(a.data={}),a=a.data),r!==t&&(a[x.camelCase(n)]=r),"string"==typeof n?(o=a[n],null==o&&(o=a[x.camelCase(n)])):o=a,o}}function W(e,t,n){if(x.acceptData(e)){var r,i,o=e.nodeType,a=o?x.cache:e,s=o?e[x.expando]:x.expando;if(a[s]){if(t&&(r=n?a[s]:a[s].data)){x.isArray(t)?t=t.concat(x.map(t,x.camelCase)):t in r?t=[t]:(t=x.camelCase(t),t=t in r?[t]:t.split(" ")),i=t.length;while(i--)delete r[t[i]];if(n?!I(r):!x.isEmptyObject(r))return}(n||(delete a[s].data,I(a[s])))&&(o?x.cleanData([e],!0):x.support.deleteExpando||a!=a.window?delete a[s]:a[s]=null)}}}x.extend({cache:{},noData:{applet:!0,embed:!0,object:"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"},hasData:function(e){return e=e.nodeType?x.cache[e[x.expando]]:e[x.expando],!!e&&!I(e)},data:function(e,t,n){return R(e,t,n)},removeData:function(e,t){return W(e,t)},_data:function(e,t,n){return R(e,t,n,!0)},_removeData:function(e,t){return W(e,t,!0)},acceptData:function(e){if(e.nodeType&&1!==e.nodeType&&9!==e.nodeType)return!1;var t=e.nodeName&&x.noData[e.nodeName.toLowerCase()];return!t||t!==!0&&e.getAttribute("classid")===t}}),x.fn.extend({data:function(e,n){var r,i,o=null,a=0,s=this[0];if(e===t){if(this.length&&(o=x.data(s),1===s.nodeType&&!x._data(s,"parsedAttrs"))){for(r=s.attributes;r.length>a;a++)i=r[a].name,0===i.indexOf("data-")&&(i=x.camelCase(i.slice(5)),$(s,i,o[i]));x._data(s,"parsedAttrs",!0)}return o}return"object"==typeof e?this.each(function(){x.data(this,e)}):arguments.length>1?this.each(function(){x.data(this,e,n)}):s?$(s,e,x.data(s,e)):null},removeData:function(e){return this.each(function(){x.removeData(this,e)})}});function $(e,n,r){if(r===t&&1===e.nodeType){var i="data-"+n.replace(P,"-$1").toLowerCase();if(r=e.getAttribute(i),"string"==typeof r){try{r="true"===r?!0:"false"===r?!1:"null"===r?null:+r+""===r?+r:B.test(r)?x.parseJSON(r):r}catch(o){}x.data(e,n,r)}else r=t}return r}function I(e){var t;for(t in e)if(("data"!==t||!x.isEmptyObject(e[t]))&&"toJSON"!==t)return!1;return!0}x.extend({queue:function(e,n,r){var i;return e?(n=(n||"fx")+"queue",i=x._data(e,n),r&&(!i||x.isArray(r)?i=x._data(e,n,x.makeArray(r)):i.push(r)),i||[]):t},dequeue:function(e,t){t=t||"fx";var n=x.queue(e,t),r=n.length,i=n.shift(),o=x._queueHooks(e,t),a=function(){x.dequeue(e,t)};"inprogress"===i&&(i=n.shift(),r--),i&&("fx"===t&&n.unshift("inprogress"),delete o.stop,i.call(e,a,o)),!r&&o&&o.empty.fire()},_queueHooks:function(e,t){var n=t+"queueHooks";return x._data(e,n)||x._data(e,n,{empty:x.Callbacks("once memory").add(function(){x._removeData(e,t+"queue"),x._removeData(e,n)})})}}),x.fn.extend({queue:function(e,n){var r=2;return"string"!=typeof e&&(n=e,e="fx",r--),r>arguments.length?x.queue(this[0],e):n===t?this:this.each(function(){var t=x.queue(this,e,n);x._queueHooks(this,e),"fx"===e&&"inprogress"!==t[0]&&x.dequeue(this,e)})},dequeue:function(e){return this.each(function(){x.dequeue(this,e)})},delay:function(e,t){return e=x.fx?x.fx.speeds[e]||e:e,t=t||"fx",this.queue(t,function(t,n){var r=setTimeout(t,e);n.stop=function(){clearTimeout(r)}})},clearQueue:function(e){return this.queue(e||"fx",[])},promise:function(e,n){var r,i=1,o=x.Deferred(),a=this,s=this.length,l=function(){--i||o.resolveWith(a,[a])};"string"!=typeof e&&(n=e,e=t),e=e||"fx";while(s--)r=x._data(a[s],e+"queueHooks"),r&&r.empty&&(i++,r.empty.add(l));return l(),o.promise(n)}});var z,X,U=/[\t\r\n\f]/g,V=/\r/g,Y=/^(?:input|select|textarea|button|object)$/i,J=/^(?:a|area)$/i,G=/^(?:checked|selected)$/i,Q=x.support.getSetAttribute,K=x.support.input;x.fn.extend({attr:function(e,t){return x.access(this,x.attr,e,t,arguments.length>1)},removeAttr:function(e){return this.each(function(){x.removeAttr(this,e)})},prop:function(e,t){return x.access(this,x.prop,e,t,arguments.length>1)},removeProp:function(e){return e=x.propFix[e]||e,this.each(function(){try{this[e]=t,delete this[e]}catch(n){}})},addClass:function(e){var t,n,r,i,o,a=0,s=this.length,l="string"==typeof e&&e;if(x.isFunction(e))return this.each(function(t){x(this).addClass(e.call(this,t,this.className))});if(l)for(t=(e||"").match(T)||[];s>a;a++)if(n=this[a],r=1===n.nodeType&&(n.className?(" "+n.className+" ").replace(U," "):" ")){o=0;while(i=t[o++])0>r.indexOf(" "+i+" ")&&(r+=i+" ");n.className=x.trim(r)}return this},removeClass:function(e){var t,n,r,i,o,a=0,s=this.length,l=0===arguments.length||"string"==typeof e&&e;if(x.isFunction(e))return this.each(function(t){x(this).removeClass(e.call(this,t,this.className))});if(l)for(t=(e||"").match(T)||[];s>a;a++)if(n=this[a],r=1===n.nodeType&&(n.className?(" "+n.className+" ").replace(U," "):"")){o=0;while(i=t[o++])while(r.indexOf(" "+i+" ")>=0)r=r.replace(" "+i+" "," ");n.className=e?x.trim(r):""}return this},toggleClass:function(e,t){var n=typeof e,r="boolean"==typeof t;return x.isFunction(e)?this.each(function(n){x(this).toggleClass(e.call(this,n,this.className,t),t)}):this.each(function(){if("string"===n){var o,a=0,s=x(this),l=t,u=e.match(T)||[];while(o=u[a++])l=r?l:!s.hasClass(o),s[l?"addClass":"removeClass"](o)}else(n===i||"boolean"===n)&&(this.className&&x._data(this,"__className__",this.className),this.className=this.className||e===!1?"":x._data(this,"__className__")||"")})},hasClass:function(e){var t=" "+e+" ",n=0,r=this.length;for(;r>n;n++)if(1===this[n].nodeType&&(" "+this[n].className+" ").replace(U," ").indexOf(t)>=0)return!0;return!1},val:function(e){var n,r,i,o=this[0];{if(arguments.length)return i=x.isFunction(e),this.each(function(n){var o;1===this.nodeType&&(o=i?e.call(this,n,x(this).val()):e,null==o?o="":"number"==typeof o?o+="":x.isArray(o)&&(o=x.map(o,function(e){return null==e?"":e+""})),r=x.valHooks[this.type]||x.valHooks[this.nodeName.toLowerCase()],r&&"set"in r&&r.set(this,o,"value")!==t||(this.value=o))});if(o)return r=x.valHooks[o.type]||x.valHooks[o.nodeName.toLowerCase()],r&&"get"in r&&(n=r.get(o,"value"))!==t?n:(n=o.value,"string"==typeof n?n.replace(V,""):null==n?"":n)}}}),x.extend({valHooks:{option:{get:function(e){var t=x.find.attr(e,"value");return null!=t?t:e.text}},select:{get:function(e){var t,n,r=e.options,i=e.selectedIndex,o="select-one"===e.type||0>i,a=o?null:[],s=o?i+1:r.length,l=0>i?s:o?i:0;for(;s>l;l++)if(n=r[l],!(!n.selected&&l!==i||(x.support.optDisabled?n.disabled:null!==n.getAttribute("disabled"))||n.parentNode.disabled&&x.nodeName(n.parentNode,"optgroup"))){if(t=x(n).val(),o)return t;a.push(t)}return a},set:function(e,t){var n,r,i=e.options,o=x.makeArray(t),a=i.length;while(a--)r=i[a],(r.selected=x.inArray(x(r).val(),o)>=0)&&(n=!0);return n||(e.selectedIndex=-1),o}}},attr:function(e,n,r){var o,a,s=e.nodeType;if(e&&3!==s&&8!==s&&2!==s)return typeof e.getAttribute===i?x.prop(e,n,r):(1===s&&x.isXMLDoc(e)||(n=n.toLowerCase(),o=x.attrHooks[n]||(x.expr.match.bool.test(n)?X:z)),r===t?o&&"get"in o&&null!==(a=o.get(e,n))?a:(a=x.find.attr(e,n),null==a?t:a):null!==r?o&&"set"in o&&(a=o.set(e,r,n))!==t?a:(e.setAttribute(n,r+""),r):(x.removeAttr(e,n),t))},removeAttr:function(e,t){var n,r,i=0,o=t&&t.match(T);if(o&&1===e.nodeType)while(n=o[i++])r=x.propFix[n]||n,x.expr.match.bool.test(n)?K&&Q||!G.test(n)?e[r]=!1:e[x.camelCase("default-"+n)]=e[r]=!1:x.attr(e,n,""),e.removeAttribute(Q?n:r)},attrHooks:{type:{set:function(e,t){if(!x.support.radioValue&&"radio"===t&&x.nodeName(e,"input")){var n=e.value;return e.setAttribute("type",t),n&&(e.value=n),t}}}},propFix:{"for":"htmlFor","class":"className"},prop:function(e,n,r){var i,o,a,s=e.nodeType;if(e&&3!==s&&8!==s&&2!==s)return a=1!==s||!x.isXMLDoc(e),a&&(n=x.propFix[n]||n,o=x.propHooks[n]),r!==t?o&&"set"in o&&(i=o.set(e,r,n))!==t?i:e[n]=r:o&&"get"in o&&null!==(i=o.get(e,n))?i:e[n]},propHooks:{tabIndex:{get:function(e){var t=x.find.attr(e,"tabindex");return t?parseInt(t,10):Y.test(e.nodeName)||J.test(e.nodeName)&&e.href?0:-1}}}}),X={set:function(e,t,n){return t===!1?x.removeAttr(e,n):K&&Q||!G.test(n)?e.setAttribute(!Q&&x.propFix[n]||n,n):e[x.camelCase("default-"+n)]=e[n]=!0,n}},x.each(x.expr.match.bool.source.match(/\w+/g),function(e,n){var r=x.expr.attrHandle[n]||x.find.attr;x.expr.attrHandle[n]=K&&Q||!G.test(n)?function(e,n,i){var o=x.expr.attrHandle[n],a=i?t:(x.expr.attrHandle[n]=t)!=r(e,n,i)?n.toLowerCase():null;return x.expr.attrHandle[n]=o,a}:function(e,n,r){return r?t:e[x.camelCase("default-"+n)]?n.toLowerCase():null}}),K&&Q||(x.attrHooks.value={set:function(e,n,r){return x.nodeName(e,"input")?(e.defaultValue=n,t):z&&z.set(e,n,r)}}),Q||(z={set:function(e,n,r){var i=e.getAttributeNode(r);return i||e.setAttributeNode(i=e.ownerDocument.createAttribute(r)),i.value=n+="","value"===r||n===e.getAttribute(r)?n:t}},x.expr.attrHandle.id=x.expr.attrHandle.name=x.expr.attrHandle.coords=function(e,n,r){var i;return r?t:(i=e.getAttributeNode(n))&&""!==i.value?i.value:null},x.valHooks.button={get:function(e,n){var r=e.getAttributeNode(n);return r&&r.specified?r.value:t},set:z.set},x.attrHooks.contenteditable={set:function(e,t,n){z.set(e,""===t?!1:t,n)}},x.each(["width","height"],function(e,n){x.attrHooks[n]={set:function(e,r){return""===r?(e.setAttribute(n,"auto"),r):t}}})),x.support.hrefNormalized||x.each(["href","src"],function(e,t){x.propHooks[t]={get:function(e){return e.getAttribute(t,4)}}}),x.support.style||(x.attrHooks.style={get:function(e){return e.style.cssText||t},set:function(e,t){return e.style.cssText=t+""}}),x.support.optSelected||(x.propHooks.selected={get:function(e){var t=e.parentNode;return t&&(t.selectedIndex,t.parentNode&&t.parentNode.selectedIndex),null}}),x.each(["tabIndex","readOnly","maxLength","cellSpacing","cellPadding","rowSpan","colSpan","useMap","frameBorder","contentEditable"],function(){x.propFix[this.toLowerCase()]=this}),x.support.enctype||(x.propFix.enctype="encoding"),x.each(["radio","checkbox"],function(){x.valHooks[this]={set:function(e,n){return x.isArray(n)?e.checked=x.inArray(x(e).val(),n)>=0:t}},x.support.checkOn||(x.valHooks[this].get=function(e){return null===e.getAttribute("value")?"on":e.value})});var Z=/^(?:input|select|textarea)$/i,et=/^key/,tt=/^(?:mouse|contextmenu)|click/,nt=/^(?:focusinfocus|focusoutblur)$/,rt=/^([^.]*)(?:\.(.+)|)$/;function it(){return!0}function ot(){return!1}function at(){try{return a.activeElement}catch(e){}}x.event={global:{},add:function(e,n,r,o,a){var s,l,u,c,p,f,d,h,g,m,y,v=x._data(e);if(v){r.handler&&(c=r,r=c.handler,a=c.selector),r.guid||(r.guid=x.guid++),(l=v.events)||(l=v.events={}),(f=v.handle)||(f=v.handle=function(e){return typeof x===i||e&&x.event.triggered===e.type?t:x.event.dispatch.apply(f.elem,arguments)},f.elem=e),n=(n||"").match(T)||[""],u=n.length;while(u--)s=rt.exec(n[u])||[],g=y=s[1],m=(s[2]||"").split(".").sort(),g&&(p=x.event.special[g]||{},g=(a?p.delegateType:p.bindType)||g,p=x.event.special[g]||{},d=x.extend({type:g,origType:y,data:o,handler:r,guid:r.guid,selector:a,needsContext:a&&x.expr.match.needsContext.test(a),namespace:m.join(".")},c),(h=l[g])||(h=l[g]=[],h.delegateCount=0,p.setup&&p.setup.call(e,o,m,f)!==!1||(e.addEventListener?e.addEventListener(g,f,!1):e.attachEvent&&e.attachEvent("on"+g,f))),p.add&&(p.add.call(e,d),d.handler.guid||(d.handler.guid=r.guid)),a?h.splice(h.delegateCount++,0,d):h.push(d),x.event.global[g]=!0);e=null}},remove:function(e,t,n,r,i){var o,a,s,l,u,c,p,f,d,h,g,m=x.hasData(e)&&x._data(e);if(m&&(c=m.events)){t=(t||"").match(T)||[""],u=t.length;while(u--)if(s=rt.exec(t[u])||[],d=g=s[1],h=(s[2]||"").split(".").sort(),d){p=x.event.special[d]||{},d=(r?p.delegateType:p.bindType)||d,f=c[d]||[],s=s[2]&&RegExp("(^|\\.)"+h.join("\\.(?:.*\\.|)")+"(\\.|$)"),l=o=f.length;while(o--)a=f[o],!i&&g!==a.origType||n&&n.guid!==a.guid||s&&!s.test(a.namespace)||r&&r!==a.selector&&("**"!==r||!a.selector)||(f.splice(o,1),a.selector&&f.delegateCount--,p.remove&&p.remove.call(e,a));l&&!f.length&&(p.teardown&&p.teardown.call(e,h,m.handle)!==!1||x.removeEvent(e,d,m.handle),delete c[d])}else for(d in c)x.event.remove(e,d+t[u],n,r,!0);x.isEmptyObject(c)&&(delete m.handle,x._removeData(e,"events"))}},trigger:function(n,r,i,o){var s,l,u,c,p,f,d,h=[i||a],g=v.call(n,"type")?n.type:n,m=v.call(n,"namespace")?n.namespace.split("."):[];if(u=f=i=i||a,3!==i.nodeType&&8!==i.nodeType&&!nt.test(g+x.event.triggered)&&(g.indexOf(".")>=0&&(m=g.split("."),g=m.shift(),m.sort()),l=0>g.indexOf(":")&&"on"+g,n=n[x.expando]?n:new x.Event(g,"object"==typeof n&&n),n.isTrigger=o?2:3,n.namespace=m.join("."),n.namespace_re=n.namespace?RegExp("(^|\\.)"+m.join("\\.(?:.*\\.|)")+"(\\.|$)"):null,n.result=t,n.target||(n.target=i),r=null==r?[n]:x.makeArray(r,[n]),p=x.event.special[g]||{},o||!p.trigger||p.trigger.apply(i,r)!==!1)){if(!o&&!p.noBubble&&!x.isWindow(i)){for(c=p.delegateType||g,nt.test(c+g)||(u=u.parentNode);u;u=u.parentNode)h.push(u),f=u;f===(i.ownerDocument||a)&&h.push(f.defaultView||f.parentWindow||e)}d=0;while((u=h[d++])&&!n.isPropagationStopped())n.type=d>1?c:p.bindType||g,s=(x._data(u,"events")||{})[n.type]&&x._data(u,"handle"),s&&s.apply(u,r),s=l&&u[l],s&&x.acceptData(u)&&s.apply&&s.apply(u,r)===!1&&n.preventDefault();if(n.type=g,!o&&!n.isDefaultPrevented()&&(!p._default||p._default.apply(h.pop(),r)===!1)&&x.acceptData(i)&&l&&i[g]&&!x.isWindow(i)){f=i[l],f&&(i[l]=null),x.event.triggered=g;try{i[g]()}catch(y){}x.event.triggered=t,f&&(i[l]=f)}return n.result}},dispatch:function(e){e=x.event.fix(e);var n,r,i,o,a,s=[],l=g.call(arguments),u=(x._data(this,"events")||{})[e.type]||[],c=x.event.special[e.type]||{};if(l[0]=e,e.delegateTarget=this,!c.preDispatch||c.preDispatch.call(this,e)!==!1){s=x.event.handlers.call(this,e,u),n=0;while((o=s[n++])&&!e.isPropagationStopped()){e.currentTarget=o.elem,a=0;while((i=o.handlers[a++])&&!e.isImmediatePropagationStopped())(!e.namespace_re||e.namespace_re.test(i.namespace))&&(e.handleObj=i,e.data=i.data,r=((x.event.special[i.origType]||{}).handle||i.handler).apply(o.elem,l),r!==t&&(e.result=r)===!1&&(e.preventDefault(),e.stopPropagation()))}return c.postDispatch&&c.postDispatch.call(this,e),e.result}},handlers:function(e,n){var r,i,o,a,s=[],l=n.delegateCount,u=e.target;if(l&&u.nodeType&&(!e.button||"click"!==e.type))for(;u!=this;u=u.parentNode||this)if(1===u.nodeType&&(u.disabled!==!0||"click"!==e.type)){for(o=[],a=0;l>a;a++)i=n[a],r=i.selector+" ",o[r]===t&&(o[r]=i.needsContext?x(r,this).index(u)>=0:x.find(r,this,null,[u]).length),o[r]&&o.push(i);o.length&&s.push({elem:u,handlers:o})}return n.length>l&&s.push({elem:this,handlers:n.slice(l)}),s},fix:function(e){if(e[x.expando])return e;var t,n,r,i=e.type,o=e,s=this.fixHooks[i];s||(this.fixHooks[i]=s=tt.test(i)?this.mouseHooks:et.test(i)?this.keyHooks:{}),r=s.props?this.props.concat(s.props):this.props,e=new x.Event(o),t=r.length;while(t--)n=r[t],e[n]=o[n];return e.target||(e.target=o.srcElement||a),3===e.target.nodeType&&(e.target=e.target.parentNode),e.metaKey=!!e.metaKey,s.filter?s.filter(e,o):e},props:"altKey bubbles cancelable ctrlKey currentTarget eventPhase metaKey relatedTarget shiftKey target timeStamp view which".split(" "),fixHooks:{},keyHooks:{props:"char charCode key keyCode".split(" "),filter:function(e,t){return null==e.which&&(e.which=null!=t.charCode?t.charCode:t.keyCode),e}},mouseHooks:{props:"button buttons clientX clientY fromElement offsetX offsetY pageX pageY screenX screenY toElement".split(" "),filter:function(e,n){var r,i,o,s=n.button,l=n.fromElement;return null==e.pageX&&null!=n.clientX&&(i=e.target.ownerDocument||a,o=i.documentElement,r=i.body,e.pageX=n.clientX+(o&&o.scrollLeft||r&&r.scrollLeft||0)-(o&&o.clientLeft||r&&r.clientLeft||0),e.pageY=n.clientY+(o&&o.scrollTop||r&&r.scrollTop||0)-(o&&o.clientTop||r&&r.clientTop||0)),!e.relatedTarget&&l&&(e.relatedTarget=l===e.target?n.toElement:l),e.which||s===t||(e.which=1&s?1:2&s?3:4&s?2:0),e}},special:{load:{noBubble:!0},focus:{trigger:function(){if(this!==at()&&this.focus)try{return this.focus(),!1}catch(e){}},delegateType:"focusin"},blur:{trigger:function(){return this===at()&&this.blur?(this.blur(),!1):t},delegateType:"focusout"},click:{trigger:function(){return x.nodeName(this,"input")&&"checkbox"===this.type&&this.click?(this.click(),!1):t},_default:function(e){return x.nodeName(e.target,"a")}},beforeunload:{postDispatch:function(e){e.result!==t&&(e.originalEvent.returnValue=e.result)}}},simulate:function(e,t,n,r){var i=x.extend(new x.Event,n,{type:e,isSimulated:!0,originalEvent:{}});r?x.event.trigger(i,null,t):x.event.dispatch.call(t,i),i.isDefaultPrevented()&&n.preventDefault()}},x.removeEvent=a.removeEventListener?function(e,t,n){e.removeEventListener&&e.removeEventListener(t,n,!1)}:function(e,t,n){var r="on"+t;e.detachEvent&&(typeof e[r]===i&&(e[r]=null),e.detachEvent(r,n))},x.Event=function(e,n){return this instanceof x.Event?(e&&e.type?(this.originalEvent=e,this.type=e.type,this.isDefaultPrevented=e.defaultPrevented||e.returnValue===!1||e.getPreventDefault&&e.getPreventDefault()?it:ot):this.type=e,n&&x.extend(this,n),this.timeStamp=e&&e.timeStamp||x.now(),this[x.expando]=!0,t):new x.Event(e,n)},x.Event.prototype={isDefaultPrevented:ot,isPropagationStopped:ot,isImmediatePropagationStopped:ot,preventDefault:function(){var e=this.originalEvent;this.isDefaultPrevented=it,e&&(e.preventDefault?e.preventDefault():e.returnValue=!1)},stopPropagation:function(){var e=this.originalEvent;this.isPropagationStopped=it,e&&(e.stopPropagation&&e.stopPropagation(),e.cancelBubble=!0)},stopImmediatePropagation:function(){this.isImmediatePropagationStopped=it,this.stopPropagation()}},x.each({mouseenter:"mouseover",mouseleave:"mouseout"},function(e,t){x.event.special[e]={delegateType:t,bindType:t,handle:function(e){var n,r=this,i=e.relatedTarget,o=e.handleObj;return(!i||i!==r&&!x.contains(r,i))&&(e.type=o.origType,n=o.handler.apply(this,arguments),e.type=t),n}}}),x.support.submitBubbles||(x.event.special.submit={setup:function(){return x.nodeName(this,"form")?!1:(x.event.add(this,"click._submit keypress._submit",function(e){var n=e.target,r=x.nodeName(n,"input")||x.nodeName(n,"button")?n.form:t;r&&!x._data(r,"submitBubbles")&&(x.event.add(r,"submit._submit",function(e){e._submit_bubble=!0}),x._data(r,"submitBubbles",!0))}),t)},postDispatch:function(e){e._submit_bubble&&(delete e._submit_bubble,this.parentNode&&!e.isTrigger&&x.event.simulate("submit",this.parentNode,e,!0))},teardown:function(){return x.nodeName(this,"form")?!1:(x.event.remove(this,"._submit"),t)}}),x.support.changeBubbles||(x.event.special.change={setup:function(){return Z.test(this.nodeName)?(("checkbox"===this.type||"radio"===this.type)&&(x.event.add(this,"propertychange._change",function(e){"checked"===e.originalEvent.propertyName&&(this._just_changed=!0)}),x.event.add(this,"click._change",function(e){this._just_changed&&!e.isTrigger&&(this._just_changed=!1),x.event.simulate("change",this,e,!0)})),!1):(x.event.add(this,"beforeactivate._change",function(e){var t=e.target;Z.test(t.nodeName)&&!x._data(t,"changeBubbles")&&(x.event.add(t,"change._change",function(e){!this.parentNode||e.isSimulated||e.isTrigger||x.event.simulate("change",this.parentNode,e,!0)}),x._data(t,"changeBubbles",!0))}),t)},handle:function(e){var n=e.target;return this!==n||e.isSimulated||e.isTrigger||"radio"!==n.type&&"checkbox"!==n.type?e.handleObj.handler.apply(this,arguments):t},teardown:function(){return x.event.remove(this,"._change"),!Z.test(this.nodeName)}}),x.support.focusinBubbles||x.each({focus:"focusin",blur:"focusout"},function(e,t){var n=0,r=function(e){x.event.simulate(t,e.target,x.event.fix(e),!0)};x.event.special[t]={setup:function(){0===n++&&a.addEventListener(e,r,!0)},teardown:function(){0===--n&&a.removeEventListener(e,r,!0)}}}),x.fn.extend({on:function(e,n,r,i,o){var a,s;if("object"==typeof e){"string"!=typeof n&&(r=r||n,n=t);for(a in e)this.on(a,n,r,e[a],o);return this}if(null==r&&null==i?(i=n,r=n=t):null==i&&("string"==typeof n?(i=r,r=t):(i=r,r=n,n=t)),i===!1)i=ot;else if(!i)return this;return 1===o&&(s=i,i=function(e){return x().off(e),s.apply(this,arguments)},i.guid=s.guid||(s.guid=x.guid++)),this.each(function(){x.event.add(this,e,i,r,n)})},one:function(e,t,n,r){return this.on(e,t,n,r,1)},off:function(e,n,r){var i,o;if(e&&e.preventDefault&&e.handleObj)return i=e.handleObj,x(e.delegateTarget).off(i.namespace?i.origType+"."+i.namespace:i.origType,i.selector,i.handler),this;if("object"==typeof e){for(o in e)this.off(o,n,e[o]);return this}return(n===!1||"function"==typeof n)&&(r=n,n=t),r===!1&&(r=ot),this.each(function(){x.event.remove(this,e,r,n)})},trigger:function(e,t){return this.each(function(){x.event.trigger(e,t,this)})},triggerHandler:function(e,n){var r=this[0];return r?x.event.trigger(e,n,r,!0):t}});var st=/^.[^:#\[\.,]*$/,lt=/^(?:parents|prev(?:Until|All))/,ut=x.expr.match.needsContext,ct={children:!0,contents:!0,next:!0,prev:!0};x.fn.extend({find:function(e){var t,n=[],r=this,i=r.length;if("string"!=typeof e)return this.pushStack(x(e).filter(function(){for(t=0;i>t;t++)if(x.contains(r[t],this))return!0}));for(t=0;i>t;t++)x.find(e,r[t],n);return n=this.pushStack(i>1?x.unique(n):n),n.selector=this.selector?this.selector+" "+e:e,n},has:function(e){var t,n=x(e,this),r=n.length;return this.filter(function(){for(t=0;r>t;t++)if(x.contains(this,n[t]))return!0})},not:function(e){return this.pushStack(ft(this,e||[],!0))},filter:function(e){return this.pushStack(ft(this,e||[],!1))},is:function(e){return!!ft(this,"string"==typeof e&&ut.test(e)?x(e):e||[],!1).length},closest:function(e,t){var n,r=0,i=this.length,o=[],a=ut.test(e)||"string"!=typeof e?x(e,t||this.context):0;for(;i>r;r++)for(n=this[r];n&&n!==t;n=n.parentNode)if(11>n.nodeType&&(a?a.index(n)>-1:1===n.nodeType&&x.find.matchesSelector(n,e))){n=o.push(n);break}return this.pushStack(o.length>1?x.unique(o):o)},index:function(e){return e?"string"==typeof e?x.inArray(this[0],x(e)):x.inArray(e.jquery?e[0]:e,this):this[0]&&this[0].parentNode?this.first().prevAll().length:-1},add:function(e,t){var n="string"==typeof e?x(e,t):x.makeArray(e&&e.nodeType?[e]:e),r=x.merge(this.get(),n);return this.pushStack(x.unique(r))},addBack:function(e){return this.add(null==e?this.prevObject:this.prevObject.filter(e))}});function pt(e,t){do e=e[t];while(e&&1!==e.nodeType);return e}x.each({parent:function(e){var t=e.parentNode;return t&&11!==t.nodeType?t:null},parents:function(e){return x.dir(e,"parentNode")},parentsUntil:function(e,t,n){return x.dir(e,"parentNode",n)},next:function(e){return pt(e,"nextSibling")},prev:function(e){return pt(e,"previousSibling")},nextAll:function(e){return x.dir(e,"nextSibling")},prevAll:function(e){return x.dir(e,"previousSibling")},nextUntil:function(e,t,n){return x.dir(e,"nextSibling",n)},prevUntil:function(e,t,n){return x.dir(e,"previousSibling",n)},siblings:function(e){return x.sibling((e.parentNode||{}).firstChild,e)},children:function(e){return x.sibling(e.firstChild)},contents:function(e){return x.nodeName(e,"iframe")?e.contentDocument||e.contentWindow.document:x.merge([],e.childNodes)}},function(e,t){x.fn[e]=function(n,r){var i=x.map(this,t,n);return"Until"!==e.slice(-5)&&(r=n),r&&"string"==typeof r&&(i=x.filter(r,i)),this.length>1&&(ct[e]||(i=x.unique(i)),lt.test(e)&&(i=i.reverse())),this.pushStack(i)}}),x.extend({filter:function(e,t,n){var r=t[0];return n&&(e=":not("+e+")"),1===t.length&&1===r.nodeType?x.find.matchesSelector(r,e)?[r]:[]:x.find.matches(e,x.grep(t,function(e){return 1===e.nodeType}))},dir:function(e,n,r){var i=[],o=e[n];while(o&&9!==o.nodeType&&(r===t||1!==o.nodeType||!x(o).is(r)))1===o.nodeType&&i.push(o),o=o[n];return i},sibling:function(e,t){var n=[];for(;e;e=e.nextSibling)1===e.nodeType&&e!==t&&n.push(e);return n}});function ft(e,t,n){if(x.isFunction(t))return x.grep(e,function(e,r){return!!t.call(e,r,e)!==n});if(t.nodeType)return x.grep(e,function(e){return e===t!==n});if("string"==typeof t){if(st.test(t))return x.filter(t,e,n);t=x.filter(t,e)}return x.grep(e,function(e){return x.inArray(e,t)>=0!==n})}function dt(e){var t=ht.split("|"),n=e.createDocumentFragment();if(n.createElement)while(t.length)n.createElement(t.pop());return n}var ht="abbr|article|aside|audio|bdi|canvas|data|datalist|details|figcaption|figure|footer|header|hgroup|mark|meter|nav|output|progress|section|summary|time|video",gt=/ jQuery\d+="(?:null|\d+)"/g,mt=RegExp("<(?:"+ht+")[\\s/>]","i"),yt=/^\s+/,vt=/<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/gi,bt=/<([\w:]+)/,xt=/<tbody/i,wt=/<|&#?\w+;/,Tt=/<(?:script|style|link)/i,Ct=/^(?:checkbox|radio)$/i,Nt=/checked\s*(?:[^=]|=\s*.checked.)/i,kt=/^$|\/(?:java|ecma)script/i,Et=/^true\/(.*)/,St=/^\s*<!(?:\[CDATA\[|--)|(?:\]\]|--)>\s*$/g,At={option:[1,"<select multiple='multiple'>","</select>"],legend:[1,"<fieldset>","</fieldset>"],area:[1,"<map>","</map>"],param:[1,"<object>","</object>"],thead:[1,"<table>","</table>"],tr:[2,"<table><tbody>","</tbody></table>"],col:[2,"<table><tbody></tbody><colgroup>","</colgroup></table>"],td:[3,"<table><tbody><tr>","</tr></tbody></table>"],_default:x.support.htmlSerialize?[0,"",""]:[1,"X<div>","</div>"]},jt=dt(a),Dt=jt.appendChild(a.createElement("div"));At.optgroup=At.option,At.tbody=At.tfoot=At.colgroup=At.caption=At.thead,At.th=At.td,x.fn.extend({text:function(e){return x.access(this,function(e){return e===t?x.text(this):this.empty().append((this[0]&&this[0].ownerDocument||a).createTextNode(e))},null,e,arguments.length)},append:function(){return this.domManip(arguments,function(e){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var t=Lt(this,e);t.appendChild(e)}})},prepend:function(){return this.domManip(arguments,function(e){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var t=Lt(this,e);t.insertBefore(e,t.firstChild)}})},before:function(){return this.domManip(arguments,function(e){this.parentNode&&this.parentNode.insertBefore(e,this)})},after:function(){return this.domManip(arguments,function(e){this.parentNode&&this.parentNode.insertBefore(e,this.nextSibling)})},remove:function(e,t){var n,r=e?x.filter(e,this):this,i=0;for(;null!=(n=r[i]);i++)t||1!==n.nodeType||x.cleanData(Ft(n)),n.parentNode&&(t&&x.contains(n.ownerDocument,n)&&_t(Ft(n,"script")),n.parentNode.removeChild(n));return this},empty:function(){var e,t=0;for(;null!=(e=this[t]);t++){1===e.nodeType&&x.cleanData(Ft(e,!1));while(e.firstChild)e.removeChild(e.firstChild);e.options&&x.nodeName(e,"select")&&(e.options.length=0)}return this},clone:function(e,t){return e=null==e?!1:e,t=null==t?e:t,this.map(function(){return x.clone(this,e,t)})},html:function(e){return x.access(this,function(e){var n=this[0]||{},r=0,i=this.length;if(e===t)return 1===n.nodeType?n.innerHTML.replace(gt,""):t;if(!("string"!=typeof e||Tt.test(e)||!x.support.htmlSerialize&&mt.test(e)||!x.support.leadingWhitespace&&yt.test(e)||At[(bt.exec(e)||["",""])[1].toLowerCase()])){e=e.replace(vt,"<$1></$2>");try{for(;i>r;r++)n=this[r]||{},1===n.nodeType&&(x.cleanData(Ft(n,!1)),n.innerHTML=e);n=0}catch(o){}}n&&this.empty().append(e)},null,e,arguments.length)},replaceWith:function(){var e=x.map(this,function(e){return[e.nextSibling,e.parentNode]}),t=0;return this.domManip(arguments,function(n){var r=e[t++],i=e[t++];i&&(r&&r.parentNode!==i&&(r=this.nextSibling),x(this).remove(),i.insertBefore(n,r))},!0),t?this:this.remove()},detach:function(e){return this.remove(e,!0)},domManip:function(e,t,n){e=d.apply([],e);var r,i,o,a,s,l,u=0,c=this.length,p=this,f=c-1,h=e[0],g=x.isFunction(h);if(g||!(1>=c||"string"!=typeof h||x.support.checkClone)&&Nt.test(h))return this.each(function(r){var i=p.eq(r);g&&(e[0]=h.call(this,r,i.html())),i.domManip(e,t,n)});if(c&&(l=x.buildFragment(e,this[0].ownerDocument,!1,!n&&this),r=l.firstChild,1===l.childNodes.length&&(l=r),r)){for(a=x.map(Ft(l,"script"),Ht),o=a.length;c>u;u++)i=l,u!==f&&(i=x.clone(i,!0,!0),o&&x.merge(a,Ft(i,"script"))),t.call(this[u],i,u);if(o)for(s=a[a.length-1].ownerDocument,x.map(a,qt),u=0;o>u;u++)i=a[u],kt.test(i.type||"")&&!x._data(i,"globalEval")&&x.contains(s,i)&&(i.src?x._evalUrl(i.src):x.globalEval((i.text||i.textContent||i.innerHTML||"").replace(St,"")));l=r=null}return this}});function Lt(e,t){return x.nodeName(e,"table")&&x.nodeName(1===t.nodeType?t:t.firstChild,"tr")?e.getElementsByTagName("tbody")[0]||e.appendChild(e.ownerDocument.createElement("tbody")):e}function Ht(e){return e.type=(null!==x.find.attr(e,"type"))+"/"+e.type,e}function qt(e){var t=Et.exec(e.type);return t?e.type=t[1]:e.removeAttribute("type"),e}function _t(e,t){var n,r=0;for(;null!=(n=e[r]);r++)x._data(n,"globalEval",!t||x._data(t[r],"globalEval"))}function Mt(e,t){if(1===t.nodeType&&x.hasData(e)){var n,r,i,o=x._data(e),a=x._data(t,o),s=o.events;if(s){delete a.handle,a.events={};for(n in s)for(r=0,i=s[n].length;i>r;r++)x.event.add(t,n,s[n][r])}a.data&&(a.data=x.extend({},a.data))}}function Ot(e,t){var n,r,i;if(1===t.nodeType){if(n=t.nodeName.toLowerCase(),!x.support.noCloneEvent&&t[x.expando]){i=x._data(t);for(r in i.events)x.removeEvent(t,r,i.handle);t.removeAttribute(x.expando)}"script"===n&&t.text!==e.text?(Ht(t).text=e.text,qt(t)):"object"===n?(t.parentNode&&(t.outerHTML=e.outerHTML),x.support.html5Clone&&e.innerHTML&&!x.trim(t.innerHTML)&&(t.innerHTML=e.innerHTML)):"input"===n&&Ct.test(e.type)?(t.defaultChecked=t.checked=e.checked,t.value!==e.value&&(t.value=e.value)):"option"===n?t.defaultSelected=t.selected=e.defaultSelected:("input"===n||"textarea"===n)&&(t.defaultValue=e.defaultValue)}}x.each({appendTo:"append",prependTo:"prepend",insertBefore:"before",insertAfter:"after",replaceAll:"replaceWith"},function(e,t){x.fn[e]=function(e){var n,r=0,i=[],o=x(e),a=o.length-1;for(;a>=r;r++)n=r===a?this:this.clone(!0),x(o[r])[t](n),h.apply(i,n.get());return this.pushStack(i)}});function Ft(e,n){var r,o,a=0,s=typeof e.getElementsByTagName!==i?e.getElementsByTagName(n||"*"):typeof e.querySelectorAll!==i?e.querySelectorAll(n||"*"):t;if(!s)for(s=[],r=e.childNodes||e;null!=(o=r[a]);a++)!n||x.nodeName(o,n)?s.push(o):x.merge(s,Ft(o,n));return n===t||n&&x.nodeName(e,n)?x.merge([e],s):s}function Bt(e){Ct.test(e.type)&&(e.defaultChecked=e.checked)}x.extend({clone:function(e,t,n){var r,i,o,a,s,l=x.contains(e.ownerDocument,e);if(x.support.html5Clone||x.isXMLDoc(e)||!mt.test("<"+e.nodeName+">")?o=e.cloneNode(!0):(Dt.innerHTML=e.outerHTML,Dt.removeChild(o=Dt.firstChild)),!(x.support.noCloneEvent&&x.support.noCloneChecked||1!==e.nodeType&&11!==e.nodeType||x.isXMLDoc(e)))for(r=Ft(o),s=Ft(e),a=0;null!=(i=s[a]);++a)r[a]&&Ot(i,r[a]);if(t)if(n)for(s=s||Ft(e),r=r||Ft(o),a=0;null!=(i=s[a]);a++)Mt(i,r[a]);else Mt(e,o);return r=Ft(o,"script"),r.length>0&&_t(r,!l&&Ft(e,"script")),r=s=i=null,o},buildFragment:function(e,t,n,r){var i,o,a,s,l,u,c,p=e.length,f=dt(t),d=[],h=0;for(;p>h;h++)if(o=e[h],o||0===o)if("object"===x.type(o))x.merge(d,o.nodeType?[o]:o);else if(wt.test(o)){s=s||f.appendChild(t.createElement("div")),l=(bt.exec(o)||["",""])[1].toLowerCase(),c=At[l]||At._default,s.innerHTML=c[1]+o.replace(vt,"<$1></$2>")+c[2],i=c[0];while(i--)s=s.lastChild;if(!x.support.leadingWhitespace&&yt.test(o)&&d.push(t.createTextNode(yt.exec(o)[0])),!x.support.tbody){o="table"!==l||xt.test(o)?"<table>"!==c[1]||xt.test(o)?0:s:s.firstChild,i=o&&o.childNodes.length;while(i--)x.nodeName(u=o.childNodes[i],"tbody")&&!u.childNodes.length&&o.removeChild(u)}x.merge(d,s.childNodes),s.textContent="";while(s.firstChild)s.removeChild(s.firstChild);s=f.lastChild}else d.push(t.createTextNode(o));s&&f.removeChild(s),x.support.appendChecked||x.grep(Ft(d,"input"),Bt),h=0;while(o=d[h++])if((!r||-1===x.inArray(o,r))&&(a=x.contains(o.ownerDocument,o),s=Ft(f.appendChild(o),"script"),a&&_t(s),n)){i=0;while(o=s[i++])kt.test(o.type||"")&&n.push(o)}return s=null,f},cleanData:function(e,t){var n,r,o,a,s=0,l=x.expando,u=x.cache,c=x.support.deleteExpando,f=x.event.special;for(;null!=(n=e[s]);s++)if((t||x.acceptData(n))&&(o=n[l],a=o&&u[o])){if(a.events)for(r in a.events)f[r]?x.event.remove(n,r):x.removeEvent(n,r,a.handle);u[o]&&(delete u[o],c?delete n[l]:typeof n.removeAttribute!==i?n.removeAttribute(l):n[l]=null,p.push(o))}},_evalUrl:function(e){return x.ajax({url:e,type:"GET",dataType:"script",async:!1,global:!1,"throws":!0})}}),x.fn.extend({wrapAll:function(e){if(x.isFunction(e))return this.each(function(t){x(this).wrapAll(e.call(this,t))});if(this[0]){var t=x(e,this[0].ownerDocument).eq(0).clone(!0);this[0].parentNode&&t.insertBefore(this[0]),t.map(function(){var e=this;while(e.firstChild&&1===e.firstChild.nodeType)e=e.firstChild;return e}).append(this)}return this},wrapInner:function(e){return x.isFunction(e)?this.each(function(t){x(this).wrapInner(e.call(this,t))}):this.each(function(){var t=x(this),n=t.contents();n.length?n.wrapAll(e):t.append(e)})},wrap:function(e){var t=x.isFunction(e);return this.each(function(n){x(this).wrapAll(t?e.call(this,n):e)})},unwrap:function(){return this.parent().each(function(){x.nodeName(this,"body")||x(this).replaceWith(this.childNodes)}).end()}});var Pt,Rt,Wt,$t=/alpha\([^)]*\)/i,It=/opacity\s*=\s*([^)]*)/,zt=/^(top|right|bottom|left)$/,Xt=/^(none|table(?!-c[ea]).+)/,Ut=/^margin/,Vt=RegExp("^("+w+")(.*)$","i"),Yt=RegExp("^("+w+")(?!px)[a-z%]+$","i"),Jt=RegExp("^([+-])=("+w+")","i"),Gt={BODY:"block"},Qt={position:"absolute",visibility:"hidden",display:"block"},Kt={letterSpacing:0,fontWeight:400},Zt=["Top","Right","Bottom","Left"],en=["Webkit","O","Moz","ms"];function tn(e,t){if(t in e)return t;var n=t.charAt(0).toUpperCase()+t.slice(1),r=t,i=en.length;while(i--)if(t=en[i]+n,t in e)return t;return r}function nn(e,t){return e=t||e,"none"===x.css(e,"display")||!x.contains(e.ownerDocument,e)}function rn(e,t){var n,r,i,o=[],a=0,s=e.length;for(;s>a;a++)r=e[a],r.style&&(o[a]=x._data(r,"olddisplay"),n=r.style.display,t?(o[a]||"none"!==n||(r.style.display=""),""===r.style.display&&nn(r)&&(o[a]=x._data(r,"olddisplay",ln(r.nodeName)))):o[a]||(i=nn(r),(n&&"none"!==n||!i)&&x._data(r,"olddisplay",i?n:x.css(r,"display"))));for(a=0;s>a;a++)r=e[a],r.style&&(t&&"none"!==r.style.display&&""!==r.style.display||(r.style.display=t?o[a]||"":"none"));return e}x.fn.extend({css:function(e,n){return x.access(this,function(e,n,r){var i,o,a={},s=0;if(x.isArray(n)){for(o=Rt(e),i=n.length;i>s;s++)a[n[s]]=x.css(e,n[s],!1,o);return a}return r!==t?x.style(e,n,r):x.css(e,n)},e,n,arguments.length>1)},show:function(){return rn(this,!0)},hide:function(){return rn(this)},toggle:function(e){var t="boolean"==typeof e;return this.each(function(){(t?e:nn(this))?x(this).show():x(this).hide()})}}),x.extend({cssHooks:{opacity:{get:function(e,t){if(t){var n=Wt(e,"opacity");return""===n?"1":n}}}},cssNumber:{columnCount:!0,fillOpacity:!0,fontWeight:!0,lineHeight:!0,opacity:!0,orphans:!0,widows:!0,zIndex:!0,zoom:!0},cssProps:{"float":x.support.cssFloat?"cssFloat":"styleFloat"},style:function(e,n,r,i){if(e&&3!==e.nodeType&&8!==e.nodeType&&e.style){var o,a,s,l=x.camelCase(n),u=e.style;if(n=x.cssProps[l]||(x.cssProps[l]=tn(u,l)),s=x.cssHooks[n]||x.cssHooks[l],r===t)return s&&"get"in s&&(o=s.get(e,!1,i))!==t?o:u[n];if(a=typeof r,"string"===a&&(o=Jt.exec(r))&&(r=(o[1]+1)*o[2]+parseFloat(x.css(e,n)),a="number"),!(null==r||"number"===a&&isNaN(r)||("number"!==a||x.cssNumber[l]||(r+="px"),x.support.clearCloneStyle||""!==r||0!==n.indexOf("background")||(u[n]="inherit"),s&&"set"in s&&(r=s.set(e,r,i))===t)))try{u[n]=r}catch(c){}}},css:function(e,n,r,i){var o,a,s,l=x.camelCase(n);return n=x.cssProps[l]||(x.cssProps[l]=tn(e.style,l)),s=x.cssHooks[n]||x.cssHooks[l],s&&"get"in s&&(a=s.get(e,!0,r)),a===t&&(a=Wt(e,n,i)),"normal"===a&&n in Kt&&(a=Kt[n]),""===r||r?(o=parseFloat(a),r===!0||x.isNumeric(o)?o||0:a):a}}),e.getComputedStyle?(Rt=function(t){return e.getComputedStyle(t,null)},Wt=function(e,n,r){var i,o,a,s=r||Rt(e),l=s?s.getPropertyValue(n)||s[n]:t,u=e.style;return s&&(""!==l||x.contains(e.ownerDocument,e)||(l=x.style(e,n)),Yt.test(l)&&Ut.test(n)&&(i=u.width,o=u.minWidth,a=u.maxWidth,u.minWidth=u.maxWidth=u.width=l,l=s.width,u.width=i,u.minWidth=o,u.maxWidth=a)),l}):a.documentElement.currentStyle&&(Rt=function(e){return e.currentStyle},Wt=function(e,n,r){var i,o,a,s=r||Rt(e),l=s?s[n]:t,u=e.style;return null==l&&u&&u[n]&&(l=u[n]),Yt.test(l)&&!zt.test(n)&&(i=u.left,o=e.runtimeStyle,a=o&&o.left,a&&(o.left=e.currentStyle.left),u.left="fontSize"===n?"1em":l,l=u.pixelLeft+"px",u.left=i,a&&(o.left=a)),""===l?"auto":l});function on(e,t,n){var r=Vt.exec(t);return r?Math.max(0,r[1]-(n||0))+(r[2]||"px"):t}function an(e,t,n,r,i){var o=n===(r?"border":"content")?4:"width"===t?1:0,a=0;for(;4>o;o+=2)"margin"===n&&(a+=x.css(e,n+Zt[o],!0,i)),r?("content"===n&&(a-=x.css(e,"padding"+Zt[o],!0,i)),"margin"!==n&&(a-=x.css(e,"border"+Zt[o]+"Width",!0,i))):(a+=x.css(e,"padding"+Zt[o],!0,i),"padding"!==n&&(a+=x.css(e,"border"+Zt[o]+"Width",!0,i)));return a}function sn(e,t,n){var r=!0,i="width"===t?e.offsetWidth:e.offsetHeight,o=Rt(e),a=x.support.boxSizing&&"border-box"===x.css(e,"boxSizing",!1,o);if(0>=i||null==i){if(i=Wt(e,t,o),(0>i||null==i)&&(i=e.style[t]),Yt.test(i))return i;r=a&&(x.support.boxSizingReliable||i===e.style[t]),i=parseFloat(i)||0}return i+an(e,t,n||(a?"border":"content"),r,o)+"px"}function ln(e){var t=a,n=Gt[e];return n||(n=un(e,t),"none"!==n&&n||(Pt=(Pt||x("<iframe frameborder='0' width='0' height='0'/>").css("cssText","display:block !important")).appendTo(t.documentElement),t=(Pt[0].contentWindow||Pt[0].contentDocument).document,t.write("<!doctype html><html><body>"),t.close(),n=un(e,t),Pt.detach()),Gt[e]=n),n}function un(e,t){var n=x(t.createElement(e)).appendTo(t.body),r=x.css(n[0],"display");return n.remove(),r}x.each(["height","width"],function(e,n){x.cssHooks[n]={get:function(e,r,i){return r?0===e.offsetWidth&&Xt.test(x.css(e,"display"))?x.swap(e,Qt,function(){return sn(e,n,i)}):sn(e,n,i):t},set:function(e,t,r){var i=r&&Rt(e);return on(e,t,r?an(e,n,r,x.support.boxSizing&&"border-box"===x.css(e,"boxSizing",!1,i),i):0)}}}),x.support.opacity||(x.cssHooks.opacity={get:function(e,t){return It.test((t&&e.currentStyle?e.currentStyle.filter:e.style.filter)||"")?.01*parseFloat(RegExp.$1)+"":t?"1":""},set:function(e,t){var n=e.style,r=e.currentStyle,i=x.isNumeric(t)?"alpha(opacity="+100*t+")":"",o=r&&r.filter||n.filter||"";n.zoom=1,(t>=1||""===t)&&""===x.trim(o.replace($t,""))&&n.removeAttribute&&(n.removeAttribute("filter"),""===t||r&&!r.filter)||(n.filter=$t.test(o)?o.replace($t,i):o+" "+i)}}),x(function(){x.support.reliableMarginRight||(x.cssHooks.marginRight={get:function(e,n){return n?x.swap(e,{display:"inline-block"},Wt,[e,"marginRight"]):t}}),!x.support.pixelPosition&&x.fn.position&&x.each(["top","left"],function(e,n){x.cssHooks[n]={get:function(e,r){return r?(r=Wt(e,n),Yt.test(r)?x(e).position()[n]+"px":r):t}}})}),x.expr&&x.expr.filters&&(x.expr.filters.hidden=function(e){return 0>=e.offsetWidth&&0>=e.offsetHeight||!x.support.reliableHiddenOffsets&&"none"===(e.style&&e.style.display||x.css(e,"display"))},x.expr.filters.visible=function(e){return!x.expr.filters.hidden(e)}),x.each({margin:"",padding:"",border:"Width"},function(e,t){x.cssHooks[e+t]={expand:function(n){var r=0,i={},o="string"==typeof n?n.split(" "):[n];for(;4>r;r++)i[e+Zt[r]+t]=o[r]||o[r-2]||o[0];return i}},Ut.test(e)||(x.cssHooks[e+t].set=on)});var cn=/%20/g,pn=/\[\]$/,fn=/\r?\n/g,dn=/^(?:submit|button|image|reset|file)$/i,hn=/^(?:input|select|textarea|keygen)/i;x.fn.extend({serialize:function(){return x.param(this.serializeArray())},serializeArray:function(){return this.map(function(){var e=x.prop(this,"elements");return e?x.makeArray(e):this}).filter(function(){var e=this.type;return this.name&&!x(this).is(":disabled")&&hn.test(this.nodeName)&&!dn.test(e)&&(this.checked||!Ct.test(e))}).map(function(e,t){var n=x(this).val();return null==n?null:x.isArray(n)?x.map(n,function(e){return{name:t.name,value:e.replace(fn,"\r\n")}}):{name:t.name,value:n.replace(fn,"\r\n")}}).get()}}),x.param=function(e,n){var r,i=[],o=function(e,t){t=x.isFunction(t)?t():null==t?"":t,i[i.length]=encodeURIComponent(e)+"="+encodeURIComponent(t)};if(n===t&&(n=x.ajaxSettings&&x.ajaxSettings.traditional),x.isArray(e)||e.jquery&&!x.isPlainObject(e))x.each(e,function(){o(this.name,this.value)});else for(r in e)gn(r,e[r],n,o);return i.join("&").replace(cn,"+")};function gn(e,t,n,r){var i;if(x.isArray(t))x.each(t,function(t,i){n||pn.test(e)?r(e,i):gn(e+"["+("object"==typeof i?t:"")+"]",i,n,r)});else if(n||"object"!==x.type(t))r(e,t);else for(i in t)gn(e+"["+i+"]",t[i],n,r)}x.each("blur focus focusin focusout load resize scroll unload click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave change select submit keydown keypress keyup error contextmenu".split(" "),function(e,t){x.fn[t]=function(e,n){return arguments.length>0?this.on(t,null,e,n):this.trigger(t)}}),x.fn.extend({hover:function(e,t){return this.mouseenter(e).mouseleave(t||e)},bind:function(e,t,n){return this.on(e,null,t,n)},unbind:function(e,t){return this.off(e,null,t)},delegate:function(e,t,n,r){return this.on(t,e,n,r)},undelegate:function(e,t,n){return 1===arguments.length?this.off(e,"**"):this.off(t,e||"**",n)}});var mn,yn,vn=x.now(),bn=/\?/,xn=/#.*$/,wn=/([?&])_=[^&]*/,Tn=/^(.*?):[ \t]*([^\r\n]*)\r?$/gm,Cn=/^(?:about|app|app-storage|.+-extension|file|res|widget):$/,Nn=/^(?:GET|HEAD)$/,kn=/^\/\//,En=/^([\w.+-]+:)(?:\/\/([^\/?#:]*)(?::(\d+)|)|)/,Sn=x.fn.load,An={},jn={},Dn="*/".concat("*");try{yn=o.href}catch(Ln){yn=a.createElement("a"),yn.href="",yn=yn.href}mn=En.exec(yn.toLowerCase())||[];function Hn(e){return function(t,n){"string"!=typeof t&&(n=t,t="*");var r,i=0,o=t.toLowerCase().match(T)||[];if(x.isFunction(n))while(r=o[i++])"+"===r[0]?(r=r.slice(1)||"*",(e[r]=e[r]||[]).unshift(n)):(e[r]=e[r]||[]).push(n)}}function qn(e,n,r,i){var o={},a=e===jn;function s(l){var u;return o[l]=!0,x.each(e[l]||[],function(e,l){var c=l(n,r,i);return"string"!=typeof c||a||o[c]?a?!(u=c):t:(n.dataTypes.unshift(c),s(c),!1)}),u}return s(n.dataTypes[0])||!o["*"]&&s("*")}function _n(e,n){var r,i,o=x.ajaxSettings.flatOptions||{};for(i in n)n[i]!==t&&((o[i]?e:r||(r={}))[i]=n[i]);return r&&x.extend(!0,e,r),e}x.fn.load=function(e,n,r){if("string"!=typeof e&&Sn)return Sn.apply(this,arguments);var i,o,a,s=this,l=e.indexOf(" ");return l>=0&&(i=e.slice(l,e.length),e=e.slice(0,l)),x.isFunction(n)?(r=n,n=t):n&&"object"==typeof n&&(a="POST"),s.length>0&&x.ajax({url:e,type:a,dataType:"html",data:n}).done(function(e){o=arguments,s.html(i?x("<div>").append(x.parseHTML(e)).find(i):e)}).complete(r&&function(e,t){s.each(r,o||[e.responseText,t,e])}),this},x.each(["ajaxStart","ajaxStop","ajaxComplete","ajaxError","ajaxSuccess","ajaxSend"],function(e,t){x.fn[t]=function(e){return this.on(t,e)}}),x.extend({active:0,lastModified:{},etag:{},ajaxSettings:{url:yn,type:"GET",isLocal:Cn.test(mn[1]),global:!0,processData:!0,async:!0,contentType:"application/x-www-form-urlencoded; charset=UTF-8",accepts:{"*":Dn,text:"text/plain",html:"text/html",xml:"application/xml, text/xml",json:"application/json, text/javascript"},contents:{xml:/xml/,html:/html/,json:/json/},responseFields:{xml:"responseXML",text:"responseText",json:"responseJSON"},converters:{"* text":String,"text html":!0,"text json":x.parseJSON,"text xml":x.parseXML},flatOptions:{url:!0,context:!0}},ajaxSetup:function(e,t){return t?_n(_n(e,x.ajaxSettings),t):_n(x.ajaxSettings,e)},ajaxPrefilter:Hn(An),ajaxTransport:Hn(jn),ajax:function(e,n){"object"==typeof e&&(n=e,e=t),n=n||{};var r,i,o,a,s,l,u,c,p=x.ajaxSetup({},n),f=p.context||p,d=p.context&&(f.nodeType||f.jquery)?x(f):x.event,h=x.Deferred(),g=x.Callbacks("once memory"),m=p.statusCode||{},y={},v={},b=0,w="canceled",C={readyState:0,getResponseHeader:function(e){var t;if(2===b){if(!c){c={};while(t=Tn.exec(a))c[t[1].toLowerCase()]=t[2]}t=c[e.toLowerCase()]}return null==t?null:t},getAllResponseHeaders:function(){return 2===b?a:null},setRequestHeader:function(e,t){var n=e.toLowerCase();return b||(e=v[n]=v[n]||e,y[e]=t),this},overrideMimeType:function(e){return b||(p.mimeType=e),this},statusCode:function(e){var t;if(e)if(2>b)for(t in e)m[t]=[m[t],e[t]];else C.always(e[C.status]);return this},abort:function(e){var t=e||w;return u&&u.abort(t),k(0,t),this}};if(h.promise(C).complete=g.add,C.success=C.done,C.error=C.fail,p.url=((e||p.url||yn)+"").replace(xn,"").replace(kn,mn[1]+"//"),p.type=n.method||n.type||p.method||p.type,p.dataTypes=x.trim(p.dataType||"*").toLowerCase().match(T)||[""],null==p.crossDomain&&(r=En.exec(p.url.toLowerCase()),p.crossDomain=!(!r||r[1]===mn[1]&&r[2]===mn[2]&&(r[3]||("http:"===r[1]?"80":"443"))===(mn[3]||("http:"===mn[1]?"80":"443")))),p.data&&p.processData&&"string"!=typeof p.data&&(p.data=x.param(p.data,p.traditional)),qn(An,p,n,C),2===b)return C;l=p.global,l&&0===x.active++&&x.event.trigger("ajaxStart"),p.type=p.type.toUpperCase(),p.hasContent=!Nn.test(p.type),o=p.url,p.hasContent||(p.data&&(o=p.url+=(bn.test(o)?"&":"?")+p.data,delete p.data),p.cache===!1&&(p.url=wn.test(o)?o.replace(wn,"$1_="+vn++):o+(bn.test(o)?"&":"?")+"_="+vn++)),p.ifModified&&(x.lastModified[o]&&C.setRequestHeader("If-Modified-Since",x.lastModified[o]),x.etag[o]&&C.setRequestHeader("If-None-Match",x.etag[o])),(p.data&&p.hasContent&&p.contentType!==!1||n.contentType)&&C.setRequestHeader("Content-Type",p.contentType),C.setRequestHeader("Accept",p.dataTypes[0]&&p.accepts[p.dataTypes[0]]?p.accepts[p.dataTypes[0]]+("*"!==p.dataTypes[0]?", "+Dn+"; q=0.01":""):p.accepts["*"]);for(i in p.headers)C.setRequestHeader(i,p.headers[i]);if(p.beforeSend&&(p.beforeSend.call(f,C,p)===!1||2===b))return C.abort();w="abort";for(i in{success:1,error:1,complete:1})C[i](p[i]);if(u=qn(jn,p,n,C)){C.readyState=1,l&&d.trigger("ajaxSend",[C,p]),p.async&&p.timeout>0&&(s=setTimeout(function(){C.abort("timeout")},p.timeout));try{b=1,u.send(y,k)}catch(N){if(!(2>b))throw N;k(-1,N)}}else k(-1,"No Transport");function k(e,n,r,i){var c,y,v,w,T,N=n;2!==b&&(b=2,s&&clearTimeout(s),u=t,a=i||"",C.readyState=e>0?4:0,c=e>=200&&300>e||304===e,r&&(w=Mn(p,C,r)),w=On(p,w,C,c),c?(p.ifModified&&(T=C.getResponseHeader("Last-Modified"),T&&(x.lastModified[o]=T),T=C.getResponseHeader("etag"),T&&(x.etag[o]=T)),204===e||"HEAD"===p.type?N="nocontent":304===e?N="notmodified":(N=w.state,y=w.data,v=w.error,c=!v)):(v=N,(e||!N)&&(N="error",0>e&&(e=0))),C.status=e,C.statusText=(n||N)+"",c?h.resolveWith(f,[y,N,C]):h.rejectWith(f,[C,N,v]),C.statusCode(m),m=t,l&&d.trigger(c?"ajaxSuccess":"ajaxError",[C,p,c?y:v]),g.fireWith(f,[C,N]),l&&(d.trigger("ajaxComplete",[C,p]),--x.active||x.event.trigger("ajaxStop")))}return C},getJSON:function(e,t,n){return x.get(e,t,n,"json")},getScript:function(e,n){return x.get(e,t,n,"script")}}),x.each(["get","post"],function(e,n){x[n]=function(e,r,i,o){return x.isFunction(r)&&(o=o||i,i=r,r=t),x.ajax({url:e,type:n,dataType:o,data:r,success:i})}});function Mn(e,n,r){var i,o,a,s,l=e.contents,u=e.dataTypes;while("*"===u[0])u.shift(),o===t&&(o=e.mimeType||n.getResponseHeader("Content-Type"));if(o)for(s in l)if(l[s]&&l[s].test(o)){u.unshift(s);break}if(u[0]in r)a=u[0];else{for(s in r){if(!u[0]||e.converters[s+" "+u[0]]){a=s;break}i||(i=s)}a=a||i}return a?(a!==u[0]&&u.unshift(a),r[a]):t}function On(e,t,n,r){var i,o,a,s,l,u={},c=e.dataTypes.slice();if(c[1])for(a in e.converters)u[a.toLowerCase()]=e.converters[a];o=c.shift();while(o)if(e.responseFields[o]&&(n[e.responseFields[o]]=t),!l&&r&&e.dataFilter&&(t=e.dataFilter(t,e.dataType)),l=o,o=c.shift())if("*"===o)o=l;else if("*"!==l&&l!==o){if(a=u[l+" "+o]||u["* "+o],!a)for(i in u)if(s=i.split(" "),s[1]===o&&(a=u[l+" "+s[0]]||u["* "+s[0]])){a===!0?a=u[i]:u[i]!==!0&&(o=s[0],c.unshift(s[1]));break}if(a!==!0)if(a&&e["throws"])t=a(t);else try{t=a(t)}catch(p){return{state:"parsererror",error:a?p:"No conversion from "+l+" to "+o}}}return{state:"success",data:t}}x.ajaxSetup({accepts:{script:"text/javascript, application/javascript, application/ecmascript, application/x-ecmascript"},contents:{script:/(?:java|ecma)script/},converters:{"text script":function(e){return x.globalEval(e),e}}}),x.ajaxPrefilter("script",function(e){e.cache===t&&(e.cache=!1),e.crossDomain&&(e.type="GET",e.global=!1)}),x.ajaxTransport("script",function(e){if(e.crossDomain){var n,r=a.head||x("head")[0]||a.documentElement;return{send:function(t,i){n=a.createElement("script"),n.async=!0,e.scriptCharset&&(n.charset=e.scriptCharset),n.src=e.url,n.onload=n.onreadystatechange=function(e,t){(t||!n.readyState||/loaded|complete/.test(n.readyState))&&(n.onload=n.onreadystatechange=null,n.parentNode&&n.parentNode.removeChild(n),n=null,t||i(200,"success"))},r.insertBefore(n,r.firstChild)},abort:function(){n&&n.onload(t,!0)}}}});var Fn=[],Bn=/(=)\?(?=&|$)|\?\?/;x.ajaxSetup({jsonp:"callback",jsonpCallback:function(){var e=Fn.pop()||x.expando+"_"+vn++;return this[e]=!0,e}}),x.ajaxPrefilter("json jsonp",function(n,r,i){var o,a,s,l=n.jsonp!==!1&&(Bn.test(n.url)?"url":"string"==typeof n.data&&!(n.contentType||"").indexOf("application/x-www-form-urlencoded")&&Bn.test(n.data)&&"data");return l||"jsonp"===n.dataTypes[0]?(o=n.jsonpCallback=x.isFunction(n.jsonpCallback)?n.jsonpCallback():n.jsonpCallback,l?n[l]=n[l].replace(Bn,"$1"+o):n.jsonp!==!1&&(n.url+=(bn.test(n.url)?"&":"?")+n.jsonp+"="+o),n.converters["script json"]=function(){return s||x.error(o+" was not called"),s[0]},n.dataTypes[0]="json",a=e[o],e[o]=function(){s=arguments},i.always(function(){e[o]=a,n[o]&&(n.jsonpCallback=r.jsonpCallback,Fn.push(o)),s&&x.isFunction(a)&&a(s[0]),s=a=t}),"script"):t});var Pn,Rn,Wn=0,$n=e.ActiveXObject&&function(){var e;for(e in Pn)Pn[e](t,!0)};function In(){try{return new e.XMLHttpRequest}catch(t){}}function zn(){try{return new e.ActiveXObject("Microsoft.XMLHTTP")}catch(t){}}x.ajaxSettings.xhr=e.ActiveXObject?function(){return!this.isLocal&&In()||zn()}:In,Rn=x.ajaxSettings.xhr(),x.support.cors=!!Rn&&"withCredentials"in Rn,Rn=x.support.ajax=!!Rn,Rn&&x.ajaxTransport(function(n){if(!n.crossDomain||x.support.cors){var r;return{send:function(i,o){var a,s,l=n.xhr();if(n.username?l.open(n.type,n.url,n.async,n.username,n.password):l.open(n.type,n.url,n.async),n.xhrFields)for(s in n.xhrFields)l[s]=n.xhrFields[s];n.mimeType&&l.overrideMimeType&&l.overrideMimeType(n.mimeType),n.crossDomain||i["X-Requested-With"]||(i["X-Requested-With"]="XMLHttpRequest");try{for(s in i)l.setRequestHeader(s,i[s])}catch(u){}l.send(n.hasContent&&n.data||null),r=function(e,i){var s,u,c,p;try{if(r&&(i||4===l.readyState))if(r=t,a&&(l.onreadystatechange=x.noop,$n&&delete Pn[a]),i)4!==l.readyState&&l.abort();else{p={},s=l.status,u=l.getAllResponseHeaders(),"string"==typeof l.responseText&&(p.text=l.responseText);try{c=l.statusText}catch(f){c=""}s||!n.isLocal||n.crossDomain?1223===s&&(s=204):s=p.text?200:404}}catch(d){i||o(-1,d)}p&&o(s,c,p,u)},n.async?4===l.readyState?setTimeout(r):(a=++Wn,$n&&(Pn||(Pn={},x(e).unload($n)),Pn[a]=r),l.onreadystatechange=r):r()},abort:function(){r&&r(t,!0)}}}});var Xn,Un,Vn=/^(?:toggle|show|hide)$/,Yn=RegExp("^(?:([+-])=|)("+w+")([a-z%]*)$","i"),Jn=/queueHooks$/,Gn=[nr],Qn={"*":[function(e,t){var n=this.createTween(e,t),r=n.cur(),i=Yn.exec(t),o=i&&i[3]||(x.cssNumber[e]?"":"px"),a=(x.cssNumber[e]||"px"!==o&&+r)&&Yn.exec(x.css(n.elem,e)),s=1,l=20;if(a&&a[3]!==o){o=o||a[3],i=i||[],a=+r||1;do s=s||".5",a/=s,x.style(n.elem,e,a+o);while(s!==(s=n.cur()/r)&&1!==s&&--l)}return i&&(a=n.start=+a||+r||0,n.unit=o,n.end=i[1]?a+(i[1]+1)*i[2]:+i[2]),n}]};function Kn(){return setTimeout(function(){Xn=t}),Xn=x.now()}function Zn(e,t,n){var r,i=(Qn[t]||[]).concat(Qn["*"]),o=0,a=i.length;for(;a>o;o++)if(r=i[o].call(n,t,e))return r}function er(e,t,n){var r,i,o=0,a=Gn.length,s=x.Deferred().always(function(){delete l.elem}),l=function(){if(i)return!1;var t=Xn||Kn(),n=Math.max(0,u.startTime+u.duration-t),r=n/u.duration||0,o=1-r,a=0,l=u.tweens.length;for(;l>a;a++)u.tweens[a].run(o);return s.notifyWith(e,[u,o,n]),1>o&&l?n:(s.resolveWith(e,[u]),!1)},u=s.promise({elem:e,props:x.extend({},t),opts:x.extend(!0,{specialEasing:{}},n),originalProperties:t,originalOptions:n,startTime:Xn||Kn(),duration:n.duration,tweens:[],createTween:function(t,n){var r=x.Tween(e,u.opts,t,n,u.opts.specialEasing[t]||u.opts.easing);return u.tweens.push(r),r},stop:function(t){var n=0,r=t?u.tweens.length:0;if(i)return this;for(i=!0;r>n;n++)u.tweens[n].run(1);return t?s.resolveWith(e,[u,t]):s.rejectWith(e,[u,t]),this}}),c=u.props;for(tr(c,u.opts.specialEasing);a>o;o++)if(r=Gn[o].call(u,e,c,u.opts))return r;return x.map(c,Zn,u),x.isFunction(u.opts.start)&&u.opts.start.call(e,u),x.fx.timer(x.extend(l,{elem:e,anim:u,queue:u.opts.queue})),u.progress(u.opts.progress).done(u.opts.done,u.opts.complete).fail(u.opts.fail).always(u.opts.always)}function tr(e,t){var n,r,i,o,a;for(n in e)if(r=x.camelCase(n),i=t[r],o=e[n],x.isArray(o)&&(i=o[1],o=e[n]=o[0]),n!==r&&(e[r]=o,delete e[n]),a=x.cssHooks[r],a&&"expand"in a){o=a.expand(o),delete e[r];for(n in o)n in e||(e[n]=o[n],t[n]=i)}else t[r]=i}x.Animation=x.extend(er,{tweener:function(e,t){x.isFunction(e)?(t=e,e=["*"]):e=e.split(" ");var n,r=0,i=e.length;for(;i>r;r++)n=e[r],Qn[n]=Qn[n]||[],Qn[n].unshift(t)},prefilter:function(e,t){t?Gn.unshift(e):Gn.push(e)}});function nr(e,t,n){var r,i,o,a,s,l,u=this,c={},p=e.style,f=e.nodeType&&nn(e),d=x._data(e,"fxshow");n.queue||(s=x._queueHooks(e,"fx"),null==s.unqueued&&(s.unqueued=0,l=s.empty.fire,s.empty.fire=function(){s.unqueued||l()}),s.unqueued++,u.always(function(){u.always(function(){s.unqueued--,x.queue(e,"fx").length||s.empty.fire()})})),1===e.nodeType&&("height"in t||"width"in t)&&(n.overflow=[p.overflow,p.overflowX,p.overflowY],"inline"===x.css(e,"display")&&"none"===x.css(e,"float")&&(x.support.inlineBlockNeedsLayout&&"inline"!==ln(e.nodeName)?p.zoom=1:p.display="inline-block")),n.overflow&&(p.overflow="hidden",x.support.shrinkWrapBlocks||u.always(function(){p.overflow=n.overflow[0],p.overflowX=n.overflow[1],p.overflowY=n.overflow[2]}));for(r in t)if(i=t[r],Vn.exec(i)){if(delete t[r],o=o||"toggle"===i,i===(f?"hide":"show"))continue;c[r]=d&&d[r]||x.style(e,r)}if(!x.isEmptyObject(c)){d?"hidden"in d&&(f=d.hidden):d=x._data(e,"fxshow",{}),o&&(d.hidden=!f),f?x(e).show():u.done(function(){x(e).hide()}),u.done(function(){var t;x._removeData(e,"fxshow");for(t in c)x.style(e,t,c[t])});for(r in c)a=Zn(f?d[r]:0,r,u),r in d||(d[r]=a.start,f&&(a.end=a.start,a.start="width"===r||"height"===r?1:0))}}function rr(e,t,n,r,i){return new rr.prototype.init(e,t,n,r,i)}x.Tween=rr,rr.prototype={constructor:rr,init:function(e,t,n,r,i,o){this.elem=e,this.prop=n,this.easing=i||"swing",this.options=t,this.start=this.now=this.cur(),this.end=r,this.unit=o||(x.cssNumber[n]?"":"px")},cur:function(){var e=rr.propHooks[this.prop];return e&&e.get?e.get(this):rr.propHooks._default.get(this)},run:function(e){var t,n=rr.propHooks[this.prop];return this.pos=t=this.options.duration?x.easing[this.easing](e,this.options.duration*e,0,1,this.options.duration):e,this.now=(this.end-this.start)*t+this.start,this.options.step&&this.options.step.call(this.elem,this.now,this),n&&n.set?n.set(this):rr.propHooks._default.set(this),this}},rr.prototype.init.prototype=rr.prototype,rr.propHooks={_default:{get:function(e){var t;return null==e.elem[e.prop]||e.elem.style&&null!=e.elem.style[e.prop]?(t=x.css(e.elem,e.prop,""),t&&"auto"!==t?t:0):e.elem[e.prop]},set:function(e){x.fx.step[e.prop]?x.fx.step[e.prop](e):e.elem.style&&(null!=e.elem.style[x.cssProps[e.prop]]||x.cssHooks[e.prop])?x.style(e.elem,e.prop,e.now+e.unit):e.elem[e.prop]=e.now}}},rr.propHooks.scrollTop=rr.propHooks.scrollLeft={set:function(e){e.elem.nodeType&&e.elem.parentNode&&(e.elem[e.prop]=e.now)}},x.each(["toggle","show","hide"],function(e,t){var n=x.fn[t];x.fn[t]=function(e,r,i){return null==e||"boolean"==typeof e?n.apply(this,arguments):this.animate(ir(t,!0),e,r,i)}}),x.fn.extend({fadeTo:function(e,t,n,r){return this.filter(nn).css("opacity",0).show().end().animate({opacity:t},e,n,r)},animate:function(e,t,n,r){var i=x.isEmptyObject(e),o=x.speed(t,n,r),a=function(){var t=er(this,x.extend({},e),o);(i||x._data(this,"finish"))&&t.stop(!0)};return a.finish=a,i||o.queue===!1?this.each(a):this.queue(o.queue,a)},stop:function(e,n,r){var i=function(e){var t=e.stop;delete e.stop,t(r)};return"string"!=typeof e&&(r=n,n=e,e=t),n&&e!==!1&&this.queue(e||"fx",[]),this.each(function(){var t=!0,n=null!=e&&e+"queueHooks",o=x.timers,a=x._data(this);if(n)a[n]&&a[n].stop&&i(a[n]);else for(n in a)a[n]&&a[n].stop&&Jn.test(n)&&i(a[n]);for(n=o.length;n--;)o[n].elem!==this||null!=e&&o[n].queue!==e||(o[n].anim.stop(r),t=!1,o.splice(n,1));(t||!r)&&x.dequeue(this,e)})},finish:function(e){return e!==!1&&(e=e||"fx"),this.each(function(){var t,n=x._data(this),r=n[e+"queue"],i=n[e+"queueHooks"],o=x.timers,a=r?r.length:0;for(n.finish=!0,x.queue(this,e,[]),i&&i.stop&&i.stop.call(this,!0),t=o.length;t--;)o[t].elem===this&&o[t].queue===e&&(o[t].anim.stop(!0),o.splice(t,1));for(t=0;a>t;t++)r[t]&&r[t].finish&&r[t].finish.call(this);delete n.finish})}});function ir(e,t){var n,r={height:e},i=0;for(t=t?1:0;4>i;i+=2-t)n=Zt[i],r["margin"+n]=r["padding"+n]=e;return t&&(r.opacity=r.width=e),r}x.each({slideDown:ir("show"),slideUp:ir("hide"),slideToggle:ir("toggle"),fadeIn:{opacity:"show"},fadeOut:{opacity:"hide"},fadeToggle:{opacity:"toggle"}},function(e,t){x.fn[e]=function(e,n,r){return this.animate(t,e,n,r)}}),x.speed=function(e,t,n){var r=e&&"object"==typeof e?x.extend({},e):{complete:n||!n&&t||x.isFunction(e)&&e,duration:e,easing:n&&t||t&&!x.isFunction(t)&&t};return r.duration=x.fx.off?0:"number"==typeof r.duration?r.duration:r.duration in x.fx.speeds?x.fx.speeds[r.duration]:x.fx.speeds._default,(null==r.queue||r.queue===!0)&&(r.queue="fx"),r.old=r.complete,r.complete=function(){x.isFunction(r.old)&&r.old.call(this),r.queue&&x.dequeue(this,r.queue)},r},x.easing={linear:function(e){return e},swing:function(e){return.5-Math.cos(e*Math.PI)/2}},x.timers=[],x.fx=rr.prototype.init,x.fx.tick=function(){var e,n=x.timers,r=0;for(Xn=x.now();n.length>r;r++)e=n[r],e()||n[r]!==e||n.splice(r--,1);n.length||x.fx.stop(),Xn=t},x.fx.timer=function(e){e()&&x.timers.push(e)&&x.fx.start()},x.fx.interval=13,x.fx.start=function(){Un||(Un=setInterval(x.fx.tick,x.fx.interval))},x.fx.stop=function(){clearInterval(Un),Un=null},x.fx.speeds={slow:600,fast:200,_default:400},x.fx.step={},x.expr&&x.expr.filters&&(x.expr.filters.animated=function(e){return x.grep(x.timers,function(t){return e===t.elem}).length}),x.fn.offset=function(e){if(arguments.length)return e===t?this:this.each(function(t){x.offset.setOffset(this,e,t)});var n,r,o={top:0,left:0},a=this[0],s=a&&a.ownerDocument;if(s)return n=s.documentElement,x.contains(n,a)?(typeof a.getBoundingClientRect!==i&&(o=a.getBoundingClientRect()),r=or(s),{top:o.top+(r.pageYOffset||n.scrollTop)-(n.clientTop||0),left:o.left+(r.pageXOffset||n.scrollLeft)-(n.clientLeft||0)}):o},x.offset={setOffset:function(e,t,n){var r=x.css(e,"position");"static"===r&&(e.style.position="relative");var i=x(e),o=i.offset(),a=x.css(e,"top"),s=x.css(e,"left"),l=("absolute"===r||"fixed"===r)&&x.inArray("auto",[a,s])>-1,u={},c={},p,f;l?(c=i.position(),p=c.top,f=c.left):(p=parseFloat(a)||0,f=parseFloat(s)||0),x.isFunction(t)&&(t=t.call(e,n,o)),null!=t.top&&(u.top=t.top-o.top+p),null!=t.left&&(u.left=t.left-o.left+f),"using"in t?t.using.call(e,u):i.css(u)}},x.fn.extend({position:function(){if(this[0]){var e,t,n={top:0,left:0},r=this[0];return"fixed"===x.css(r,"position")?t=r.getBoundingClientRect():(e=this.offsetParent(),t=this.offset(),x.nodeName(e[0],"html")||(n=e.offset()),n.top+=x.css(e[0],"borderTopWidth",!0),n.left+=x.css(e[0],"borderLeftWidth",!0)),{top:t.top-n.top-x.css(r,"marginTop",!0),left:t.left-n.left-x.css(r,"marginLeft",!0)}}},offsetParent:function(){return this.map(function(){var e=this.offsetParent||s;while(e&&!x.nodeName(e,"html")&&"static"===x.css(e,"position"))e=e.offsetParent;return e||s})}}),x.each({scrollLeft:"pageXOffset",scrollTop:"pageYOffset"},function(e,n){var r=/Y/.test(n);x.fn[e]=function(i){return x.access(this,function(e,i,o){var a=or(e);return o===t?a?n in a?a[n]:a.document.documentElement[i]:e[i]:(a?a.scrollTo(r?x(a).scrollLeft():o,r?o:x(a).scrollTop()):e[i]=o,t)},e,i,arguments.length,null)}});function or(e){return x.isWindow(e)?e:9===e.nodeType?e.defaultView||e.parentWindow:!1}x.each({Height:"height",Width:"width"},function(e,n){x.each({padding:"inner"+e,content:n,"":"outer"+e},function(r,i){x.fn[i]=function(i,o){var a=arguments.length&&(r||"boolean"!=typeof i),s=r||(i===!0||o===!0?"margin":"border");return x.access(this,function(n,r,i){var o;return x.isWindow(n)?n.document.documentElement["client"+e]:9===n.nodeType?(o=n.documentElement,Math.max(n.body["scroll"+e],o["scroll"+e],n.body["offset"+e],o["offset"+e],o["client"+e])):i===t?x.css(n,r,s):x.style(n,r,i,s)},n,a?i:t,a,null)}})}),x.fn.size=function(){return this.length},x.fn.andSelf=x.fn.addBack,"object"==typeof module&&module&&"object"==typeof module.exports?module.exports=x:(e.jQuery=e.$=x,"function"==typeof define&&define.amd&&define("jquery_1_10_1",[],function(){return x})),INTELNAV.jQuery_1_10_1=x})(window);
};

var load_jquery_migrate1_2_1 = function() {
	// ! jQuery Migrate v1.2.1 | (c) 2005, 2013 jQuery Foundation, Inc. and other contributors | jquery.org/license
	INTELNAV.jQuery_1_10_1.migrateMute===void 0&&(jQuery.migrateMute=!0),function(e,t,n){function r(n){var r=t.console;i[n]||(i[n]=!0,e.migrateWarnings.push(n),r&&r.warn&&!e.migrateMute&&(r.warn("JQMIGRATE: "+n),e.migrateTrace&&r.trace&&r.trace()))}function a(t,a,i,o){if(Object.defineProperty)try{return Object.defineProperty(t,a,{configurable:!0,enumerable:!0,get:function(){return r(o),i},set:function(e){r(o),i=e}}),n}catch(s){}e._definePropertyBroken=!0,t[a]=i}var i={};e.migrateWarnings=[],!e.migrateMute&&t.console&&t.console.log&&t.console.log("JQMIGRATE: Logging is active"),e.migrateTrace===n&&(e.migrateTrace=!0),e.migrateReset=function(){i={},e.migrateWarnings.length=0},"BackCompat"===document.compatMode&&r("jQuery is not compatible with Quirks Mode");var o=e("<input/>",{size:1}).attr("size")&&e.attrFn,s=e.attr,u=e.attrHooks.value&&e.attrHooks.value.get||function(){return null},c=e.attrHooks.value&&e.attrHooks.value.set||function(){return n},l=/^(?:input|button)$/i,d=/^[238]$/,p=/^(?:autofocus|autoplay|async|checked|controls|defer|disabled|hidden|loop|multiple|open|readonly|required|scoped|selected)$/i,f=/^(?:checked|selected)$/i;a(e,"attrFn",o||{},"jQuery.attrFn is deprecated"),e.attr=function(t,a,i,u){var c=a.toLowerCase(),g=t&&t.nodeType;return u&&(4>s.length&&r("jQuery.fn.attr( props, pass ) is deprecated"),t&&!d.test(g)&&(o?a in o:e.isFunction(e.fn[a])))?e(t)[a](i):("type"===a&&i!==n&&l.test(t.nodeName)&&t.parentNode&&r("Can't change the 'type' of an input or button in IE 6/7/8"),!e.attrHooks[c]&&p.test(c)&&(e.attrHooks[c]={get:function(t,r){var a,i=e.prop(t,r);return i===!0||"boolean"!=typeof i&&(a=t.getAttributeNode(r))&&a.nodeValue!==!1?r.toLowerCase():n},set:function(t,n,r){var a;return n===!1?e.removeAttr(t,r):(a=e.propFix[r]||r,a in t&&(t[a]=!0),t.setAttribute(r,r.toLowerCase())),r}},f.test(c)&&r("jQuery.fn.attr('"+c+"') may use property instead of attribute")),s.call(e,t,a,i))},e.attrHooks.value={get:function(e,t){var n=(e.nodeName||"").toLowerCase();return"button"===n?u.apply(this,arguments):("input"!==n&&"option"!==n&&r("jQuery.fn.attr('value') no longer gets properties"),t in e?e.value:null)},set:function(e,t){var a=(e.nodeName||"").toLowerCase();return"button"===a?c.apply(this,arguments):("input"!==a&&"option"!==a&&r("jQuery.fn.attr('value', val) no longer sets properties"),e.value=t,n)}};var g,h,v=e.fn.init,m=e.parseJSON,y=/^([^<]*)(<[\w\W]+>)([^>]*)$/;e.fn.init=function(t,n,a){var i;return t&&"string"==typeof t&&!e.isPlainObject(n)&&(i=y.exec(e.trim(t)))&&i[0]&&("<"!==t.charAt(0)&&r("$(html) HTML strings must start with '<' character"),i[3]&&r("$(html) HTML text after last tag is ignored"),"#"===i[0].charAt(0)&&(r("HTML string cannot start with a '#' character"),e.error("JQMIGRATE: Invalid selector string (XSS)")),n&&n.context&&(n=n.context),e.parseHTML)?v.call(this,e.parseHTML(i[2],n,!0),n,a):v.apply(this,arguments)},e.fn.init.prototype=e.fn,e.parseJSON=function(e){return e||null===e?m.apply(this,arguments):(r("jQuery.parseJSON requires a valid JSON string"),null)},e.uaMatch=function(e){e=e.toLowerCase();var t=/(chrome)[ \/]([\w.]+)/.exec(e)||/(webkit)[ \/]([\w.]+)/.exec(e)||/(opera)(?:.*version|)[ \/]([\w.]+)/.exec(e)||/(msie) ([\w.]+)/.exec(e)||0>e.indexOf("compatible")&&/(mozilla)(?:.*? rv:([\w.]+)|)/.exec(e)||[];return{browser:t[1]||"",version:t[2]||"0"}},e.browser||(g=e.uaMatch(navigator.userAgent),h={},g.browser&&(h[g.browser]=!0,h.version=g.version),h.chrome?h.webkit=!0:h.webkit&&(h.safari=!0),e.browser=h),a(e,"browser",e.browser,"jQuery.browser is deprecated"),e.sub=function(){function t(e,n){return new t.fn.init(e,n)}e.extend(!0,t,this),t.superclass=this,t.fn=t.prototype=this(),t.fn.constructor=t,t.sub=this.sub,t.fn.init=function(r,a){return a&&a instanceof e&&!(a instanceof t)&&(a=t(a)),e.fn.init.call(this,r,a,n)},t.fn.init.prototype=t.fn;var n=t(document);return r("jQuery.sub() is deprecated"),t},e.ajaxSetup({converters:{"text json":e.parseJSON}});var b=e.fn.data;e.fn.data=function(t){var a,i,o=this[0];return!o||"events"!==t||1!==arguments.length||(a=e.data(o,t),i=e._data(o,t),a!==n&&a!==i||i===n)?b.apply(this,arguments):(r("Use of jQuery.fn.data('events') is deprecated"),i)};var j=/\/(java|ecma)script/i,w=e.fn.andSelf||e.fn.addBack;e.fn.andSelf=function(){return r("jQuery.fn.andSelf() replaced by jQuery.fn.addBack()"),w.apply(this,arguments)},e.clean||(e.clean=function(t,a,i,o){a=a||document,a=!a.nodeType&&a[0]||a,a=a.ownerDocument||a,r("jQuery.clean() is deprecated");var s,u,c,l,d=[];if(e.merge(d,e.buildFragment(t,a).childNodes),i)for(c=function(e){return!e.type||j.test(e.type)?o?o.push(e.parentNode?e.parentNode.removeChild(e):e):i.appendChild(e):n},s=0;null!=(u=d[s]);s++)e.nodeName(u,"script")&&c(u)||(i.appendChild(u),u.getElementsByTagName!==n&&(l=e.grep(e.merge([],u.getElementsByTagName("script")),c),d.splice.apply(d,[s+1,0].concat(l)),s+=l.length));return d});var Q=e.event.add,x=e.event.remove,k=e.event.trigger,N=e.fn.toggle,T=e.fn.live,M=e.fn.die,S="ajaxStart|ajaxStop|ajaxSend|ajaxComplete|ajaxError|ajaxSuccess",C=RegExp("\\b(?:"+S+")\\b"),H=/(?:^|\s)hover(\.\S+|)\b/,A=function(t){return"string"!=typeof t||e.event.special.hover?t:(H.test(t)&&r("'hover' pseudo-event is deprecated, use 'mouseenter mouseleave'"),t&&t.replace(H,"mouseenter$1 mouseleave$1"))};e.event.props&&"attrChange"!==e.event.props[0]&&e.event.props.unshift("attrChange","attrName","relatedNode","srcElement"),e.event.dispatch&&a(e.event,"handle",e.event.dispatch,"jQuery.event.handle is undocumented and deprecated"),e.event.add=function(e,t,n,a,i){e!==document&&C.test(t)&&r("AJAX events should be attached to document: "+t),Q.call(this,e,A(t||""),n,a,i)},e.event.remove=function(e,t,n,r,a){x.call(this,e,A(t)||"",n,r,a)},e.fn.error=function(){var e=Array.prototype.slice.call(arguments,0);return r("jQuery.fn.error() is deprecated"),e.splice(0,0,"error"),arguments.length?this.bind.apply(this,e):(this.triggerHandler.apply(this,e),this)},e.fn.toggle=function(t,n){if(!e.isFunction(t)||!e.isFunction(n))return N.apply(this,arguments);r("jQuery.fn.toggle(handler, handler...) is deprecated");var a=arguments,i=t.guid||e.guid++,o=0,s=function(n){var r=(e._data(this,"lastToggle"+t.guid)||0)%o;return e._data(this,"lastToggle"+t.guid,r+1),n.preventDefault(),a[r].apply(this,arguments)||!1};for(s.guid=i;a.length>o;)a[o++].guid=i;return this.click(s)},e.fn.live=function(t,n,a){return r("jQuery.fn.live() is deprecated"),T?T.apply(this,arguments):(e(this.context).on(t,this.selector,n,a),this)},e.fn.die=function(t,n){return r("jQuery.fn.die() is deprecated"),M?M.apply(this,arguments):(e(this.context).off(t,this.selector||"**",n),this)},e.event.trigger=function(e,t,n,a){return n||C.test(e)||r("Global events are undocumented and deprecated"),k.call(this,e,t,n||document,a)},e.each(S.split("|"),function(t,n){e.event.special[n]={setup:function(){var t=this;return t!==document&&(e.event.add(document,n+"."+e.guid,function(){e.event.trigger(n,null,t,!0)}),e._data(this,n,e.guid++)),!1},teardown:function(){return this!==document&&e.event.remove(document,n+"."+e._data(this,n)),!1}}})}(INTELNAV.jQuery_1_10_1,window);
};

var load_jquery_modernizer2_6_2 = function() {
	// * Modernizr 2.6.2 (Custom Build) | MIT & BSD
	// * Build: http://modernizr.com/download/#-fontface-backgroundsize-borderimage-borderradius-boxshadow-flexbox-hsla-multiplebgs-opacity-rgba-textshadow-cssanimations-csscolumns-generatedcontent-cssgradients-cssreflections-csstransforms-csstransforms3d-csstransitions-applicationcache-canvas-canvastext-draganddrop-hashchange-history-audio-video-indexeddb-input-inputtypes-localstorage-postmessage-sessionstorage-websockets-websqldatabase-webworkers-geolocation-inlinesvg-smil-svg-svgclippaths-touch-webgl-shiv-mq-cssclasses-addtest-prefixed-teststyles-testprop-testallprops-hasevent-prefixes-domprefixes-load
	;window.Modernizr=function(a,b,c){function D(a){j.cssText=a}function E(a,b){return D(n.join(a+";")+(b||""))}function F(a,b){return typeof a===b}function G(a,b){return!!~(""+a).indexOf(b)}function H(a,b){for(var d in a){var e=a[d];if(!G(e,"-")&&j[e]!==c)return b=="pfx"?e:!0}return!1}function I(a,b,d){for(var e in a){var f=b[a[e]];if(f!==c)return d===!1?a[e]:F(f,"function")?f.bind(d||b):f}return!1}function J(a,b,c){var d=a.charAt(0).toUpperCase()+a.slice(1),e=(a+" "+p.join(d+" ")+d).split(" ");return F(b,"string")||F(b,"undefined")?H(e,b):(e=(a+" "+q.join(d+" ")+d).split(" "),I(e,b,c))}function K(){e.input=function(c){for(var d=0,e=c.length;d<e;d++)u[c[d]]=c[d]in k;return u.list&&(u.list=!!b.createElement("datalist")&&!!a.HTMLDataListElement),u}("autocomplete autofocus list placeholder max min multiple pattern required step".split(" ")),e.inputtypes=function(a){for(var d=0,e,f,h,i=a.length;d<i;d++)k.setAttribute("type",f=a[d]),e=k.type!=="text",e&&(k.value=l,k.style.cssText="position:absolute;visibility:hidden;",/^range$/.test(f)&&k.style.WebkitAppearance!==c?(g.appendChild(k),h=b.defaultView,e=h.getComputedStyle&&h.getComputedStyle(k,null).WebkitAppearance!=="textfield"&&k.offsetHeight!==0,g.removeChild(k)):/^(search|tel)$/.test(f)||(/^(url|email)$/.test(f)?e=k.checkValidity&&k.checkValidity()===!1:e=k.value!=l)),t[a[d]]=!!e;return t}("search tel url email datetime date month week time datetime-local number range color".split(" "))}var d="2.6.2",e={},f=!0,g=b.documentElement,h="modernizr",i=b.createElement(h),j=i.style,k=b.createElement("input"),l=":)",m={}.toString,n=" -webkit- -moz- -o- -ms- ".split(" "),o="Webkit Moz O ms",p=o.split(" "),q=o.toLowerCase().split(" "),r={svg:"http://www.w3.org/2000/svg"},s={},t={},u={},v=[],w=v.slice,x,y=function(a,c,d,e){var f,i,j,k,l=b.createElement("div"),m=b.body,n=m||b.createElement("body");if(parseInt(d,10))while(d--)j=b.createElement("div"),j.id=e?e[d]:h+(d+1),l.appendChild(j);return f=["&#173;",'<style id="s',h,'">',a,"</style>"].join(""),l.id=h,(m?l:n).innerHTML+=f,n.appendChild(l),m||(n.style.background="",n.style.overflow="hidden",k=g.style.overflow,g.style.overflow="hidden",g.appendChild(n)),i=c(l,a),m?l.parentNode.removeChild(l):(n.parentNode.removeChild(n),g.style.overflow=k),!!i},z=function(b){var c=a.matchMedia||a.msMatchMedia;if(c)return c(b).matches;var d;return y("@media "+b+" { #"+h+" { position: absolute; } }",function(b){d=(a.getComputedStyle?getComputedStyle(b,null):b.currentStyle)["position"]=="absolute"}),d},A=function(){function d(d,e){e=e||b.createElement(a[d]||"div"),d="on"+d;var f=d in e;return f||(e.setAttribute||(e=b.createElement("div")),e.setAttribute&&e.removeAttribute&&(e.setAttribute(d,""),f=F(e[d],"function"),F(e[d],"undefined")||(e[d]=c),e.removeAttribute(d))),e=null,f}var a={select:"input",change:"input",submit:"form",reset:"form",error:"img",load:"img",abort:"img"};return d}(),B={}.hasOwnProperty,C;!F(B,"undefined")&&!F(B.call,"undefined")?C=function(a,b){return B.call(a,b)}:C=function(a,b){return b in a&&F(a.constructor.prototype[b],"undefined")},Function.prototype.bind||(Function.prototype.bind=function(b){var c=this;if(typeof c!="function")throw new TypeError;var d=w.call(arguments,1),e=function(){if(this instanceof e){var a=function(){};a.prototype=c.prototype;var f=new a,g=c.apply(f,d.concat(w.call(arguments)));return Object(g)===g?g:f}return c.apply(b,d.concat(w.call(arguments)))};return e}),s.flexbox=function(){return J("flexWrap")},s.canvas=function(){var a=b.createElement("canvas");return!!a.getContext&&!!a.getContext("2d")},s.canvastext=function(){return!!e.canvas&&!!F(b.createElement("canvas").getContext("2d").fillText,"function")},s.webgl=function(){return!!a.WebGLRenderingContext},s.touch=function(){var c;return"ontouchstart"in a||a.DocumentTouch&&b instanceof DocumentTouch?c=!0:y(["@media (",n.join("touch-enabled),("),h,")","{#modernizr{top:9px;position:absolute}}"].join(""),function(a){c=a.offsetTop===9}),c},s.geolocation=function(){return"geolocation"in navigator},s.postmessage=function(){return!!a.postMessage},s.websqldatabase=function(){return!!a.openDatabase},s.indexedDB=function(){return!!J("indexedDB",a)},s.hashchange=function(){return A("hashchange",a)&&(b.documentMode===c||b.documentMode>7)},s.history=function(){return!!a.history&&!!history.pushState},s.draganddrop=function(){var a=b.createElement("div");return"draggable"in a||"ondragstart"in a&&"ondrop"in a},s.websockets=function(){return"WebSocket"in a||"MozWebSocket"in a},s.rgba=function(){return D("background-color:rgba(150,255,150,.5)"),G(j.backgroundColor,"rgba")},s.hsla=function(){return D("background-color:hsla(120,40%,100%,.5)"),G(j.backgroundColor,"rgba")||G(j.backgroundColor,"hsla")},s.multiplebgs=function(){return D("background:url(https://),url(https://),red url(https://)"),/(url\s*\(.*?){3}/.test(j.background)},s.backgroundsize=function(){return J("backgroundSize")},s.borderimage=function(){return J("borderImage")},s.borderradius=function(){return J("borderRadius")},s.boxshadow=function(){return J("boxShadow")},s.textshadow=function(){return b.createElement("div").style.textShadow===""},s.opacity=function(){return E("opacity:.55"),/^0.55$/.test(j.opacity)},s.cssanimations=function(){return J("animationName")},s.csscolumns=function(){return J("columnCount")},s.cssgradients=function(){var a="background-image:",b="gradient(linear,left top,right bottom,from(#9f9),to(white));",c="linear-gradient(left top,#9f9, white);";return D((a+"-webkit- ".split(" ").join(b+a)+n.join(c+a)).slice(0,-a.length)),G(j.backgroundImage,"gradient")},s.cssreflections=function(){return J("boxReflect")},s.csstransforms=function(){return!!J("transform")},s.csstransforms3d=function(){var a=!!J("perspective");return a&&"webkitPerspective"in g.style&&y("@media (transform-3d),(-webkit-transform-3d){#modernizr{left:9px;position:absolute;height:3px;}}",function(b,c){a=b.offsetLeft===9&&b.offsetHeight===3}),a},s.csstransitions=function(){return J("transition")},s.fontface=function(){var a;return y('@font-face {font-family:"font";src:url("https://")}',function(c,d){var e=b.getElementById("smodernizr"),f=e.sheet||e.styleSheet,g=f?f.cssRules&&f.cssRules[0]?f.cssRules[0].cssText:f.cssText||"":"";a=/src/i.test(g)&&g.indexOf(d.split(" ")[0])===0}),a},s.generatedcontent=function(){var a;return y(["#",h,"{font:0/0 a}#",h,':after{content:"',l,'";visibility:hidden;font:3px/1 a}'].join(""),function(b){a=b.offsetHeight>=3}),a},s.video=function(){var a=b.createElement("video"),c=!1;try{if(c=!!a.canPlayType)c=new Boolean(c),c.ogg=a.canPlayType('video/ogg; codecs="theora"').replace(/^no$/,""),c.h264=a.canPlayType('video/mp4; codecs="avc1.42E01E"').replace(/^no$/,""),c.webm=a.canPlayType('video/webm; codecs="vp8, vorbis"').replace(/^no$/,"")}catch(d){}return c},s.audio=function(){var a=b.createElement("audio"),c=!1;try{if(c=!!a.canPlayType)c=new Boolean(c),c.ogg=a.canPlayType('audio/ogg; codecs="vorbis"').replace(/^no$/,""),c.mp3=a.canPlayType("audio/mpeg;").replace(/^no$/,""),c.wav=a.canPlayType('audio/wav; codecs="1"').replace(/^no$/,""),c.m4a=(a.canPlayType("audio/x-m4a;")||a.canPlayType("audio/aac;")).replace(/^no$/,"")}catch(d){}return c},s.localstorage=function(){try{return localStorage.setItem(h,h),localStorage.removeItem(h),!0}catch(a){return!1}},s.sessionstorage=function(){try{return sessionStorage.setItem(h,h),sessionStorage.removeItem(h),!0}catch(a){return!1}},s.webworkers=function(){return!!a.Worker},s.applicationcache=function(){return!!a.applicationCache},s.svg=function(){return!!b.createElementNS&&!!b.createElementNS(r.svg,"svg").createSVGRect},s.inlinesvg=function(){var a=b.createElement("div");return a.innerHTML="<svg/>",(a.firstChild&&a.firstChild.namespaceURI)==r.svg},s.smil=function(){return!!b.createElementNS&&/SVGAnimate/.test(m.call(b.createElementNS(r.svg,"animate")))},s.svgclippaths=function(){return!!b.createElementNS&&/SVGClipPath/.test(m.call(b.createElementNS(r.svg,"clipPath")))};for(var L in s)C(s,L)&&(x=L.toLowerCase(),e[x]=s[L](),v.push((e[x]?"":"no-")+x));return e.input||K(),e.addTest=function(a,b){if(typeof a=="object")for(var d in a)C(a,d)&&e.addTest(d,a[d]);else{a=a.toLowerCase();if(e[a]!==c)return e;b=typeof b=="function"?b():b,typeof f!="undefined"&&f&&(g.className+=" "+(b?"":"no-")+a),e[a]=b}return e},D(""),i=k=null,function(a,b){function k(a,b){var c=a.createElement("p"),d=a.getElementsByTagName("head")[0]||a.documentElement;return c.innerHTML="x<style>"+b+"</style>",d.insertBefore(c.lastChild,d.firstChild)}function l(){var a=r.elements;return typeof a=="string"?a.split(" "):a}function m(a){var b=i[a[g]];return b||(b={},h++,a[g]=h,i[h]=b),b}function n(a,c,f){c||(c=b);if(j)return c.createElement(a);f||(f=m(c));var g;return f.cache[a]?g=f.cache[a].cloneNode():e.test(a)?g=(f.cache[a]=f.createElem(a)).cloneNode():g=f.createElem(a),g.canHaveChildren&&!d.test(a)?f.frag.appendChild(g):g}function o(a,c){a||(a=b);if(j)return a.createDocumentFragment();c=c||m(a);var d=c.frag.cloneNode(),e=0,f=l(),g=f.length;for(;e<g;e++)d.createElement(f[e]);return d}function p(a,b){b.cache||(b.cache={},b.createElem=a.createElement,b.createFrag=a.createDocumentFragment,b.frag=b.createFrag()),a.createElement=function(c){return r.shivMethods?n(c,a,b):b.createElem(c)},a.createDocumentFragment=Function("h,f","return function(){var n=f.cloneNode(),c=n.createElement;h.shivMethods&&("+l().join().replace(/\w+/g,function(a){return b.createElem(a),b.frag.createElement(a),'c("'+a+'")'})+");return n}")(r,b.frag)}function q(a){a||(a=b);var c=m(a);return r.shivCSS&&!f&&!c.hasCSS&&(c.hasCSS=!!k(a,"article,aside,figcaption,figure,footer,header,hgroup,nav,section{display:block}mark{background:#FF0;color:#000}")),j||p(a,c),a}var c=a.html5||{},d=/^<|^(?:button|map|select|textarea|object|iframe|option|optgroup)$/i,e=/^(?:a|b|code|div|fieldset|h1|h2|h3|h4|h5|h6|i|label|li|ol|p|q|span|strong|style|table|tbody|td|th|tr|ul)$/i,f,g="_html5shiv",h=0,i={},j;(function(){try{var a=b.createElement("a");a.innerHTML="<xyz></xyz>",f="hidden"in a,j=a.childNodes.length==1||function(){b.createElement("a");var a=b.createDocumentFragment();return typeof a.cloneNode=="undefined"||typeof a.createDocumentFragment=="undefined"||typeof a.createElement=="undefined"}()}catch(c){f=!0,j=!0}})();var r={elements:c.elements||"abbr article aside audio bdi canvas data datalist details figcaption figure footer header hgroup mark meter nav output progress section summary time video",shivCSS:c.shivCSS!==!1,supportsUnknownElements:j,shivMethods:c.shivMethods!==!1,type:"default",shivDocument:q,createElement:n,createDocumentFragment:o};a.html5=r,q(b)}(this,b),e._version=d,e._prefixes=n,e._domPrefixes=q,e._cssomPrefixes=p,e.mq=z,e.hasEvent=A,e.testProp=function(a){return H([a])},e.testAllProps=J,e.testStyles=y,e.prefixed=function(a,b,c){return b?J(a,b,c):J(a,"pfx")},g.className=g.className.replace(/(^|\s)no-js(\s|$)/,"$1$2")+(f?" js "+v.join(" "):""),e}(this,this.document),function(a,b,c){function d(a){return"[object Function]"==o.call(a)}function e(a){return"string"==typeof a}function f(){}function g(a){return!a||"loaded"==a||"complete"==a||"uninitialized"==a}function h(){var a=p.shift();q=1,a?a.t?m(function(){("c"==a.t?B.injectCss:B.injectJs)(a.s,0,a.a,a.x,a.e,1)},0):(a(),h()):q=0}function i(a,c,d,e,f,i,j){function k(b){if(!o&&g(l.readyState)&&(u.r=o=1,!q&&h(),l.onload=l.onreadystatechange=null,b)){"img"!=a&&m(function(){t.removeChild(l)},50);for(var d in y[c])y[c].hasOwnProperty(d)&&y[c][d].onload()}}var j=j||B.errorTimeout,l=b.createElement(a),o=0,r=0,u={t:d,s:c,e:f,a:i,x:j};1===y[c]&&(r=1,y[c]=[]),"object"==a?l.data=c:(l.src=c,l.type=a),l.width=l.height="0",l.onerror=l.onload=l.onreadystatechange=function(){k.call(this,r)},p.splice(e,0,u),"img"!=a&&(r||2===y[c]?(t.insertBefore(l,s?null:n),m(k,j)):y[c].push(l))}function j(a,b,c,d,f){return q=0,b=b||"j",e(a)?i("c"==b?v:u,a,b,this.i++,c,d,f):(p.splice(this.i++,0,a),1==p.length&&h()),this}function k(){var a=B;return a.loader={load:j,i:0},a}var l=b.documentElement,m=a.setTimeout,n=b.getElementsByTagName("script")[0],o={}.toString,p=[],q=0,r="MozAppearance"in l.style,s=r&&!!b.createRange().compareNode,t=s?l:n.parentNode,l=a.opera&&"[object Opera]"==o.call(a.opera),l=!!b.attachEvent&&!l,u=r?"object":l?"script":"img",v=l?"script":u,w=Array.isArray||function(a){return"[object Array]"==o.call(a)},x=[],y={},z={timeout:function(a,b){return b.length&&(a.timeout=b[0]),a}},A,B;B=function(a){function b(a){var a=a.split("!"),b=x.length,c=a.pop(),d=a.length,c={url:c,origUrl:c,prefixes:a},e,f,g;for(f=0;f<d;f++)g=a[f].split("="),(e=z[g.shift()])&&(c=e(c,g));for(f=0;f<b;f++)c=x[f](c);return c}function g(a,e,f,g,h){var i=b(a),j=i.autoCallback;i.url.split(".").pop().split("?").shift(),i.bypass||(e&&(e=d(e)?e:e[a]||e[g]||e[a.split("/").pop().split("?")[0]]),i.instead?i.instead(a,e,f,g,h):(y[i.url]?i.noexec=!0:y[i.url]=1,f.load(i.url,i.forceCSS||!i.forceJS&&"css"==i.url.split(".").pop().split("?").shift()?"c":c,i.noexec,i.attrs,i.timeout),(d(e)||d(j))&&f.load(function(){k(),e&&e(i.origUrl,h,g),j&&j(i.origUrl,h,g),y[i.url]=2})))}function h(a,b){function c(a,c){if(a){if(e(a))c||(j=function(){var a=[].slice.call(arguments);k.apply(this,a),l()}),g(a,j,b,0,h);else if(Object(a)===a)for(n in m=function(){var b=0,c;for(c in a)a.hasOwnProperty(c)&&b++;return b}(),a)a.hasOwnProperty(n)&&(!c&&!--m&&(d(j)?j=function(){var a=[].slice.call(arguments);k.apply(this,a),l()}:j[n]=function(a){return function(){var b=[].slice.call(arguments);a&&a.apply(this,b),l()}}(k[n])),g(a[n],j,b,n,h))}else!c&&l()}var h=!!a.test,i=a.load||a.both,j=a.callback||f,k=j,l=a.complete||f,m,n;c(h?a.yep:a.nope,!!i),i&&c(i)}var i,j,l=this.yepnope.loader;if(e(a))g(a,0,l,0);else if(w(a))for(i=0;i<a.length;i++)j=a[i],e(j)?g(j,0,l,0):w(j)?B(j):Object(j)===j&&h(j,l);else Object(a)===a&&h(a,l)},B.addPrefix=function(a,b){z[a]=b},B.addFilter=function(a){x.push(a)},B.errorTimeout=1e4,null==b.readyState&&b.addEventListener&&(b.readyState="loading",b.addEventListener("DOMContentLoaded",A=function(){b.removeEventListener("DOMContentLoaded",A,0),b.readyState="complete"},0)),a.yepnope=k(),a.yepnope.executeStack=h,a.yepnope.injectJs=function(a,c,d,e,i,j){var k=b.createElement("script"),l,o,e=e||B.errorTimeout;k.src=a;for(o in d)k.setAttribute(o,d[o]);c=j?h:c||f,k.onreadystatechange=k.onload=function(){!l&&g(k.readyState)&&(l=1,c(),k.onload=k.onreadystatechange=null)},m(function(){l||(l=1,c(1))},e),i?k.onload():n.parentNode.insertBefore(k,n)},a.yepnope.injectCss=function(a,c,d,e,g,i){var e=b.createElement("link"),j,c=i?h:c||f;e.href=a,e.rel="stylesheet",e.type="text/css";for(j in d)e.setAttribute(j,d[j]);g||(n.parentNode.insertBefore(e,n),m(c,0))}}(this,document),Modernizr.load=function(){yepnope.apply(window,[].slice.call(arguments,0))};
};

//var INTELNAV.jQuery_1_5_2 = INTELNAV.jQuery_1_5_1.noConflict(true);
load_jquery1_5_1();

// end jQuery 


var newBrowserDetect = function(){
	if (typeof $ != 'undefined') { $.noConflict();}
	load_jquery1_10_1();
	load_jquery_migrate1_2_1();
//	load_jquery_modernizer2_6_2();

	//INTELNAV.jQuery_1_10_1 = INTELNAV.jQuery_1_5_1;
	
	if (typeof $ != 'undefined') {hf_debug("log","2. $.fn.jquery="+$.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery != 'undefined') {hf_debug("log","2. tmpjQuery.fn.jquery="+tmpjQuery.fn.jquery, "",  isdebug);}
	if (typeof jQuery != 'undefined') {hf_debug("log","2. jQuery.fn.jquery="+jQuery.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery2 != 'undefined') {hf_debug("log","2. tmpjQuery2.fn.jquery="+tmpjQuery2.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_5_1 != 'undefined') {hf_debug("log","2. INTELNAV.jQuery_1_5_1.fn.jquery="+INTELNAV.jQuery_1_5_1.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_10_1 != 'undefined') {hf_debug("log","2. INTELNAV.jQuery_1_10_1.fn.jquery="+INTELNAV.jQuery_1_10_1.fn.jquery, "",  isdebug);}

	if (typeof tmpjQuery != 'undefined') {	
		$ = tmpjQuery;
		jQuery = tmpjQuery2;
		hf_debug("log","test1", "",  isdebug);
	} else {
		$ = undefined; //INTELNAV.jQuery_1_5_1;
		jQuery = undefined; //INTELNAV.jQuery_1_5_1;
		hf_debug("log","test1b", "",  isdebug);
		hf_debug("log","$ reset to 'undefined'", "",  isdebug);
		hf_debug("log","jQuery reset to 'undefined'", "",  isdebug);
	}
	
	if (typeof tmpjQuery2 != 'undefined') {	
		$ = tmpjQuery2;
		jQuery = tmpjQuery2;
		hf_debug("log","test2", "",  isdebug);
	} 

	if (typeof $ != 'undefined') {hf_debug("log","3. $.fn.jquery="+$.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery != 'undefined') {hf_debug("log","3. tmpjQuery.fn.jquery="+tmpjQuery.fn.jquery, "",  isdebug);}
	if (typeof jQuery != 'undefined') {hf_debug("log","3. jQuery.fn.jquery="+jQuery.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery2 != 'undefined') {hf_debug("log","3. tmpjQuery2.fn.jquery="+tmpjQuery2.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_5_1 != 'undefined') {hf_debug("log","3. INTELNAV.jQuery_1_5_1.fn.jquery="+INTELNAV.jQuery_1_5_1.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_10_1 != 'undefined') {hf_debug("log","3. INTELNAV.jQuery_1_10_1.fn.jquery="+INTELNAV.jQuery_1_10_1.fn.jquery, "",  isdebug);}

	// jquery browser plugin extracted from migration plugin
	(function (jQuery, window, undefined) {
		//"use strict";
		var matched, browser;

		jQuery.uaMatch = function (ua) {
			ua = ua.toLowerCase();

			var match = /(yabrowser)[ \/]([\w.]+)/.exec(ua) ||
				/(chrome)[ \/]([\w.]+)/.exec(ua) ||
				/(webkit)(?:.*version|)[ \/]([\w.]+)/.exec(ua) ||
				/(opera)(?:.*version|)[ \/]([\w.]+)/.exec(ua) ||
				/(msie) ([\w.]+)/.exec(ua) ||
				ua.indexOf("compatible") < 0 && /(mozilla)(?:.*? rv:([\w.]+)|)/.exec(ua) || [];
			return {
				browser: match[1] || "",
				version: match[2] || "0"
			};
		};

		// Don't clobber any existing jQuery.browser in case it's different
		if (!jQuery.browser) {
			matched = jQuery.uaMatch(window.navigator.userAgent);
			browser = {};

			if (matched.browser) {
				browser[matched.browser] = true;
				browser.version = matched.version;
			}

		   // Chrome is Webkit, but Webkit is also Safari.
			if (browser.chrome) {
				browser.webkit = true;
			} else if (browser.yabrowser) {
				browser.webkit = true;
			} else if (browser.webkit) {
				browser.safari = true;
			}

			jQuery.browser = browser;
		}

	})(INTELNAV.jQuery_1_10_1, window);

	// global variables to utilize every where
	var uA, bM, bN, bV, bvClass, OS, dE;
	var bvClass = bM = dT = wk = "";
	(function (jQuery, window, undefined) {
		dE = document.documentElement;
		uA = window.navigator.userAgent.toLowerCase();
		var matchSt = /(trident)[ \/]([\w.]+)/.exec(uA) || [],
			trident = matchSt[1] || "",
			tV = matchSt[2] || "0",
			dM = document.documentMode;

		//for all IE
		if (INTELNAV.jQuery_1_10_1.browser.msie) {
			bN = "ie";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}

		//for all IE compatible view	
		if (trident == "trident") {
			var cvReg;
			if (bV === "7.0" && tV === "4.0") {
				bM = " ie8_cv";
				cvReg = /(^|\s)ie7(\s|$)/;
			}
			if (bV === "7.0" && tV === "5.0" || bV === "8.0" && tV === "5.0") {
				bM = " ie9_cv";
				cvReg = (/(^|\s)ie7(\s|$)/)? /(^|\s)ie7(\s|$)/ : /(^|\s)ie8(\s|$)/;
			}
			cvReg = (/(^|\s)ie7(\s|$)/)? /(^|\s)ie7(\s|$)/ : /(^|\s)ie8(\s|$)/;
			dE.className = dE.className.replace(cvReg, '$1$2');
			
			if (parseFloat(bV) !== parseFloat(dM)){			
				bV = dM;
			}		
		}
		
		//for opera
		if (INTELNAV.jQuery_1_10_1.browser.opera) {
			bN = "opera";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}
		//for mozilla
		if (INTELNAV.jQuery_1_10_1.browser.mozilla) {
			bN = "ff";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}
		//for chrome
		if (INTELNAV.jQuery_1_10_1.browser.chrome) {
			bN = "chrome";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}
		//for yandex
		if (INTELNAV.jQuery_1_10_1.browser.yabrowser) {
			bN = "yandex";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}
		//for safari
		if (INTELNAV.jQuery_1_10_1.browser.safari) {
			bN = "safari";
			bV = INTELNAV.jQuery_1_10_1.browser.version;
		}

		//for safari
		if (INTELNAV.jQuery_1_10_1.browser.webkit) {
			wk = " webkit";
		}

		/* mobile devices detection */
		if (uA.indexOf("mobile") !== -1) {
			dT = " mobile";
		}
		if (uA.indexOf("iphone") !== -1) {
			bN = "m_safari";
			OS = " iphone";
		}
		if (uA.indexOf("ipad") !== -1) {
			bN = "m_safari";
			OS = " ipad";
		}
		if (uA.indexOf("android") !== -1) {
			OS = " android";
		}
		if (uA.indexOf("blackberry") !== -1) {
			OS = " blackBerry";
		}
		if (uA.indexOf("windows") !== -1) {
			OS = " win";
		}
		if (uA.indexOf("macintosh") !== -1) {
			OS = " mac";
		}

		// convert the version from "." to "_"
		bvClass = parseFloat(bV).toString().replace(/[.\s]+/g, "_");

		// set the browser details as a class name to the root element that is hrml
		dE.className += OS + " " + bN + " " + bN + bvClass + bM + dT + wk;
		
		INTELNAV.browserParameters.bM = bM;
		INTELNAV.browserParameters.bN = bN;
		INTELNAV.browserParameters.bV = bV;
		INTELNAV.browserParameters.OS = OS;
		INTELNAV.browserParameters.dT = dT;
		INTELNAV.browserParameters.wk = wk;
		INTELNAV.browserParameters.bvClass = bvClass;
		//INTELNAV.jQuery_1_5_1("body").addClass(INTELNAV.browserParameters.OS + " " + INTELNAV.browserParameters.bN + " " + INTELNAV.browserParameters.bN + INTELNAV.browserParameters.bV.replace(/[^0-9]+/g, "_"));
	INTELNAV.jQuery_1_10_1("body").addClass(INTELNAV.browserParameters.OS + " " + INTELNAV.browserParameters.bN + " " + INTELNAV.browserParameters.bN + INTELNAV.browserParameters.bvClass + INTELNAV.browserParameters.bM + INTELNAV.browserParameters.dT + INTELNAV.browserParameters.wk	);
		
		var sbV = parseInt(bV);
		if (bN == 'ff' && sbV >= 3 || bN == 'ie' && sbV >= 7 || bN == 'safari' && sbV >= 3 || bN == 'chrome' && sbV >= 5 || bN == 'yandex' && sbV >= 1) {
			INTELNAV.jQuery_1_10_1('body').addClass('new_browser');
		} else {
			INTELNAV.jQuery_1_10_1('body').addClass('old_browser');
		}
		
	})(INTELNAV.jQuery_1_10_1, window);
	
	//alert("new");
};


var oldBrowserDetect = function() {
	if (typeof $ != 'undefined') {hf_debug("log","2. $.fn.jquery="+$.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery != 'undefined') {hf_debug("log","2. tmpjQuery.fn.jquery="+tmpjQuery.fn.jquery, "",  isdebug);}
	if (typeof jQuery != 'undefined') {hf_debug("log","2. jQuery.fn.jquery="+jQuery.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery2 != 'undefined') {hf_debug("log","2. tmpjQuery2.fn.jquery="+tmpjQuery2.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_5_1 != 'undefined') {hf_debug("log","2. INTELNAV.jQuery_1_5_1.fn.jquery="+INTELNAV.jQuery_1_5_1.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_10_1 != 'undefined') {hf_debug("log","2. INTELNAV.jQuery_1_10_1.fn.jquery="+INTELNAV.jQuery_1_10_1.fn.jquery, "",  isdebug);}

	if (typeof tmpjQuery != 'undefined') {	
		$ = tmpjQuery;
		jQuery = tmpjQuery2;
		hf_debug("log","set $ = tmpjQuery and jQuery = tmpjQuery2", "",  isdebug);
	} 
	if (typeof tmpjQuery2 != 'undefined') {	
		$ = tmpjQuery2;
		jQuery = tmpjQuery2;
		hf_debug("log","set $ = tmpjQuery2 and jQuery = tmpjQuery2", "",  isdebug);
	} 

	if (typeof $ != 'undefined') {hf_debug("log","3. $.fn.jquery="+$.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery != 'undefined') {hf_debug("log","3. tmpjQuery.fn.jquery="+tmpjQuery.fn.jquery, "",  isdebug);}
	if (typeof jQuery != 'undefined') {hf_debug("log","3. jQuery.fn.jquery="+jQuery.fn.jquery, "",  isdebug);}
	if (typeof tmpjQuery2 != 'undefined') {hf_debug("log","3. tmpjQuery2.fn.jquery="+tmpjQuery2.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_5_1 != 'undefined') {hf_debug("log","3. INTELNAV.jQuery_1_5_1.fn.jquery="+INTELNAV.jQuery_1_5_1.fn.jquery, "",  isdebug);}
	if (typeof INTELNAV.jQuery_1_10_1 != 'undefined') {hf_debug("log","3. INTELNAV.jQuery_1_10_1.fn.jquery="+INTELNAV.jQuery_1_10_1.fn.jquery, "",  isdebug);}



	//----------------------------------- browser check -----------------------------------------
	var str, bN, bM, bV, cM, OS;
	str = navigator.appVersion.toLowerCase().replace(/[^a-zA-Z 0-9 . / _]+/g, "").split(" ");
	OS = str[1].substr(0, 3);
	 if (INTELNAV.jQuery_1_5_1.browser.msie) { 
		bN = "ie";
		OS = str[4].substr(0, 3);
		bV = INTELNAV.jQuery_1_5_1.browser.version;
		//trap for IE8 Compatiblity mode
		bM = document.documentMode;
		if (bV == '7.0' && bM == '8') { 
			bV = '8.0'; 
		}
	}
	if (INTELNAV.jQuery_1_5_1.browser.opera) { 
		bN = "opera"; 
		bV = INTELNAV.jQuery_1_5_1.browser.version.substr(0, 4) 
	}
	if (INTELNAV.jQuery_1_5_1.browser.mozilla) {
		var moz = navigator.userAgent.toLowerCase();
		moz = moz.replace(/[^a-zA-Z 0-9 . / _]+/g, "").split(" ");
		
		if (moz[moz.length - 1].split("/", 2)[0] == "firefox") {
			bN = "ff";
			bV = moz[moz.length - 1].split("/", 2)[1].substr(0, 3);
		} else {
			bN = "ie";
			bV = moz[moz.length - 3].substr(2, 4);
		}
	}
	if (INTELNAV.jQuery_1_5_1.browser.webkit) {
		if (str[str.length - 2].split("/", 2)[0] === "chrome") {
			bN = str[str.length - 2].split("/", 2)[0];
			bV = str[str.length - 2].split("/", 2)[1].substr(0, 4)
		} else if (str[str.length - 2].split("/", 2)[0] === "mobile") {
			bN = str[str.length - 2].split("/", 2)[0].substr(0, 1) + "_" + str[str.length - 1].split("/", 2)[0];
			bV = str[6];
			if (str[1] === "ipad") {
				bV = str[5]
			}
		} else {
			bN = str[str.length - 1].split("/", 2)[0];
			bV = str[str.length - 2].split("/", 2)[1]
		}
	}
	cM = ((document.compatMode=='CSS1Compat')?'Standards': 'Quirks');
	INTELNAV.browser = {Name:bN,Version:bV, Mode:cM};
	//alert("NAME="+INTELNAV.browser.Name+", VERSION="+INTELNAV.browser.Version+", MODE="+INTELNAV.browser.Mode);

	INTELNAV.jQuery_1_5_1("body").addClass(OS + " " + bN + " " + bN + bV.replace(/[^0-9]+/g, "_")); 
	
	//alert("old");
};

//----------------------------------- end of browser check -----------------------------------------





(function() {
	
	hf_debug("timestart","TOTALLOADTIME", "",  true);
	hf_debug("bundletimestart","headerchooser.js: (function(){})", "",  isdebug);

	// TESTHARNESS parameter overrides
	if (getParameterByName("pageWidth") != '') { INTELNAV.renderSettings.pageWidth = getParameterByName("pageWidth"); }
	if (getParameterByName("textDirection") != '') { INTELNAV.renderSettings.textDirection = getParameterByName("textDirection"); }
	if (getParameterByName("protocol") != '') { INTELNAV.renderSettings.protocol = getParameterByName("protocol"); }	
	if (getParameterByName("culture") != '') { INTELNAV.renderSettings.culture = getParameterByName("culture"); } //
	if (getParameterByName("buildType") != '') { INTELNAV.renderSettings.buildType = getParameterByName("buildType"); }
	if (getParameterByName("OutputId") != '') { INTELNAV.renderSettings.OutputId = getParameterByName("OutputId"); }

	// initialize variables
	var secure = (INTELNAV.renderSettings.protocol == "https");
	var culture = INTELNAV.renderSettings.culture.toUpperCase();
	var outputid = INTELNAV.renderSettings.OutputId;
	$ = tmpjQuery;

	hf_debug("log","INTELNAV.jQuery_1_5_1="+INTELNAV.jQuery_1_5_1.fn.jquery, "",  true);
	hf_debug("log","HTTPS="+secure, "",  true);
	hf_debug("log","CULTURE="+culture, "",  true);
	hf_debug("log","OUTPUTID="+outputid, "",  true);
	hf_debug("log","boxmodel="+document.compatMode, "",  true);
	

	
	// Process ReImagine GEO overrides
	//var processUNAVgeos = function() {
	var processUNAV1geos = function() {
		var change = false;
		switch (culture)
		{
		case "EN_AP":
			culture = "EN_XA";
			INTELNAV.renderSettings.culture = "en_XA";
			change = true;
			break;
		case "EN_HK":
			culture = "EN_XA";
			INTELNAV.renderSettings.culture = "en_XA";
			change = true;
			break;
		case "EN_IR":
			culture = "EN_IE";
			INTELNAV.renderSettings.culture = "en_IE";
			change = true;
			break;
		case "EN_NZ":
			culture = "EN_AU";
			INTELNAV.renderSettings.culture = "en_AU";
			change = true;
			break;
		case "EN_PK":
			culture = "EN_XA";
			INTELNAV.renderSettings.culture = "en_XA";
			change = true;
			break;
		case "EN_SA":
			culture = "EN_XR";
			INTELNAV.renderSettings.culture = "en_XR";
			change = true;
			break;
		case "EN_XE":
			culture = "EN_UK";
			INTELNAV.renderSettings.culture = "en_UK";
			change = true;
			break;
		case "ES_LA":
			culture = "ES_XL";
			INTELNAV.renderSettings.culture = "es_XL";
			change = true;
			break;
		case "ES_MX":
			culture = "ES_XL";
			INTELNAV.renderSettings.culture = "es_XL";
			change = true;
			break;
		case "HU_HU":
			culture = "EN_EU";
			INTELNAV.renderSettings.culture = "en_EU";
			change = true;
			break;
		case "RO_RO":
			culture = "EN_UK";
			INTELNAV.renderSettings.culture = "en_UK";
			change = true;
			break;
		case "CS_CZ":
			culture = "EN_EU";
			INTELNAV.renderSettings.culture = "en_EU";
			change = true;
			break;
		case "EN_XR":
			INTELNAV.renderSettings.textDirection = "ltr"; //this is to sync the direction with CQ
			change = true;
			break;
		case "EN_SG":
			culture = "EN_XA";
			INTELNAV.renderSettings.culture = "en_XA";
			change = true;
			break;
		case "EN_CA":
			culture = "EN_US";
			INTELNAV.renderSettings.culture = "en_US";
			change = true;
			break;
		case "FR_CA":
			culture = "FR_FR";
			INTELNAV.renderSettings.culture = "fr_FR";
			change = true;
			break;
		
		default:
			// no change
			break;
		}
		if (change) { 
			hf_debug("log","NEW CULTURE="+INTELNAV.renderSettings.culture, "",  true);
		}
	};
	
	// Convert the Variablename to string of the current Header being used
	var getHeaderType = function(tmpPathHttp) {
		for (var name in INTELNAV.headerPaths.recode50){
			//alert(name);
			//alert(obj[name]);
			if (tmpPathHttp == INTELNAV.headerPaths.recode50[name]){
				return name.replace('PathHttps','').replace('PathHttp','');
				//return tmpPathHttp+" \n "+name+" \n "+INTELNAV.headerPaths.recode50[name];
				//break;
			}
		}
	};
	
	
	// Initialize the build of the header
	var initBuild = function() {
		hf_debug("bundletimestart","headerchooser.js: initBuild()", "",  isdebug);
	
//		var forcetest = getParameterByName("forcetest");
		
		
		// remove the empty glass HTML (so it doesn't get in the way)
		INTELNAV.jQuery_1_5_1("#smallfootprint-header").html("");
		INTELNAV.jQuery_1_5_1("#smallfootprint-footer").html("");

		var currentPageUrl = document.location.href;
		var lutMatch = false; // assume we'll be rendering the LOP
		var targetEnvironmentForUrl = {}; // to hold the target environment

		
		// go to the LUT 
		var LUT = INTELNAV.UrlLookUpTable.urls;
		if ((LUT.length > 0) && (RollBack == false)) {
			for (var i = 0; i < LUT.length; i++) {
				var myregexp = new RegExp("^https?:\\/\\/" + LUT[i].site.replace(/\./g, "\\.").replace(/\?(?!!)/g, "\\?")  + ".*", "i");
				var design = LUT[i].design;
				hf_debug("log",myregexp, isdebug);
				if (myregexp.test(currentPageUrl)) {

					
					// Process Re-Imagine GEO Overrides for GEOs that are not available in CQ.
					if  (design == 1) {
						//unav1 design
						processUNAV1geos();
					} else if (design == 2) {
						//unav2 design - moved to design code
						processUNAV1geos();
					} else if ( design == 3 ) {
						//unav3 design - moved to design code
						processUNAV1geos();
					} else { 
						// legacy and default design
					}
					
					
					//Check Menu3 cultures
					if (design == 3) {
						if (ActiveCulturesMenu3.toUpperCase().search(culture) != -1) {
							hf_debug("log","DESIGN=Menu3", "",  true);
							LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unav3prodPathHttp;
							LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unav3prodPathHttps;
							lutMatch = true;
							targetEnvironmentForUrl = LUT[i];
							
							//What is the visibility state for this site?
							if (typeof LUT[i].isVisible != 'undefined') {	
								segment = "-"+LUT[i].isVisible;
								if (segment == "-footeronly") {
									INTELNAV.jQuery_1_5_1("#recode50header").addClass("u-nav-hidden").hide();
									INTELNAV.jQuery_1_5_1("#recode50footer").addClass("small");
								}
							} else {
								segment = "-both";
							}
							hf_debug("log","SEGMENT="+segment, "",  isdebug);
							
							//Do we have a search filter?
							if (typeof LUT[i].searchFilter != 'undefined') {	
								searchFilter = LUT[i].searchFilter;
							}
							hf_debug("log","searchFilter="+searchFilter, "",  isdebug);
							
							//Do we have a RWD flag?
							if (typeof LUT[i].RWD != 'undefined') {
								if (ActiveCulturesMenu3_RWD.toUpperCase().search(culture) != -1) {
									isRWD = LUT[i].RWD;
								}
							}
							if (RWD!="") {isRWD = RWD;}
							hf_debug("log","isRWD="+isRWD, "",  true);
							
							//Do we have a warning?
							/*if (typeof LUT[i].warning != 'undefined') {	
								warning = LUT[i].warning;
							}
							hf_debug("log","warning="+warning, "",  isdebug);*/
							break;
						} else {
							hf_debug("log","DESIGN=Menu3 No GEO found, falling back", "",  true);
							design = design-1;
						}
					}
					
					//Check Menu2 cultures
					if (design == 2) {
						if (ActiveCulturesMenu2.toUpperCase().search(culture) != -1) {
							hf_debug("log","DESIGN=Menu2", "",  true);
							LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unav2prodPathHttp;
							LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unav2prodPathHttps;
							lutMatch = true;
							targetEnvironmentForUrl = LUT[i];
							
							//What is the visibility state for this site?
							if (typeof LUT[i].isVisible != 'undefined') {	
								segment = "-"+LUT[i].isVisible;
								if (segment == "-footeronly") {
									INTELNAV.jQuery_1_5_1("#recode50header").addClass("u-nav-hidden").hide();
									INTELNAV.jQuery_1_5_1("#recode50footer").addClass("small");
								}
							} else {
								segment = "-both";
							}
							hf_debug("log","SEGMENT="+segment, "",  isdebug);
							
							//Do we have a search filter?
							if (typeof LUT[i].searchFilter != 'undefined') {	
								searchFilter = LUT[i].searchFilter;
							}
							hf_debug("log","searchFilter="+searchFilter, "",  isdebug);
							
							//Do we have a warning?
							/*if (typeof LUT[i].warning != 'undefined') {	
								warning = LUT[i].warning;
							}
							hf_debug("log","warning="+warning, "",  isdebug);*/
							break;
						} else {
							hf_debug("log","DESIGN=Menu2 No GEO found, falling back", "",  true);
							design = design-1;
						}
					}
					
					//Check uNav1 cultures
					if (design == 1) {
						if (ActiveCultures.toUpperCase().search(culture) != -1) {
							hf_debug("log","DESIGN=Menu1", "",  true);
							hf_debug("log","DESIGN=Menu1 is no longer used so remapping to fallback (Menu0 - legacy)", "",  true);
							design = design-1;
							/*LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unavprodPathHttp;
							LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unavprodPathHttps;
							lutMatch = true;
							targetEnvironmentForUrl = LUT[i];
							break;*/
						} else {
							hf_debug("log","DESIGN=Menu1 No GEO found, falling back", "",  true);
							design = design-1;
						}
					}
					
					// nothing found then render legacy
					if (design == 0) {
						if (ActiveCultures.toUpperCase().search(culture) != -1) {
							hf_debug("log","DESIGN=Menu0", "",  true);
							forcetest="legacyprod";
							break;
						} else {
							hf_debug("log","DESIGN=Menu0 No GEO found, resetting", "",  true);
							
						}
					}
					
					// no match for fallback so reset to default
					//if (ActiveCultures.toUpperCase().search(culture) == -1) {
					/*	hf_debug("log","CULTURE="+culture+" not found", "",  true);
						hf_debug("log","CULTURE=Resetting to Menu0 and en_US", "",  true);
						forcetest="legacyprod";
						//design = LUT[i].design;
						culture="en_US";
						INTELNAV.renderSettings.culture = culture;
						break;*/
					//}
					
					// no match for fallback so reset to en_US and requested menu design
					culture="en_US";
					INTELNAV.renderSettings.culture = culture;
					forcetest = "";
					design = LUT[i].design;
					hf_debug("log","CULTURE=Resetting to "+culture, "",  true);
					switch (design)
					{
					case "3":
						hf_debug("log","DESIGN=Resetting to Menu3", "",  true);
						LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unav3qaPathHttp;
						LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unav3qaPathHttps;
						lutMatch = true;
						targetEnvironmentForUrl = LUT[i];
						break;
						
					case "2":
						hf_debug("log","DESIGN=Resetting to Menu2", "",  true);
						LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unav2prodPathHttp;
						LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unav2prodPathHttps;
						lutMatch = true;
						targetEnvironmentForUrl = LUT[i];
						break;
						
					case "1":
					case "0":
					default:
						hf_debug("log","DESIGN=Resetting to Menu0", "",  true);
						/*LUT[i].defaultPathHttp = INTELNAV.headerPaths.recode50.unav3qaPathHttp;
						LUT[i].defaultPathHttps = INTELNAV.headerPaths.recode50.unav3qaPathHttps;*/
						break;
					}
					break;

				}
			}
		}
		
		

		var assetPath = "";
		var domain = INTELNAV.renderSettings.assetPathRoot;
		var tmpPathHttp, tmpPathHttps = "";
		var host, sslHost;

		if (lutMatch) {
			INTELNAV.jQuery_1_5_1("#recode50header").addClass("recode50unav"); // so native CSS to the page can now specifically target uNav
			INTELNAV.jQuery_1_5_1("#recode50footer").addClass("recode50unav");
		}

		switch (forcetest)
		{
		case "legacy":
			domain = INTELNAV.headerPaths.recode40.devDomain;
			INTELNAV.renderSettings.assetPathRoot = domain;
			assetPath = domain + INTELNAV.headerPaths.recode40.devfilePath;
			/*INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttpS = assetPath;*/
			INTELNAV.assetServer = assetPath;
			HeaderType='LegacyDev';
			break;
			
		case "legacyprod":
			domain = INTELNAV.headerPaths.recode40.prodDomain;
			INTELNAV.renderSettings.assetPathRoot = domain;
			assetPath = domain + INTELNAV.headerPaths.recode40.prodfilePath;
			/*INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttps = assetPath;*/
			INTELNAV.assetServer = assetPath;
			HeaderType='LegacyProd';
			break;

		case "unavdev":
			if (lutMatch) {
				if (targetEnvironmentForUrl.devPathHttp != "") {
					assetPath = targetEnvironmentForUrl.devPathHttp;
					HeaderType='uNavDevOverride';
				} 
				else {
					assetPath = INTELNAV.headerPaths.recode50.unavdevPathHttp;
					HeaderType='uNavDev';
				}
			} 
			else {
				assetPath = INTELNAV.headerPaths.recode50.unavdevPathHttp;
				HeaderType='uNavDev'
			}
			INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttps = assetPath;
			INTELNAV.assetServer = assetPath;
			break;

		case "unavtest":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.testPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavtestPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.testPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavtestPathHttps;
				}
				((secure) ? HeaderType='uNavTest(HTTPs)' : HeaderType='uNavTest');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unavtestPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unavtestPathHttps;
				HeaderType='HeaderType';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "unavdayone":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.unavdayonePathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavdayonePathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unavdayonePathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavdayonePathHttps;
				}
				((secure) ? HeaderType='uNavTest(HTTPs)' : HeaderType='uNavTest');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unavdayonePathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unavdayonePathHttps;
				HeaderType='HeaderType';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "unavhotfix":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.unavhotfixPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavhotfixPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unavhotfixPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavhotfixPathHttps;
				}
				((secure) ? HeaderType='uNavTest(HTTPs)' : HeaderType='uNavTest');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unavhotfixPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unavhotfixPathHttps;
				HeaderType='HeaderType';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "unavpreview":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.prevPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavpreviewPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.prevPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavpreviewPathHttps;
				}
				((secure) ? HeaderType='uNavPreview(HTTPs)' : HeaderType='uNavPreview');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unavpreviewPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unavpreviewPathHttps;
				HeaderType='uNavPreview';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "unavprod":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.prodPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavprodPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.prodPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavprodPathHttps;
				}
				((secure) ? HeaderType='uNavProd(HTTPs)' : HeaderType='uNavProd');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unavprodPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unavprodPathHttps;
				HeaderType='uNavProd';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "gatest1":
			tmpPathHttp = INTELNAV.headerPaths.recode50.GAtest1PathHttp;
			tmpPathHttps = INTELNAV.headerPaths.recode50.GAtest1PathHttps;
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;		
			
		case "gatest2":
			tmpPathHttp = INTELNAV.headerPaths.recode50.GAtest2PathHttp;
			tmpPathHttps = INTELNAV.headerPaths.recode50.GAtest2PathHttps;
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "gatest3":
			tmpPathHttp = INTELNAV.headerPaths.recode50.GAtest3PathHttp;
			tmpPathHttps = INTELNAV.headerPaths.recode50.GAtest3PathHttps;
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
		
		case "gatest4":
			tmpPathHttp = INTELNAV.headerPaths.recode50.GAtest4PathHttp;
			tmpPathHttps = INTELNAV.headerPaths.recode50.GAtest4PathHttps;
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
			
		case "unav2dev":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.unav2devPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav2devPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav2devPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav2devPathHttps;
				}
				((secure) ? HeaderType='uNav2Dev(HTTPs)' : HeaderType='uNav2Dev');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav2devPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav2devPathHttps;
				HeaderType='uNav2Dev';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
		
		case "unav2qa":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.unav2qaPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav2qaPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav2qaPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav2qaPathHttps;
				}
				((secure) ? HeaderType='uNav2Dev(HTTPs)' : HeaderType='uNav2Dev');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav2qaPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav2qaPathHttps;
				HeaderType='uNav2Dev';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		case "unav2prev":
			assetPath = INTELNAV.headerPaths.recode50.unav2prevPathHttp;
			HeaderType='unav2prev'
			INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttps = assetPath;
			INTELNAV.assetServer = assetPath;
			break;

		case "unav2prod":
			if (lutMatch) {
				tmpPathHttp = targetEnvironmentForUrl.unav2prodPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav2prodPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav2prodPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav2prodPathHttps;
				}
				((secure) ? HeaderType='uNav2Prod(HTTPs)' : HeaderType='uNav2Prod');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav2prodPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav2prodPathHttps;
				HeaderType='uNav2Prod';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
		
		case "unav2stage":
			assetPath = INTELNAV.headerPaths.recode50.unav2stagePathHttp;
			HeaderType='uNav2stage'
			INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttps = assetPath;
			INTELNAV.assetServer = assetPath;
			break;
		
		 case "unav2rel":
			tmpPathHttp = INTELNAV.headerPaths.recode50.unav2relPathHttp;
            tmpPathHttps = INTELNAV.headerPaths.recode50.unav2relPathHttps;
            HeaderType='uNav2Dev'
            assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
            INTELNAV.assetServer = assetPath;
			break;
		
		case "unav2test":
			HeaderType='uNav2test'
			INTELNAV.assetPathHttp = INTELNAV.headerPaths.recode50.unav2testPathHttp;
			INTELNAV.assetPathHttps = INTELNAV.headerPaths.recode50.unav2testPathHttps;
            assetPath = ((secure) ? INTELNAV.assetPathHttps : INTELNAV.assetPathHttp);
            INTELNAV.assetServer = assetPath;
			break;
			
		case "footer1dev":
			HeaderType='footer1dev'
			INTELNAV.assetPathHttp = INTELNAV.headerPaths.recode50.footer1devPathHttp;
			INTELNAV.assetPathHttps = INTELNAV.headerPaths.recode50.footer1devPathHttps;
            assetPath = ((secure) ? INTELNAV.assetPathHttps : INTELNAV.assetPathHttp);
            INTELNAV.assetServer = assetPath;
			break;
			
		case "footer1prev":
			HeaderType='footer1prev'
			INTELNAV.assetPathHttp = INTELNAV.headerPaths.recode50.footer1prevPathHttp;
			INTELNAV.assetPathHttps = INTELNAV.headerPaths.recode50.footer1prevPathHttps;
            assetPath = ((secure) ? INTELNAV.assetPathHttps : INTELNAV.assetPathHttp);
            INTELNAV.assetServer = assetPath;
			break;
			
		case "footer1prod":
			HeaderType='footer1prod'
			INTELNAV.assetPathHttp = INTELNAV.headerPaths.recode50.footer1prodPathHttp;
			INTELNAV.assetPathHttps = INTELNAV.headerPaths.recode50.footer1prodPathHttps;
            assetPath = ((secure) ? INTELNAV.assetPathHttps : INTELNAV.assetPathHttp);
            INTELNAV.assetServer = assetPath;
			break;
			
		case "unav3dev":
			if (lutMatch) {
				INTELNAV.jQuery_1_5_1("#recode50header").css('height','60px');
				tmpPathHttp = targetEnvironmentForUrl.unav3devPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav3devPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav3devPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav3devPathHttps;
				}
				((secure) ? HeaderType='uNav3Dev(HTTPs)' : HeaderType='uNav3Dev');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav3devPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav3devPathHttps;
				HeaderType='uNav3Dev';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
			
		case "unav3qa":
			if (lutMatch) {
				INTELNAV.jQuery_1_5_1("#recode50header").css('height','60px');
				tmpPathHttp = targetEnvironmentForUrl.unav3qaPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav3qaPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav3qaPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav3qaPathHttps;
				}
				((secure) ? HeaderType='uNav3Qa(HTTPs)' : HeaderType='uNav3Qa');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav3qaPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav3qaPathHttps;
				HeaderType='uNav3Qa';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;
		
		case "unav3prod":
			if (lutMatch) {
				INTELNAV.jQuery_1_5_1("#recode50header").css('height','60px');
				tmpPathHttp = targetEnvironmentForUrl.unav3prodPathHttp;
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unav3prodPathHttp;
				}
				tmpPathHttps = targetEnvironmentForUrl.unav3prodPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unav3prodPathHttps;
				}
				((secure) ? HeaderType='uNav3Prod(HTTPs)' : HeaderType='uNav3Prod');
			} 
			else {
				tmpPathHttp = INTELNAV.headerPaths.recode50.unav3prodPathHttp;
				tmpPathHttps = INTELNAV.headerPaths.recode50.unav3prodPathHttps;
				HeaderType='uNav3Prod';
			}
			assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
			INTELNAV.assetPathHttp = tmpPathHttp;
			INTELNAV.assetPathHttps = tmpPathHttps;
			INTELNAV.assetServer = assetPath;
			break;

		// Default header to render when no override is used
		default:
			if (lutMatch && document.compatMode!="BackCompat") {
				//get default path
				//if not NULL get production uNav paths
				tmpPathHttp = targetEnvironmentForUrl.defaultPathHttp; //unavprodPathHttp;
				HeaderTypeHTTP='uNavProd';
				HeaderTypeHTTPS='uNavProd(HTTPs)';
				if (tmpPathHttp == "" || tmpPathHttp == null || tmpPathHttp == "undefined") {
					tmpPathHttp = INTELNAV.headerPaths.recode50.unavprodPathHttp;
				}else{
					HeaderTypeHTTP=getHeaderType(tmpPathHttp);
				}
				tmpPathHttps = targetEnvironmentForUrl.defaultPathHttps; //unavtestPathHttps;
				if (tmpPathHttps == "" || tmpPathHttps == null || tmpPathHttps == "undefined") {
					tmpPathHttps = INTELNAV.headerPaths.recode50.unavprodPathHttps;
				}else{
					HeaderTypeHTTPS=getHeaderType(tmpPathHttp)+"(HTTPs)";
				}
				assetPath = ((secure) ? tmpPathHttps : tmpPathHttp);
				((secure) ? HeaderType=HeaderTypeHTTPS : HeaderType=HeaderTypeHTTP);
			} 
			else {
				assetPath = domain + INTELNAV.headerPaths.recode40.prodfilePath;
				if (document.compatMode!="BackCompat") {
					HeaderTypeHTTP = 'LegacyProd';
					HeaderTypeHTTPS='LegacyProd(HTTPs)';
				}else{
					HeaderTypeHTTP = 'LegacyProd(Quirks)';
					HeaderTypeHTTPS='LegacyProd(Quirks)(HTTPs)';
				}
				((secure) ? HeaderType=HeaderTypeHTTPS : HeaderType=HeaderTypeHTTP);
			}
			INTELNAV.assetPathHttp = assetPath;
			INTELNAV.assetPathHttps = assetPath;
			INTELNAV.assetServer = assetPath;
			break;
		}
		hf_debug("log","INTELNAV.assetPathHttp="+INTELNAV.assetPathHttp, "",  isdebug);
		hf_debug("log","INTELNAV.assetPathHttps="+INTELNAV.assetPathHttps, "",  isdebug);
		hf_debug("log","INTELNAV.assetServer="+INTELNAV.assetServer, "",  isdebug);
    
		// GET the host domain w/protocol
		var rx;
		rx = /(https?:\/\/[-\w.]+)+(:\d+)?(\/)?/;
		matches = INTELNAV.assetPathHttp.match(rx);
		if (matches != null) {
			host = matches[0]; // http://www.intel.com
			if (host.charAt(host.length - 1) == "/") {
				host = host.slice(0, -1);
			}
		}

		matches = INTELNAV.assetPathHttps.match(rx);
		if (matches != null) {
			sslHost = matches[0]; // https://www-ssl.intel.com
			if (sslHost.charAt(sslHost.length - 1) == "/") {
				sslHost = sslHost.slice(0, -1);
			}

		}
		INTELNAV.assetServer = ((secure) ? sslHost : host);
		hf_debug("log","INTELNAV.assetServer="+INTELNAV.assetServer, "",  isdebug);

		
		// Process Re-Imagine GEO Overrides for GEOs that are not available in CQ.
		if  (assetPath.search('recode50.js') != -1) {
			//unav1 design
			processUNAV1geos();
			oldBrowserDetect();
		} else if (assetPath.search('unav2') != -1) {
			//unav2 design - moved to design code
		} else if ( assetPath.search('unav3') != -1) {
			//unav3 design - moved to design code
		} else { 
			// legacy and default design
			oldBrowserDetect(); 
		}

		
         // load the entry point javascript
		INTELNAV.jQuery_1_5_1.ajax({
			async: false,
			url: assetPath,
			type: 'GET',
			dataType: 'script',
			cache: true,
			timeoutNumber: 1000
		});
		
		hf_debug("bundletimeend","headerchooser.js: initBuild()", "",  isdebug);
	};


	
	// Initialize the Header/Footer Build
	initBuild();

	//Set the Google Analytic values - PART 2
	INTELNAV.jQuery_1_5_1(document).ready(function() {
		_gaq.push(['GAtracker._setCustomVar', 2, 'HeaderChosen', HeaderType+'.INT.50recode', 3]); //LegacyProd.50recode
		_gaq.push(['GAtracker._setCustomVar', 3, 'HeaderSubNav', outputid+'.'+HeaderType+'.INT.50recode', 3]); //INTELNAV.renderSettings.OutputID+'.LegacyProd.50recode'
		_gaq.push(['GAtracker._setCustomVar', 4, 'HeaderGEO', culture+'.'+HeaderType+'.INT.50recode', 3]); //INTELNAV.renderSettings.culture+'.'+INTELNAV.renderSettings.OutputID+'.LegacyProd.50recode'
		_gaq.push(['GAtracker._setCustomVar', 5, 'FooterChosen', INTELNAV.renderSettingsFooter.OutputId+'.'+HeaderType+'.INT.50recode', 3]); //INTELNAV.renderSettingsFooter+'.'+INTELNAV.renderSettings.culture+'.'+INTELNAV.renderSettings.OutputID+'.LegacyProd.50recode'
		_gaq.push(['GAtracker._trackPageview']);
		Google();	
	});	
	
	hf_debug("bundletimeend","headerchooser.js: (function(){})", "",  isdebug);	
})();

// Google Analytics to detect header - PART 3
function Google() {
	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
	ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
	var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
};

// Empty functions to support very old legacy pages. Code nested in the pages calls to these functions, which now do nothing.
function sectionOn() { } function hideAllMenus() { } function writeMenus() { }



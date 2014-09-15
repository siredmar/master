// This is a wrapper for Extron's licensed flowplayer
// function extronFlowPlayerStreaming() plays videos from a streaming server.
// function extronVideo() gets video data from database and creates video player and bookmarks on the web page. (recommended)

var flowplayerJS = "/inc/jquery/jqueryTools/flowplayer-3.2.11.min.js";
var flowplayeripad = "/inc/jquery/jqueryTools/flowplayer.ipad-3.2.12.min.js";
var flowplayerJSLoaded = false;
var flowplayerKey = "#@6d6da12527716a64dd9";
var flowplayerSWF = "/inc/jquery/jqueryTools/flowplayer.commercial-3.2.15.swf";
var flowplayerRTMP = "/inc/jquery/jqueryTools/flowplayer.rtmp-3.2.11.swf";
var contentPlugin = "/inc/jquery/jqueryTools/flowplayer.content-3.2.8.swf";
var captionsPlugin = "/inc/jquery/jqueryTools/flowplayer.captions-3.2.9.swf";

function extronFlowplayerStreaming(targetId, rtmpUrl, clipUrl, httpUrl, subtitleLang, autoStart, allowfullscreen, showplayagain, videoHeight, view, playagaintrans, videoId) {
    // These are effectively private functions...
    // We don't want getFlowPlayer, popIn or popReturn exposed to anything outside of extron.flowplayer
    function getFlowPlayer() {
        // specify param playagain 1 if you want the play again button to show at end of video. playagain is hidden by default.
        var playagainparam = (typeof (showplayagain) != "undefined") ? showplayagain : 0;
        var whichView = (typeof (view) != "undefined") ? view : "";    // only set for tradeshow
        var hasSubtitle = false;
        var subtitlebarHeight = 50;
        var playagaintranslation = (typeof (playagaintrans) != "undefined") ? playagaintrans : "Play again";
        
        if (autoStart) {
            $("#" + targetId).html(""); //empty inner html so flowplayer will autostart
        };

        return $f(targetId, { src: flowplayerSWF, wmode: true }, {
            key: flowplayerKey,
            clip: {
                provider: 'rtmp',
                url: clipUrl,
                ipadUrl: httpUrl,
                captionsTrackFilter: "",
                videoid: 0, // custom property (video id of the clip) - will be set during the playlistclip_changed event in playvideo.js
                cuepointscreated: [], // custom property (already tracked cuepoints for the clip) - will be updated in clip.onMetaData event
                scaling: 'fit',
                autoPlay: autoStart,
                autoBuffering: true,
                onBegin: function (clip) {
                    $(document).trigger("playlistclip_changed", { clip: clip });  // Raise a custom event to notify the document that the current clip has changed.
                },
                onStart: function () {
                    this.getPlugin("play").css({ opacity: 1 });
                    if (hasSubtitle == true) {  // subtitle in user language found                        
                        this.getPlugin("subtitles").show();     // show subtitles
                        if (whichView == "videogrid") {
                            this.getScreen().css({ bottom: subtitlebarHeight, height: parseInt(videoHeight) - subtitlebarHeight });   // resize video screen                            
                        }
                        else {
                            this.getScreen().css({ bottom: 25 });   // reposition video screen
                        }
                        this.getPlugin("controls").css({ bottom: subtitlebarHeight });      // reposition control bar                        
                    }
                    else {
                        this.getPlugin("subtitles").hide();     // hide subtitles
                        this.getScreen().css({ bottom: 0 });    // reposition video screen
                        this.getPlugin("controls").css({ bottom: 0 });  // reposition control bar                       
                    }
                },
                onFinish: function () {
                    if ($(".morevideoslink").is(":visible")) {
                        if ($('.playlistitem').length > 1 && !$('.playlistitem:last').hasClass('active')) { return false }; // If we have a playlist and aren't on the last video, don't show more videos.
                        this.getPlugin("play").css({ opacity: playagainparam }); setTimeout("$('.morevideoslink').click();", 2000);
                    }
                },
                onMetaData: function () {   // this function gets called before clip.onStart()                    
                    // determine whether subtitle in user language exists (look at video clip's track meta data)
                    var clip = this.getClip();
                    var trackInfo = clip.metaData.trackinfo;
                    var trackId;
                    for (var i = 0; i < trackInfo.length; i++) {
                        if (trackInfo[i].language == subtitleLang && trackInfo[i].sampledescription[0].sampletype == "tx3g") { // subtitle in user language found
                            hasSubtitle = true;
                            trackId = i + 1;    // get corresponding track ID
                            break;
                        }
                    }

                    if (hasSubtitle == true) {  // subtitle in user language found
                        // show subtitles
                        clip.update({ 'captionsTrackFilter': 'trackid=' + trackId });   // set trackfilter
                        if (whichView != "videogrid") { $("#" + targetId).height(parseInt(videoHeight) + subtitlebarHeight); }     // resize video container
                    }
                    else {
                        $("#" + targetId).height(videoHeight);  // resize video container
                    }

                    // video view tracking
                    var t = clip.fullDuration;
                    if (clip.videoid == 0) { clip.update({ "videoid": videoId }); }  // will be 0 if there is no play list
                    if (typeof t !== "undefined") {
                        clip.onCuepoint(
                            [
                                // Cuepoints are set in milliseconds, fullDuration returns seconds, hence the 1000x multiplier to get percent
                                { time: 1000, percent: "0" }, // 1 second after start
                                { time: t * 250, percent: "25" },
                                { time: t * 500, percent: "50" },
                                { time: t * 750, percent: "75" },
                                { time: (t * 1000) - 5000, percent: "100" } // 5 seconds before the end
                            ],
                            function (clip, cuepoint) {
                                // The Captions/Subtitles plugin causes multiple cuepoint triggers, regardless if there are actual subtitles. Only track percentages once.
                                if ($.inArray(cuepoint.percent, clip.cuepointscreated) == -1) {
                                    $.ajax({ url: "/video/vidtrack.ashx?id=" + clip.videoid + "&percent=" + cuepoint.percent + "&lang=" + subtitleLang + "&subtitles=" + hasSubtitle, cache: false });

                                    // Once a percent is tracked, place it in clip.cuepointscreated so it won't be tracked again
                                    clip.cuepointscreated.push(cuepoint.percent); 
                                    clip.update({ "cuepointscreated": clip.cuepointscreated });
                                }
                            }
                        );
                    }

                }
            },
            play: {
                replayLabel: playagaintranslation
            },
            canvas: { backgroundGradient: "none" },
            screen: {
                bottom: 0      // position video screen at distance X from the bottom edge of the layer
            },
            plugins: {
                rtmp: { url: flowplayerRTMP, netConnectionUrl: rtmpUrl },
                controls: { bottom: 0 },
                captions: {
                    url: captionsPlugin,
                    captionTarget: "subtitles",
                    button: null    // hide cc-button initally
                },
                subtitles: {
                    url: contentPlugin,
                    bottom: 0,
                    width: "100%",
                    height: subtitlebarHeight,
                    backgroundColor: "#000000",
                    backgroundGradient: "none",
                    borderRadius: 0,
                    border: 0,
                    style: {
                        body: {
                            fontSize: 14,
                            fontFamily: "verdana,arial,helvetica,sans-serif",
                            textAlign: "center",
                            color: "#ffffff"
                        }
                    },
                    display: "none"     // hide subtitles initially
                }
            },
            onBeforeFullscreen: function () {
                if (!allowfullscreen) {
                    if ($("#" + targetId).hasClass("docked")) {
                        popIn();
                    } else {
                        popReturn();
                    }
                    return false;
                }
            }
        }).ipad().play();
    }        

    function popIn() {
        $('body').append('<div id="fade"></div>'); //Add the fade layer to bottom of the body tag.
        $('#fade').css({ 'filter': 'alpha(opacity=80)' }).fadeIn(); //Fade in the fade layer - .css({'filter' : 'alpha(opacity=80)'}) is used to fix the IE Bug on fading transparencies
        $('#fade').click(function () { popReturn(); return false; });
        $("#" + targetId).removeClass("docked").addClass("popped");

        //center popup
        var popMargTop = ($("#" + targetId).height()) / 2;
        var popMargLeft = ($("#" + targetId).width()) / 2;
        $("#" + targetId).css({ 'margin-top': -popMargTop, 'margin-left': -popMargLeft });
    }

    function popReturn() {
        $("#" + targetId).css({ 'margin-top': '', 'margin-left': '' });
        $("#" + targetId).removeClass("popped").addClass("docked");
        $('#fade').fadeOut("fast", function () { $('#fade').remove() });
    }

    if (!flowplayerJSLoaded) {
        $.getScript(flowplayerJS, function () {
            $.getScript(flowplayeripad, function () {
                flowplayerJSLoaded = true;
                return getFlowPlayer();
            });
        });
    } else {
        return getFlowPlayer();
    }
}


function extronVideo(videoPlayerId, videoId, autoStart, allowFullscreen, useOrigSize, bookmarksId, firstBKItemText, firstBKItemVal, titleId, descId) {
    //USAGE: * = required
    // videoPlayerId* - the html id of the <div id=xxxx></div> element where the video will be inserted
    // videoId* - the FileID of the video
    // autoStart* - true/false. whether or not to start playing the video automatically
    // allowFullscreen* - true/false.
    //                  true: player's fullscreen button will make the player take over the entire screen.
    //                  false: player's fullscreen button will make the player pop-in over current page and dim the background
    // useOrigSize* - true/false.
    //                  true: if Dimensions data is stored, the videoPlayerId <div></div> element will be resized to those dimensions.
    //                  false: videoPlayerId will not be resized and whatever dimensions are currently set in the <div></div> will be retained.
    //
    // bookmarksId - the html id of the <select></select> element where video bookmarks (if any) will be listed
    // firstBKItemText - if set, the first dropdown item will show this text, setting this requires firstBKItemVal to be set as well.
    // firstBKItemVal - the value of the first dropdown item. recommended value is "0" including quotes.
    // titleId - the html id of the element where the Title text should be displayed
    // descId - the html id of the element where the Description text should be displayed
    
    var target = $("#" + videoPlayerId);

    //Get video data via json
    $.getJSON("/video/videodata.ashx?id=" + videoId, function (video) {
        if (video.length == 0 || video.IsLoaded == false) {
            target.hide();
        } else {

            //Show Bookmarks
            if (bookmarksId) {
                var bkSelect = $("#" + bookmarksId);
                bkSelect.empty();
                if (video.Bookmarks.length > 0) {
                    if (firstBKItemText) { bkSelect.append($("<option></option").attr("value", firstBKItemVal).text(firstBKItemText)); }
                    bkSelect.change(function () { $f(videoPlayerId).seek($(this).val()); });
                    $.each(video.Bookmarks, function () {
                        bkSelect.append($("<option></option>").attr("value", this.Seconds).text(this.Description));
                    });
                    bkSelect.show();
                } else {
                    bkSelect.hide();
                }
            }

            //set the width and height if desired and available
            if ((useOrigSize) && (video.Width > 0 & video.Height > 0)) {
                target.width(video.Width);
                target.height(video.Height);
            }

            //Title and Description text
            if (titleId) { $("#" + titleId).html(video.Title); }
            if (descId) { $("#" + descId).html(video.Description); }

            extronFlowplayerStreaming(videoPlayerId, video.RTMP_ServerURL, video.RTMP_ServerClip, autoStart, allowFullscreen);

        }
    });
}

function extronVideoPopIn(videoId, playlist, prioritytag) {
    var playlistparam = (typeof (playlist) != "undefined") ? '&playlist=' + playlist : '';
    var prioritytagparam = (typeof (prioritytag) != "undefined") ? '&prioritytag=' + prioritytag : ''; //prioritytag will first find related videos from the specified prioritytag before trying other tags.
    $('body').append('<div id="fade"></div>'); //Add the fade layer to bottom of the body tag.
    $('#fade').click(function () { popReturn(); return false; });
    $('body').append('<iframe id="idVidBox" frameborder="0" scrolling="no" allowtransparency="true" src="/video/playvideo.aspx?popin=1&id=' + videoId + playlistparam + prioritytagparam + '"></iframe>');
    positionFrame(); //Fade in and position the fade layer and iframe.
    $("#idSlideshowContainer, #idLandingSlides, #idTooltipBox").trigger('mouseenter'); //Trigger mouseenter events for hompeage slideshows and landing slides so they dont autoadvance
}

function isipad() {
    return (navigator.platform.indexOf("iPad") != -1);
}

function noVidTn(el) {
    el.src = "/img/movie_reel.png";
    el.onerror = "";
    return true;
}

function popReturn(needReload) {
    $('#fade').fadeOut("fast", function () { $('#fade').remove() });
    try {
        $('#idVidBox').contents().find('object').remove(); //prevent IE9/10 __flash__removeCallback error.
    } catch (err) {
        //Since not-logged-in users are redirected to https for login, accessing of .contents() would violate JS xss same origin policy (Different procol).
    }
    $('#idVidBox').fadeOut("fast", function () { $('#idVidBox').remove() });

    //Refresh the parent page if the user just logged in via the pop-in. (data-justloggedin=1 is set by /video/playVideo.aspx if needed)
    if (needReload) {
        var loc = window.location.href.split("#")[0]; //get url without hash.
        if (loc.indexOf('/product.aspx') > -1) {
            var needReload = false;
            // On the prod page, we need to add view=video to the url since videos are listed in the video tab.
            // If &view=video already exists in the url, the page won't reload so we'll have to call a manual reload.
            if (loc.indexOf('&view=video') > -1) { needReload = true; }
            window.location.replace(loc.replace('&view=video', '') + "&view=video#scrollY" + $(window).scrollTop());
            if (needReload) { window.location.reload(true); } //because if url already had &view=video, it wouldn't reload.
        } else {
            window.location.replace(loc + "#scrollY" + $(window).scrollTop());
            window.location.reload(true);
        }
    }
}

function positionFrame() {
    var fadeH = $(document).height();
    var fadeW = $(document).width();
    if (!$.support.leadingWhitespace) {
        //IE8 or less can only support opacity on height and width less than 4096
        if (fadeH > 4096) { fadeH = 4096 }
        if (fadeW > 4096) { fadeW = 4096 }
    }
    $('#fade').css({ 'filter': 'alpha(opacity=80)', 'height': fadeH, 'width': fadeW }).fadeIn(function () { //Fade in the fade layer - .css({'filter' : 'alpha(opacity=80)'}) is used to fix the IE Bug on fading transparencies
        var vidBoxH = 700; // used to be $("#idVidBox").outerHeight(), but had issues getting value.
        var vidBoxW = 1010; // used to be $("#idVidBox").outerWidth(), but had issues getting value.
        var popTop = Math.max(0, (($(window).height() - vidBoxH) / 2) + $(window).scrollTop());
        var popLeft = Math.max(0, (($(window).width() - vidBoxW) / 2) + $(window).scrollLeft());
        if (isipad()) { popTop += 50 }; //ipad needs more top padding
        $("#idVidBox").css({ 'top': popTop, 'left': popLeft }).fadeIn('fast');
    });

}

function resizeVidBox(newheight) {
    $('#idVidBox').height(newheight);
}

$(document).ready(function () {
    //Look for #scrollYnnn in the url hashtag and scroll the page. nnn is the number of pixels to scroll down
    var hash = location.hash.replace('#', '');
    if (hash.indexOf('scrollY') == 0) {
        window.location.hash = ''; // remove scrollYnnn from url. leaves a # behind (completely removing # causes entire page reload)
        setTimeout(function () { $(window).scrollTop(hash.replace('scrollY', '')); }, 150);
    }
    //Look for #playvideoXXXXXX for automatic video playback. XXXXXX is the support file video id.
    if (hash.indexOf('playvideo') == 0) {
        //a video needs to play automatically. 
        // Only do this if there isn't another video window open
        if ($('#idVidBox').length == 0) {
            var playvidId = parseInt(hash.replace('playvideo', ''));
            extronVideoPopIn(playvidId);
        }
    }

    $(window).on('resize',positionFrame)
        .on('message', function (e) {
        var message = JSON.parse(e.originalEvent.data);
        if (message.command == 'popReturn') {
            popReturn(message.needReload);
        }
    });
});
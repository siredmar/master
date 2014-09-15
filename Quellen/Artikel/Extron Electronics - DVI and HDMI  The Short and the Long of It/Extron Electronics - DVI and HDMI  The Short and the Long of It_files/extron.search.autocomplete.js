var autoc;
var updownpressed;
var resultclicked;
var origquery;

$().ready(function () {
    if (navigator.userAgent.toLowerCase().indexOf("msie 6") == -1) //only enable autocomplete if not IE6.
    {
        $("#ctl00_idExtronHeader_idSearchString").autocomplete("/s.ashx", {
            delay: 150,
            matchSubset: false,
            selectFirst: false,
            scrollHeight: 750,
            width: 200,
            formatItem: function (data, pos, tot, value) {
                if (data[2] == '4') {
                    //"See all results" option
                    return "<div style='margin-left:55px;'>" + data[0] + "</div>";
                }
                else if (data[4] != '') {
                    return "<img src='/product/img/" + data[3] + "_" + data[4] + ".jpg' border='0' width='50' style='float:left;' onerror='ImgError(this);' /><div style='margin-left:55px;'>" + data[0] + "<div class='ac_subtext'>" + data[1] + "</div></div>";
                }
                else {
                    return "<img src='/product/img-sm/" + data[3] + "-sm.jpg' border='0' width='50' style='float:left;' onerror='ImgError(this);' /><div style='margin-left:55px;'>" + data[0] + "<div class='ac_subtext'>" + data[1] + "</div></div>";
                }
            }
        });

        $("#ctl00_idExtronHeader_idSearchString").result(function (event, data, formatted) {
            if (data) {
                autoc = 1;
                var destUrl = '/product/product.aspx?id=' + data[3];
                if (data[2] == '2') destUrl += '&allparts=1';
                if (data[4] != '') { destUrl += '&search=' + data[4];}
                if (data[2] == '4') {
                    //"See all results" option
                    destUrl = '/search.aspx?search=' + data[3];
                    this.value = data[3];
                }
                if (!updownpressed && !resultclicked) {
                    //if neither updown arrows nor mouseclicked results, then send user to search page with original query.
                    this.value = origquery;
                    destUrl = '/search.aspx?search=' + encodeURIComponent(origquery);
                }
                nextpageLoading();
                top.location = destUrl + '&s=5';
            };
        });
    };

    $("#ctl00_idExtronHeader_idSearchString").bind("keypress", function (event) {
        if (event.keyCode == 13) {
            //prevent the default action from firing (form submit to current page, which just makes it postback to self)
            event.preventDefault();
            //firefox insists on firing the keypress event before our result. IE and Chrome and Safari work as expected.
            //Delay the search submit and check for autocomplete selection before searching
            setTimeout(function () {
                if (!autoc) {
                    nextpageLoading();
                    top.location = "/search.aspx?search=" + encodeURIComponent($("#ctl00_idExtronHeader_idSearchString").val());
                };
            }, 100);
        }
    });

    $("#ctl00_idExtronHeader_idSearchString").bind("keyup", function (event) {
        if (event.keyCode == 38 || event.keyCode == 40) {
            updownpressed = 1;
        };
        origquery = $("#ctl00_idExtronHeader_idSearchString").val();
    });

    $(".ac_over").live("mousedown", function () {
        resultclicked = 1;
    });
});

function nextpageLoading() {
    $(".ac_results").hide();
}

function ImgError(source) {
    source.src = "/img/spacer.gif";
    source.onerror = "";
    return true;
}
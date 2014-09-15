$j(document).ready(function () {


//testing search
var url = location.pathname;

   if($j("meta[property='og:site_name']").attr("content") == "GCG Hub"){

//if (url.indexOf("intel_newsroom/gcg_hub") > -1){   //search


           //add chipshot graphic
           if($j("#jive-widget-container_2").find(".jive-widget-rsssubscriptionwidget").length){
                var prShotID = $j("#jive-widget-container_2").find(".jive-widget-rsssubscriptionwidget").attr("id").split("_")[1];//get just id
               $j("#jive-widgetframe-header_"+prShotID).removeClass("jive-box-header ").addClass("prShotsGCG");
               $j("#jive-widgetframe-title_"+prShotID).html("");//blankout title
        }


  if(getParameterByName("isSearch").length){
    document.write("<link rel='stylesheet' href='http://www.intel.com/newsroom/assets/css/mysearch.css' /> ");
    var strSearchThisForm = '<div id="controller">'
                        +'<h4 class="jive-widget-handle"><span><b>MCM Q&A</b></span></h4><br/><label>Search Input: '
                        +'<input type="text" id="term">'
                        +'</label>&nbsp;'
                        +' '
                        +'&nbsp;Search by: &nbsp;<input type="radio" name="category" value="question">Topic&nbsp; '
                        +'<input type="radio" name="category" value="answer">Contact&nbsp;'
                        +'<!--input type="radio" name="category" value="title">Title&nbsp;'
                        +'<input type="radio" name="category" value="by">By&nbsp;'
                        +'<select name="category" id="category">'
                        +'<option value="none">Please Select a Type</option> '
                        +'<option value="question">Question</option> '
                        +'<option value="answer">Answer</option> '
                        +'<option value="title">Title</option> '
                        +'<option value="by">Contact</option> '
                        +' </select-->'
                        +'  '
                        +' <input name="Search" type="button" id="searchButton" value="Search">'
                        +'</div><div id="result">&nbsp;</div>';


                        $j("#jive-widget-container_1").html("<div style='margin:10px'>"+strSearchThisForm+"</div>");

  	//GLOBAL VAR
	var XMLSource = "http://newsroom.intel.com/servlet/JiveServlet/previewBody/3371-102-1-6062/test-q.xml";//$j('#data').attr('xmlData');
	var keyword = '';
	var catType = '';
	var pub = '';

	var i = 0;

	$j("#searchButton").click(function() {
		keyword = $j("input#term").val();
		//catType = $j("#category option:selected").val();
        catType =$j('input[name=category]:checked').val();

		//Reset any message
		var errMsg = '';
		pub = '';

		if ( keyword == '' ) { errMsg += 'Please enter a search term' + '\n'; }
		else if ( catType == 'none' ) { errMsg += 'Please select a category' + '\n'; }
		else { if(catType != undefined){searchThis();}else{errMsg += "Please select a category"} }

		if ( errMsg != '' ) {
			pub += '<div class="error" style="padding: 4px; background: #ffebe8; border: 1px #eb8a71 solid;">' + '\n';
			pub += errMsg;
			pub += '</div>' + '\n';
		}

		//Show error
		$j('#result').html( pub );

	});

	function searchThis() {
		$j.ajax({
			type: "GET",
			url: XMLSource,
			dataType: "xml",
			success: function(xml) { loadPublication (xml) }
		});
	}

	function loadPublication (xmlData) {
		i = 0;
		var row;

		var searchExp = "";

		$j(xmlData).find('index').each(function(){

			var question = $j(this).find('Question').text();
			var answer = $j(this).find('Answer').text();
			var title = $j(this).find('Title').text();
            var by = $j(this).find('By').text();
            var url = $j(this).attr('Url');
            var d =  $j(this).attr('Date');


			//Format the keyword expression
			var exp = new RegExp(keyword,"gi");
            var expURL = new RegExp("intel_newsroom","gi");

			//Check if there is a category selected;
			//if not, use height column as a default search
			if ( catType == 'question' ) { searchExp = question.match(exp); }
			else if ( catType == 'answer' ) { searchExp = answer.match(exp); }
			else if ( catType == 'title' ) { searchExp = title.match(exp); }
            else if ( catType == 'by' ) { searchExp = by.match(exp); }

			if ( searchExp != null ) {

				//Start building the result
				if ((i % 2) == 0) { row = 'even';pub += '<tr class="row ' + row + '" style="background-color: #fff;">' + '\n'; }
				else { row = 'odd';pub += '<tr class="row ' + row + '" style="background-color: #F0F0F6;">' + '\n'; }

				i++;

				//pub += '<tr class="row ' + row + '">' + '\n';
				pub += '<td valign="top" class="col1" style="padding: 5px;">' + question + '</td>' + '\n';
				pub += '<td valign="top" class="col2" style="padding: 5px;"><div style="height:65px;width:90%;overflow:auto;" class="answerTxtContainer">' + answer + '</div></td>' + '\n';
			   	pub += '<td valign="top" class="col3" style="padding: 5px;" align="center">';

                if(url.match(expURL)!=null){
			   	    pub += '<b>Title:</b> <a href=' + url + ' target=_blank>'+title+"</a><br/><b>Release Date:</b> "+ d + "<br/><b>Contact:</b> "+by;
                }else{
                    pub += '<b>Title:</b> '+title+"<br/><b>Release Date:</b> "+ d + "<br/><b>Contact:</b> "+by;
                }
				pub += '<br/>Share:&nbsp;<a href="mailto:?subject='+title+'&body=Title: '+title+'%0D%0AContact: '+by+'%0D%0ARelease Date: '+d+'%0D%0A%0D%0AQuestion: '+encodeURIComponent(question)+'%0D%0AAnswer:'+encodeURIComponent(answer)+'"><img src="http://www.intel.com/newsroom/assets/images/gbl-nav-email.png" class="answerTxt" border=0></a></td>\n</tr>' + '\n';
			}
		});

		if ( i == 0 ) {
			pub += '<div class="error" style="padding: 4px; background: #ffebe8; border: 1px #eb8a71 solid;">' + '\n';
			pub += 'No Result was Found' + '\n';
			pub += '</div>' + '\n';

			//Populate the result
			$j('#result').html( pub );
		}
		else {
			//Pass the result set
			showResult ( pub );
		}
	}

	function showResult (resultSet) {

		//Show the result
		pub = '<div class="message" style="padding: 4px; background: #e8ffec; border: 1px #79ef9d solid;">There are ' + i + ' results!</div>';
		pub += '<table id="grid" border="0" style=" width: 100%; margin: 10px 0 0;">' + '\n';
		pub += '<thead><tr>' + '\n';
		pub += '<th class="col1" style="background: url(http://www.intel.com/newsroom/assets/images/small.gif) no-repeat #E6EEEE right center; cursor: pointer; padding: 4px;">Question</th>' + '\n';
		pub += '<th class="col2" style="background: url(http://www.intel.com/newsroom/assets/images/small.gif) no-repeat #E6EEEE right center; cursor: pointer; padding: 4px;">Answer</th>' + '\n';
		pub += '<th class="col3" style="background: url(http://www.intel.com/newsroom/assets/images/small.gif) no-repeat #E6EEEE right center; cursor: pointer; padding: 4px;">Details</th>' + '\n';
		pub += '</tr></thead>' + '\n';
		pub += '<tbody>' + '\n';

		pub += resultSet;

		pub += '</tbody>' + '\n';
		pub += '</table>' + '\n';

		//Populate
		$j('#result').html(pub)
	}
  }
}
        });
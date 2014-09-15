var topVideo = "";
var fromVidList = 0;
function Pager(tableName, itemsPerPage) {

    this.tableName = tableName;
    //alert(this.tableName)
    this.itemsPerPage = itemsPerPage;
    this.currentPage = 1;
    this.pages = 0;
    this.inited = false;

    this.showRecords = function(from, to) {
      var thisItem = 1;

        $j(".playlistthumbs > li").each(function(){
          //alert($j(this).html())
           if (thisItem < from || thisItem > to){
                $j(this).css("display", "none");
           } else    {
                $j(this).css("display", "");
                if(thisItem == fromVidList){
                    topVideo = $j(this).find("a").attr("href");
                   playFirstVideo();
                }
           }
           thisItem +=1;
        })
    }

    this.showPage = function(pageNumber) {
     // alert("test")
    	if (! this.inited) {
    		alert("not inited");
    		return;
    	}

			var oldPageAnchor = document.getElementById(tableName+'1-'+this.currentPage);
			if (oldPageAnchor!=null) oldPageAnchor.className = 'pg-normal';
			var oldPageAnchor2 = document.getElementById(tableName+'2-'+this.currentPage);
        	if (oldPageAnchor2!=null) oldPageAnchor2.className = 'pg-normal';

        	this.currentPage = pageNumber;
        	var newPageAnchor = document.getElementById(tableName+'1-'+this.currentPage);
			if (newPageAnchor!=null) newPageAnchor.className = 'pg-selected';
        	var newPageAnchor2 = document.getElementById(tableName+'2-'+this.currentPage);
        	if (newPageAnchor2!=null) newPageAnchor2.className = 'pg-selected';


        	var from = (pageNumber - 1) * itemsPerPage + 1;
        	var to = from + itemsPerPage - 1;
            fromVidList = from
        	this.showRecords(from, to);

    }

    this.prev = function() {
      //alert("test")
        if (this.currentPage > 1)
            this.showPage(this.currentPage - 1);
    }

    this.next = function() {
        if (this.currentPage < this.pages) {
            this.showPage(this.currentPage + 1);
        }
    }
    // alert(thisItem)
    this.init = function() {
       // var rows = document.getElementById(tableName).rows;
        var records = ($j(".playlistthumbs > li").length);
        this.pages = Math.ceil(records / itemsPerPage);
        $j(".playlistvideocount").html(records+" items")
        this.inited = true;
    }

    this.showPageNav = function(pagerName) {
    	if (! this.inited) {
    		alert("not inited");
    		return;
    	}
    	//var element1 = document.getElementById(positionId1);

		var showBtn;

		var pagerHtml2 = '<span onclick="' + pagerName + '.prev();ScrollToTop();" class="pg-normal leftarrowpgr"></span>';

		for (var page = 1; page <= this.pages; page++) {
            //pagerHtml += '<span id="' + tableName + '1-'+page+'" class="pg-normal" onclick="' + pagerName + '.showPage(' + page + ');">' + page + '</span> | ';
			pagerHtml2 += '<span id="' + tableName + '2-'+page+'" class="pg-normal" onclick="' + pagerName + '.showPage(' + page + ');ScrollToTop();">' + page + '</span>';
			if (page > 1) showBtn="show";
			if (page < this.pages) pagerHtml2 += ' &#149; ';
        }

		pagerHtml2 += '</span><span onclick="'+pagerName+'.next();ScrollToTop();" class="pg-normal rightarrowpgr">';

		if (!showBtn) {
			//pagerHtml="";
			pagerHtml2="";
		}

       // element1.innerHTML = pagerHtml2;
       //$j("<div style='width:150px'>"+pagerHtml2+"</div>").insertAfter(".thumbs");
       $j(".playlistpager").html(pagerHtml2)

    }
}
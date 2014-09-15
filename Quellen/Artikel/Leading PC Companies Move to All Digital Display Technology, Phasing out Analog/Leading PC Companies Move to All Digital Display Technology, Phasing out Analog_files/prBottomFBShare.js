
if(jQuery("#prMasterContainer").length > 0) {

  var prContactID = jQuery('#prMasterContainer').attr('class');

  if (!prContactID){prContactID = "corporatePR";}

  var prContactURL = '<div class="prContactID" style="margin-top:15px;clear:both;padding-top:5px"></div>';

  jQuery(prContactURL).insertAfter("#prMasterContainer");


  jQuery(".prContactID").click(function (e) {

      contactWindow = window.open('http://www.intel.com/newsroom/assets/2014/prContactPR.htm?c='+prContactID, 'Contact_Intel_PR', 'width=512, height=280', 'scrollbars=0');

  });

}

/*
(function($){
    $.wtMeta = function(){
        var $Meta;
        if($('meta[name='+arguments[0]+']').size()>0){
            $Meta=$('meta[name='+arguments[0]+']');
        }else{
            $Meta=$('meta[http-equiv='+arguments[0]+']');
        }

        if(arguments.length==1) return $Meta.attr('content');
        else $Meta.attr('content',arguments[1]);
    };
})($);*/





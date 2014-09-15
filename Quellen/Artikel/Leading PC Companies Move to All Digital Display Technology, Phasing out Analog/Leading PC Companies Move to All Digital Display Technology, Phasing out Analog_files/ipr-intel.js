$j(document).ready(function(){  
    /*
    $j("ul.topnav li a").live('click', function() { 
   
        $j(this).parent().find(".subnavmenujive").show(); 
        $j(this).parent().hover(
        function() {  
        },
        function(){  
            $j(this).parent().find(".subnavmenujive").hide();
        });
    });
    */
    $j("div.btn-gry-div a").live('click', function() { 
 	   
    	$j(this).parent().parent().find(".subnavmenujive").show(); 
    	$j(this).parent().find(".tip-point").show(); 
        $j(this).parent().parent().hover(
        function() {  
        },
        function(){  
            $j(this).parent().parent().find(".subnavmenujive").hide();
            $j(this).parent().find(".tip-point").hide(); 
        });
    });
    
});  
var intelSSO = {
    setCookie: function(c_name,value,exdays) {
        var exdate=new Date();
        exdate.setDate(exdate.getDate() + exdays);
        var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString()) + ";path=/";
        document.cookie=c_name + "=" + c_value;
    },
    getCookie: function(c_name) {
        var c_value = document.cookie;
        var c_start = c_value.indexOf(" " + c_name + "=");
        if (c_start == -1) {
            c_start = c_value.indexOf(c_name + "=");
        }
        if (c_start == -1) {
            c_value = null;
        } else {
            c_start = c_value.indexOf("=", c_start) + 1;
            var c_end = c_value.indexOf(";", c_start);
            if (c_end == -1) {
                c_end = c_value.length;
            }
            c_value = unescape(c_value.substring(c_start,c_end));
        }
        return c_value;
    },
    deleteCookie: function (c_name) {
        document.cookie = c_name + '=; expires=Thu, 01 Jan 1970 00:00:01 GMT;'; // default, default
        document.cookie = c_name + '=; expires=Thu, 01 Jan 1970 00:00:01 GMT; path=/;'; // default, root
        document.cookie = c_name + '=; domain=' + document.domain + '; expires=Thu, 01 Jan 1970 00:00:01 GMT;'; // domain, default
        document.cookie = c_name + '=; domain=' + document.domain + '; expires=Thu, 01 Jan 1970 00:00:01 GMT; path=/;'; // domain, root
        document.cookie = c_name + '=; domain=.' + document.domain + '; expires=Thu, 01 Jan 1970 00:00:01 GMT;'; // subdomain, default
        document.cookie = c_name + '=; domain=.' + document.domain + '; expires=Thu, 01 Jan 1970 00:00:01 GMT; path=/;'; // subdomain, root
        if (document.cookie.indexOf(c_name) >= 0) {
            throw new Error("[" + c_name + "] cookie still exists!");
        }
    }
};
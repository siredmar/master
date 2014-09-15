$j(document).ready(function() {
    $j('#j-search').prepend(jive.event.calendar.profileCalendarLink({userID: window._jive_effective_user_id}));
});
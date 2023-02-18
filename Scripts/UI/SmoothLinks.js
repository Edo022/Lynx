var ui_smooth_links = {
    init : function(){
        let links = document.getElementsByTagName("A");
        for(let i = 0; i < links.length; ++i){
            links[i].addEventListener("click", function(event){
                event.preventDefault();
                history.pushState(null, '', links[i].href);
                setup_index.on_location_changed();
                move_to_view(true);
            });
        }
    }
}
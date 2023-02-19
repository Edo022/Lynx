var ui_smooth_links = {
    init : function(){
        let links = document.getElementsByTagName("A");
        for(let i = 0; i < links.length; ++i){
            links[i].addEventListener("click", function(event){
                event.preventDefault();
                setup_index.go_to_index(links[i].href);
            });
        }
    }
}
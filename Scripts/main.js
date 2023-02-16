

let left = document.querySelector("body > left-");
let right = document.querySelector("body > right-");


// smooth = true|false
function move_to_view(smooth){
    let hash = location.hash.slice(1);
    let id = hash;
    if(hash.length == 0 || hash == null || document.getElementById(id) == null) id = "overview";

    let e = document.getElementById(id); if(!e) return;
    e.scrollIntoView({ block: "start"  , behavior: smooth ? "smooth" : "auto" });
    e = document.getElementById("index--" + id); if(!e) return;
    e.scrollIntoView({ block: "nearest", behavior: smooth ? "smooth" : "auto" });
}




function init() {
    ui_slider.init();

    setup_index.init();
    setup_fix.init();
    setup_syntax.init();
    setup_copy_syntax.init();

    summary_list.init();

    readability.init();

    ui_copy_code.init();
    //!^ Must be executed after modifying the body as changing the innerHTML recreates all the elements and removes the event listeners
}


function view(){
    // This scrolls to the correct header, which is the same the browser already scrolled to.
    // Apparently, JS is loaded after scrolling to it and adding new HTML elements messes everything up,
    // so JS has to scroll again after the new elements are loaded.
    // Anything else refuses to work
    move_to_view("smooth");

    // The id="main-mask" div is used to hide the page before js is done moving stuff around as anything else just doesn't work
    // This line removes it from the body so that the user can see the page and think it loaded flawlessly
    let e = document.getElementById('main-mask');
    e.style.pointerEvents = 'none';
    e.style.opacity = '0%';
}




function unload(){
    let e = document.getElementById('main-mask');
    e.style.pointerEvents = 'all';
    e.style.opacity = '100%';
}



